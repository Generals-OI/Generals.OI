#include "server.h"

Server::Server(int gameMode, double gameSpeed) :
        gameMode(gameMode), gameSpeed(gameSpeed) {
    server = new QWebSocketServer("Generals.OI Server", QWebSocketServer::NonSecureMode, this);
    address = QHostAddress::Any;

    if (server->listen(address, 32767)) {
        teamMbrCnt = QVector<int>(maxPlayerNum + 1);
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
        auto itCurrent = clients.find(socket);
        if (itCurrent != clients.end()) {
            auto &currentClientInfo = itCurrent.value();
            if (!currentClientInfo.isSpect) {
                auto &lastClientInfo = clients[clientsIndex[cntPlayer]];
                lastClientInfo.idPlayer = currentClientInfo.idPlayer;
                cntPlayer--;
                if (currentClientInfo.isReadied)
                    cntReadied--;
                teamMbrCnt[currentClientInfo.idTeam]--;
            }

            clients.remove(socket);
            for (int i = 0; i < nicknames.size(); i++)
                if (nicknames.at(i) == currentClientInfo.nickName) {
                    nicknames.removeAt(i);
                    break;
                }
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
                    socket->sendBinaryMessage(generateMessage(
                            "Status", {"Invalid nickname."}));
                    socket->close();
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
            if (clients[socket].isSpect) return;
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

                    qDebug() << "[server.cpp] Start generating.";
                    serMap = new ServerMap(MapGenerator::randomMap(cntPlayer, totTeam, teamInfo));
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

        if (!flagGameStarted && (msgType == "Connected" || msgType == "Readied" || msgType == "ChooseTeam"))
            updateStatus();
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
        data.append(QString("Waiting (%1/%2) ...").arg(QString::number(cntReadied), QString::number(cntPlayer)));

        for (const auto &client: clients)
            teamsInfo[client.idTeam - 1].append(client.nickName);
        for (const auto &teamInfo: teamsInfo)
            data.append(teamInfo);

        emit sendMessage(generateMessage("Status", data));
    }
}

int Server::getEmptyTeam() {
    for (int i = 1; i <= maxPlayerNum; i++)
        if (!teamMbrCnt[i])
            return i;
    qDebug() << "[server.cpp] No valid team left, error occurred.";
    return 0;
}
