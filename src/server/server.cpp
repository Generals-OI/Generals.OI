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
    for (auto it = clients.begin(); it != clients.end(); it++) {
        auto itSocket = it.key();
        itSocket->close();
        itSocket->deleteLater();
        delete itSocket;
    }
    clients.clear();
}

void Server::onNewConnection() {
    QWebSocket *socket = server->nextPendingConnection();
    if (!socket) return;

    connect(socket, &QWebSocket::disconnected, [this, socket]() -> void {
        if (clients.find(socket) != clients.end()) {
            auto &currentClientInfo = clients[socket];
            if (!currentClientInfo.isSpect) {
                auto &lastClientInfo = clients[clientsIndex[cntPlayer]];
                lastClientInfo.idPlayer = currentClientInfo.idPlayer;
                cntPlayer--;
                if (currentClientInfo.isReadied)
                    cntReadied--;
            }
            clients.remove(socket);
        }
        socket->disconnect();
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
            if (!flagGameStarted) {
                auto playerNickName = msgData.at(0).toString();
                if (cntPlayer < maxPlayerNum) {
                    cntPlayer++;
                    clients[socket] = PlayerInfo(playerNickName, cntPlayer, cntPlayer, false, false);
                    clientsIndex[cntPlayer] = socket;
//                    teamInfo.push_back(cntPlayer);
                } else {
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
                    socket->sendBinaryMessage(baPlayersInfo);
                    socket->sendBinaryMessage(generateMessage("InitMap",
                                                              {QString::fromStdString(serMap->exportMap(true))}));
                }
            } else {
                if ((++cntReadied) == cntPlayer && cntPlayer >= 2) {
                    flagGameStarted = true;
                    emit sendMessage(generateMessage("Status", {"Game starting!"}));

                    QJsonArray playersInfoData;
                    playersInfoData.push_back(cntPlayer);

                    for (auto it = clients.begin(); it != clients.end(); it++) {
                        auto itSocket = it.key();
                        auto itInfo = it.value();
                        itSocket->sendBinaryMessage(generateMessage("PlayerInfo", {itInfo.idPlayer, itInfo.idTeam}));

                        if (!itInfo.isSpect) {
                            QJsonArray playerInfoData;
                            playerInfoData.push_back(itInfo.nickName);
                            playerInfoData.push_back(itInfo.idPlayer);
                            playerInfoData.push_back(itInfo.idTeam);

                            playersInfoData.push_back(playerInfoData);
                        }
                    }

                    baPlayersInfo = generateMessage("PlayersInfo", playersInfoData);
                    emit sendMessage(baPlayersInfo);

                    // TODO: Add team information
                    std::vector<int> alternateTeamInfo;
                    for (int i = 1; i <= cntPlayer; i++)
                        alternateTeamInfo.push_back(i);

                    qDebug() << "[server.cpp] Start generating.";
                    serMap = new ServerMap(MapGenerator::randomMap(cntPlayer, cntPlayer, alternateTeamInfo));
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
