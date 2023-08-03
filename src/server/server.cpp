#include "server.h"

Server::Server(int gameMode, double gameSpeed) :
        gameMode(gameMode), gameSpeed(gameSpeed) {
    server = new QWebSocketServer("Generals.OI Server", QWebSocketServer::NonSecureMode, this);
    address = QHostAddress::Any;

    if (server->listen(address, 32767)) {
        connect(server, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
    } else {
        qDebug() << "[server.cpp] Error: Cannot listen port 32767!";
        qApp->quit();
    }
}

Server::~Server() {
    clearClient();
    server->close();
    delete server;
    delete serMap;
}

void Server::clearClient() {
    for (const auto &it: clients) {
        it.first->disconnect();
        it.first->deleteLater();
        delete it.first;
    }
    clients.clear();
}

void Server::onNewConnection() {
    QWebSocket *socket = server->nextPendingConnection();
    if (!socket) return;

    connect(socket, &QWebSocket::disconnected, [this, socket]() -> void {
        // TODO: Do not erase socket at once (Future feature: reconnect)
        if (!clients[socket].isSpect) {
            cntPlayer--;
            if (clients[socket].isReadied)
                cntReadied--;
        }
        clients.erase(socket);
        socket->deleteLater();
    });

    connect(this, &Server::sendMessage, socket, &QWebSocket::sendBinaryMessage);

    connect(socket, &QWebSocket::binaryMessageReceived, [this, socket](const QByteArray &msg) -> void {
        auto json = loadJson(msg);
        auto msgType = json.first.toString();
        auto msgData = json.second.toArray();
        if (msgType.isNull()) return;

        qDebug() << "[server.cpp] Received:" << msgType
                 << "From:" << socket->peerAddress().toString() << socket->peerPort();

        if (msgType == "Connected") {
            const int maxPlayerNum = 8;
            if (!flagGameStarted) {
                auto playerNickName = msgData.at(0).toString();
                if ((++cntPlayer) <= maxPlayerNum) {
                    clients[socket] = PlayerInfo(playerNickName, cntPlayer, cntPlayer, false, false);
                } else {
                    cntPlayer--;
                    socket->sendBinaryMessage(generateMessage("Status", {"You will enter as an spectator."}));
                    clients[socket] = PlayerInfo("[Spectator] " + playerNickName, -1, -1, true, true);
                    qDebug() << "[server.cpp] Too many players, join as spectator";
                    return;
                }
            } else {
                socket->sendBinaryMessage(generateMessage("Status", {"You will enter as an spectator."}));
                clients[socket] = PlayerInfo("[Spectator]", -1, -1, true, false);
                qDebug() << "[server.cpp] Game started, join as spectator";
                return;
            }
        } else if (msgType == "Readied") {
            if (clients[socket].isReadied) return;
            clients[socket].isReadied = true;

            if (flagGameStarted) {
                if (clients[socket].isSpect) {
                    socket->sendBinaryMessage(generateMessage("PlayerInfo", {-1, -1}));
                    socket->sendBinaryMessage(generateMessage("PlayerCnt", {cntPlayer}));
                    socket->sendBinaryMessage(baPlayersInfo);
                    socket->sendBinaryMessage(generateMessage("InitMap",
                                                              {QString::fromStdString(serMap->exportMap(true))}));
                }
            } else {
                auto idTeam = msgData.at(0).toInt();
                if (idTeam)
                    clients[socket].idTeam = idTeam;

                if ((++cntReadied) == cntPlayer && cntPlayer >= 2) {
                    flagGameStarted = true;
                    emit sendMessage(generateMessage("Status", {"Game starting!"}));
                    emit sendMessage(generateMessage("PlayerCnt", {cntPlayer}));

                    QJsonArray playersInfoData;

                    for (auto &it: clients) {
                        it.first->sendBinaryMessage(generateMessage("PlayerInfo",
                                                                    {it.second.idPlayer, it.second.idTeam}));
                        if (!it.second.isSpect) {
                            QJsonArray playerInfoData;
                            playerInfoData.push_back(it.second.nickName);
                            playerInfoData.push_back(it.second.idPlayer);
                            playerInfoData.push_back(it.second.idTeam);

                            playersInfoData.push_back(playerInfoData);
                        }
                    }

                    baPlayersInfo = generateMessage("PlayersInfo", playersInfoData);
                    emit sendMessage(baPlayersInfo);

                    // TODO: Add team information
                    std::vector<int> teamInfo;
                    for (int i = 1; i <= cntPlayer; i++)
                        teamInfo.push_back(i);

                    qDebug() << "[server.cpp] Start generating.";
                    serMap = new ServerMap(MapGenerator::randomMap(cntPlayer, cntPlayer, teamInfo));
                    qDebug() << "[server.cpp] Game map generated.";

                    emit sendMessage(generateMessage("InitMap", {QString::fromStdString(serMap->exportMap(true))}));

                    gameTimer = new QTimer(this);
                    connect(gameTimer, &QTimer::timeout, this, &Server::broadcastMessage);
                    gameTimer->start(int(500 / gameSpeed));
                }
            }
        } else if (msgType == "Chat") {
            emit sendMessage(msg);
        } else if (msgType == "Move") {
            int idPlayer = msgData.at(0).toInt();
            int startX = msgData.at(1).toInt();
            int startY = msgData.at(2).toInt();
            int deltaX = msgData.at(3).toInt();
            int deltaY = msgData.at(4).toInt();
            int flag50p = msgData.at(5).toInt();

            serMap->move(idPlayer, Point(startX, startY), deltaX, deltaY, flag50p);
        }

        if (!flagGameStarted && (msgType == "Connected" || msgType == "Readied")) {
            emit sendMessage(generateMessage("Status", {QString("Waiting (%1/%2) ...")
                                                                .arg(QString::number(cntReadied),
                                                                     QString::number(cntPlayer))}));
        }
    });
}

void Server::broadcastMessage() {
    serMap->addRound();
    auto mapInfo = QString::fromStdString(serMap->exportMap(false));
    emit sendMessage(generateMessage("UpdateMap", {mapInfo}));
    qDebug() << "[server.cpp] Message sent.";

    if (flagGameOvered) {
        disconnect(gameTimer, &QTimer::timeout, this, &Server::broadcastMessage);
        // emit something
        // transfer replay files
        qApp->quit();
    }
    flagGameOvered = serMap->gameOver();
}
