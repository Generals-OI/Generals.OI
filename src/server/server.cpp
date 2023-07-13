#include "server.h"

Server::Server(QWidget *parent) : QWidget(parent) {
    server = new QWebSocketServer("Generals.OI Server", QWebSocketServer::NonSecureMode, this);
    address = QHostAddress::Any;

    if (server->listen(address, 32767)) {
        connect(server, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
    } else {
        qDebug() << "[server.cpp] Error: Cannot listen port 32767!";
        delete this;
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

    connect(this, &Server::sendMessage, socket, &QWebSocket::sendTextMessage);

    connect(socket, &QWebSocket::textMessageReceived, [this, socket](const QString &msg) -> void {
        QString msgType = msg.section(":", 0, 0);
        qDebug() << "[server.cpp] Received:" << msgType
                 << socket->peerAddress().toString() << socket->peerPort();

        if (msgType == "Connected") {
            const int maxPlayerNum = 8;
            if (!flagGameStarted) {
                if ((++cntPlayer) <= maxPlayerNum) {
                    clients[socket] = PlayerInfo(msg.section(":", 1), cntPlayer, cntPlayer, false, false);
                } else {
                    cntPlayer--;
                    socket->sendTextMessage("Status:You will enter as an spectator.");
                    clients[socket] = PlayerInfo("[Spectator]" + msg.section(":", 1), -1, -1, true, true);
                    qDebug() << "[server.cpp] Too many players, join as spectator";
                    return;
                }
            } else {
                socket->sendTextMessage("Status:You will enter as an spectator.");
                clients[socket] = PlayerInfo("Spectator", -1, -1, true, false);
                qDebug() << "[server.cpp] Game started, join as spectator";
                return;
            }
        } else if (msgType == "Readied") {
            if (clients[socket].isReadied) return;
            clients[socket].isReadied = true;

            if (flagGameStarted) {
                if (clients[socket].isSpect) {
                    socket->sendTextMessage("PlayerInfo:-1:-1");
                    socket->sendTextMessage(QString("PlayerCnt:%1").arg(QString::number(cntPlayer)));
                    for (auto &it: clients) {
                        if (!it.second.isSpect)
                            socket->sendTextMessage(QString("PlayersInfo:%1:%2:%3")
                                                            .arg(QString::number(it.second.idPlayer),
                                                                 QString::number(it.second.idTeam),
                                                                 it.second.nickName));
                    }
                    socket->sendTextMessage(QString("InitMap:%1")
                                                    .arg(QString::fromStdString(serMap->export2Str(true))));
                }
            } else {
                auto idTeam = msg.section(":", 1, 1).toInt();
                if (idTeam) {
                    clients[socket].idTeam = idTeam;
                }

                if ((++cntReadied) == cntPlayer && cntPlayer >= 2) {
                    flagGameStarted = true;
                    emit sendMessage("Status:Game starting");
                    emit sendMessage(QString("PlayerCnt:%1").arg(QString::number(cntPlayer)));

                    for (auto &it: clients) {
                        it.first->sendTextMessage(QString("PlayerInfo:%1:%2")
                                                          .arg(QString::number(it.second.idPlayer),
                                                               QString::number(it.second.idTeam)));
                        if (!it.second.isSpect)
                                emit sendMessage(QString("PlayersInfo:%1:%2:%3")
                                                         .arg(QString::number(it.second.idPlayer),
                                                              QString::number(it.second.idTeam),
                                                              it.second.nickName));
                    }

                    // BUG: sort playerList first
                    // emit sendMessage(msgPlayerList);

                    // TODO: Add team info
                    std::vector<int> teamInfo;
                    for (int i = 1; i <= cntPlayer; i++)
                        teamInfo.push_back(i);

                    qDebug() << "[server.cpp] Start generating.";
//                    serMap = new ServerMap;
//                    *serMap = generate(cntPlayer, cntPlayer, teamInfo);
                    serMap = new ServerMap(generate(cntPlayer, cntPlayer, teamInfo));
                    qDebug() << "[server.cpp] Game map generated.";

                    QString mapInfo = QString::fromStdString(serMap->export2Str(true));
                    emit sendMessage(QString("InitMap:%1").arg(mapInfo));

                    auto *gameTimer = new QTimer(this);
                    connect(gameTimer, SIGNAL(timeout()), this, SLOT(broadcastMessage()));
                    // TODO: Find a proper way to stop the timer
                    gameTimer->start(20); // Debug - Xx faster
                }
            }
        } else if (msgType == "Chat") {
            emit sendMessage(msg);
        } else if (msgType == "Move") {
            int idPlayer = msg.section(":", 1, 1).toInt();
            int startX = msg.section(":", 2, 2).toInt();
            int startY = msg.section(":", 3, 3).toInt();
            int deltaX = msg.section(":", 4, 4).toInt();
            int deltaY = msg.section(":", 5, 5).toInt();
            int flag50p = msg.section(":", 6, 6).toInt();

            serMap->move(idPlayer, Point(startX, startY), deltaX, deltaY, flag50p);
        }

        if (!flagGameStarted && (msgType == "Connected" || msgType == "Readied")) {
            emit sendMessage(QString("Status:Waiting (%1/%2) ...")
                                     .arg(QString::number(cntReadied), QString::number(cntPlayer)));
        }
    });
}

void Server::broadcastMessage() {
    serMap->addRound();
    auto mapInfo = QString::fromStdString(serMap->export2Str(false));
    emit sendMessage(QString("UpdateMap:%1").arg(mapInfo));
    qDebug() << "[server.cpp] Message sent.";
}
