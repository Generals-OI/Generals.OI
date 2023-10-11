#include "server.h"

extern const int maxPlayerNum;

Server::Server() {
    server = new QWebSocketServer("Generals.OI Server", QWebSocketServer::NonSecureMode, this);
    address = QHostAddress::Any;

    QApplication::setQuitOnLastWindowClosed(false);

    serverWindow = new ServerWindow;
    serverWindowFrame = new WindowFrame(serverWindow);
    serverWindowFrame->setTitle("Generals.OI - Server");
    serverWindow->setTarget(serverWindowFrame);
    serverWindowFrame->show();

    gameTimer = new QTimer(this);

    teamMbrCnt = QVector<int>(maxPlayerNum + 1);
    clientsIndex = QVector<QWebSocket *>(maxPlayerNum + 1);

    if (server->listen(address, 32767)) {
        nicknames.append("Generals.OI");
        nicknames.append("Server");
        connect(server, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
        connect(serverWindow, &ServerWindow::createServer, this, &Server::onCreateServer);
        serverWindow->showMessage("Listened to port.");
    } else {
        serverWindow->showMessage("Error: Cannot listen to port!");
        serverWindow->pbCreateServer->setEnabled(false);
        qDebug() << "[server.cpp] Error: Cannot listen to port 32767!";
    }
}

Server::~Server() {
    clearClient();
    server->close();
    delete server;
    delete serMap;
}

void Server::onCreateServer(int mode, double speed) {
    if (flagPlayed) {
        emit sendMessage(generateMessage("Rematch", QJsonArray()));
        updateStatus();
    }

    gameMode = mode;
    gameSpeed = speed;
    flagServerReadied = flagPlayed = true;
    serverWindow->showMessage("Server created.");
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

    if (!flagServerReadied) {
        socket->sendBinaryMessage(generateMessage("Status", {"Server isn't ready."}));
        socket->sendBinaryMessage(generateMessage("Disconnect", QJsonArray()));
        return;
    }

    connect(socket, &QWebSocket::disconnected, [this, socket]() -> void {
        auto itCurrent = clients.find(socket);
        if (itCurrent != clients.end()) {
            auto &currentClientInfo = itCurrent.value();
            if (!currentClientInfo.isSpec) {
                auto &lastClientInfo = clients[clientsIndex[cntPlayer]];
                lastClientInfo.idPlayer = currentClientInfo.idPlayer;
                cntPlayer--;
                if (currentClientInfo.isReadied)
                    cntReadied--;
                teamMbrCnt[currentClientInfo.idTeam]--;
            }

            for (int i = 0; i < nicknames.size(); i++)
                if (nicknames.at(i) == currentClientInfo.nickname) {
                    nicknames.removeAt(i);
                    break;
                }
            clients.erase(itCurrent);
        }
        socket->disconnect();
        socket->deleteLater();
        updateStatus();
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
                auto playerNickname = msgData.at(0).toString();
                if (!checkNickname(playerNickname)) {
                    socket->sendBinaryMessage(generateMessage("Status", {"Conflicting nickname."}));
                    socket->sendBinaryMessage(generateMessage("Disconnect", QJsonArray()));
                    return;
                }
                if (cntPlayer < maxPlayerNum) {
                    int idPlayer = ++cntPlayer;
                    int idTeam = getEmptyTeam();
                    clients[socket] = PlayerInfo(playerNickname, idPlayer, idTeam, false, false);
                    clientsIndex[idPlayer] = socket;
                    teamMbrCnt[idTeam]++;
                } else {
                    socket->sendBinaryMessage(generateMessage("Status", {"You will enter as an spectator."}));
                    clients[socket] = PlayerInfo("[Spectator] " + playerNickname, -1, -1, true, true);
                    qDebug() << "[server.cpp] Too many players, join as spectator";
                    return;
                }
            } else {
                socket->sendBinaryMessage(generateMessage("Status", {"You will enter as an spectator."}));
                clients[socket] = PlayerInfo("[Spectator]", -1, -1, true, false);
                qDebug() << "[server.cpp] Game started, join as spectator";
                return;
            }
        } else if (msgType == "ChooseTeam") {
            if (clients[socket].isSpec) return;
            auto idTeam = msgData.at(0).toInt();
            teamMbrCnt[clients[socket].idTeam]--;
            if (idTeam < maxPlayerNum)
                clients[socket].idTeam = idTeam;
            else
                clients[socket].idTeam = getEmptyTeam();
            teamMbrCnt[clients[socket].idTeam]++;
        } else if (msgType == "Readied") {
            if (clients[socket].isReadied) return;
            clients[socket].isReadied = true;

            if (flagGameStarted) {
                if (clients[socket].isSpec) {
                    socket->sendBinaryMessage(generateMessage("PlayerInfo", {-1, -1}));
                    socket->sendBinaryMessage(baPlayersInfo);
                    socket->sendBinaryMessage(generateMessage("GameMode", {gameMode}));
                    socket->sendBinaryMessage(generateMessage(
                            "InitGame", QJsonArray::fromVariantList(toVariantList(serMap->toVectorSM()))));
                }
            } else {
                if ((++cntReadied) == cntPlayer && cntPlayer >= 2) {
                    flagGameStarted = true;
                    emit sendMessage(generateMessage("Status", {"Game starting!"}));

                    std::vector<int> teamInfo(cntPlayer), newTeamId(maxPlayerNum + 1);
                    int totTeam = 0;
                    for (int i = 1; i <= maxPlayerNum; i++) {
                        while (i <= maxPlayerNum && !teamMbrCnt[i])
                            i++;
                        if (i <= maxPlayerNum)
                            newTeamId[i] = ++totTeam;
                    }
                    for (auto &player: clients)
                        player.idTeam = teamInfo[player.idPlayer - 1] = newTeamId[player.idTeam];

                    QVector<QPair<QString, int>> playersNicknames(cntPlayer);
                    QJsonArray playersInfoData;
                    playersInfoData.push_back(cntPlayer);

                    for (auto it = clients.begin(); it != clients.end(); it++) {
                        auto itSocket = it.key();
                        auto itInfo = it.value();
                        itSocket->sendBinaryMessage(generateMessage("PlayerInfo", {itInfo.idPlayer, itInfo.idTeam}));

                        if (!itInfo.isSpec) {
                            QJsonArray playerInfoData;
                            playerInfoData.push_back(itInfo.nickname);
                            playerInfoData.push_back(itInfo.idPlayer);
                            playerInfoData.push_back(itInfo.idTeam);
                            playersInfoData.push_back(playerInfoData);
                            playersNicknames[itInfo.idPlayer - 1] = {itInfo.nickname, itInfo.idTeam};
                        }
                    }

                    baPlayersInfo = generateMessage("PlayersInfo", playersInfoData);
                    emit sendMessage(baPlayersInfo);

                    qDebug() << "[server.cpp] Start generating.";
                    serMap = new ServerMap(RandomMapGenerator::randomMap(cntPlayer, totTeam, teamInfo, gameMode));
                    qDebug() << "[server.cpp] Game map generated.";

                    gameMapData = serMap->toByteArray();
                    recorder.init(playersNicknames, gameMode);

                    emit sendMessage(generateMessage("GameMode", {gameMode}));
                    emit sendMessage(generateMessage(
                            "InitGame",
                            QJsonArray::fromVariantList(toVariantList(serMap->toVectorSM()))));

                    connect(gameTimer, &QTimer::timeout, this, &Server::broadcastMessage);
                    gameTimer->start(int(500 / gameSpeed));

                    emit sendMessage(generateMessage("Chat", {"Generals.OI", QString(
                            "If you faced problems, please include the game ID \"%1\" in your feedback. "
                            "Thanks a lot! Have a good time!").arg(QString::number(RandomMapGenerator::lastSeed()))}
                    ));
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
            bool flag50p = msgData.at(5).toBool();

            serMap->move(idPlayer, Point(startX, startY), deltaX, deltaY, flag50p, gameMode);
            recorder.addRecord(idPlayer, startX, startY, deltaX, deltaY, flag50p);
        } else if (msgType == "Surrender") {
            int idPlayer = msgData.at(0).toInt();
            serMap->surrender(idPlayer);
        }

        if (!flagGameStarted && (msgType == "Connected" || msgType == "Readied" || msgType == "ChooseTeam"))
            updateStatus();
    });
}

void Server::broadcastMessage() {
    auto losers = serMap->addRound();
    recorder.addRecord(-1, 0, 0, 0, 0, false);

    for (auto i: losers)
        if (i.second == i.first) {
            emit sendMessage(generateMessage(
                    "Chat", {"Server", QString("@%1 surrendered.").arg(nicknames.at(i.first + 1))}));
            recorder.surrender(i.first);
        } else {
            emit sendMessage(generateMessage("Chat", {"Server", QString("@%1 captured @%2.")
                    .arg(nicknames.at(i.second + 1), nicknames.at(i.first + 1))}));
        }

    emit sendMessage(generateMessage("UpdateMap", QJsonArray::fromVariantList(toVariantList(serMap->exportDiff()))));
    serverWindow->showMessage(QString("Gaming ... (Round: %1)").arg(serMap->round));
    qDebug() << "[server.cpp] Message sent.";

    if (flagGameOvered) {
        // TODO: gfy1729 - add "winner"
        // emit sendMessage(generateMessage("Chat", {"Server", QString("@%1 won!").arg(serMap->winner)}));
        emit sendMessage(generateMessage("Chat", {"Server", "Please wait for the host to click \"Rematch\"."}));

        disconnect(gameTimer, &QTimer::timeout, this, &Server::broadcastMessage);
        
        // transfer replay files
        QString fileName = QString("replay_%1_%2")
                .arg(QDateTime::currentDateTime().toString("yyMMddhhmmsszzz"))
                .arg(RandomMapGenerator::lastSeed());
        QFile replayFile(fileName);
        if (replayFile.open(QIODevice::WriteOnly)) {
            qDebug() << "[server.cpp] Saving replay files.";
            replayFile.write(gameMapData);
            replayFile.write(recorder.exportRecords());
            replayFile.close();
            qDebug() << "[server.cpp] Replay files saved.";
        }

        flagGameOvered = flagGameStarted = flagServerReadied = false;
        for (auto &client: clients)
            client.isReadied = false;
        cntReadied = 0;
        // Delete serMap
        // clear recorder

        serverWindow->pbCreateServer->setText("Rematch");
        serverWindow->pbCreateServer->setEnabled(true);

        serverWindowFrame->raise();
        serverWindowFrame->activateWindow();
        return;
        // qApp->quit();
    }

    flagGameOvered = serMap->gameOver();
}

bool Server::checkNickname(const QString &newNickname) {
    for (const auto &nickname: nicknames)
        if (newNickname.contains(nickname) || nickname.contains(newNickname))
            return false;
    nicknames.append(newNickname);
    return true;
}

void Server::updateStatus() {
    if (!flagGameStarted) {
        QJsonArray data;
        QVector<QJsonArray> teamsInfo(maxPlayerNum);
        QString msg = QString("Waiting for players (%1/%2).").arg(QString::number(cntReadied),
                                                                  QString::number(cntPlayer));
        data.append(msg);

        for (const auto &client: clients)
            teamsInfo[client.idTeam - 1].append(client.nickname);
        for (const auto &teamInfo: teamsInfo)
            data.append(teamInfo);

        emit sendMessage(generateMessage("Status", data));
        serverWindow->showMessage(msg);
    }
}

int Server::getEmptyTeam() {
    for (int i = 1; i <= maxPlayerNum; i++)
        if (!teamMbrCnt[i])
            return i;
    qDebug() << "[server.cpp] No valid team left, error occurred.";
    return 0;
}
