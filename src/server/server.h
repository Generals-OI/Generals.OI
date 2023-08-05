#ifndef SERVER_H
#define SERVER_H

#include "serverMap.h"
#include "playerInfo.h"
#include "mapGenerator.h"

#include <QWebSocket>
#include <QWebSocketServer>
#include <QWidget>
#include <QApplication>
#include <QTimer>

#include "processJson.h"

class Server : public QWidget, public ProcessJson {
Q_OBJECT

public:
    explicit Server(int, double);

    ~Server() override;

signals:

    void sendMessage(const QByteArray &msg);

public slots:

    void broadcastMessage();

private:
    void onNewConnection();

    void clearClient();

private:
    QWebSocketServer *server;
    QHostAddress address;
    ServerMap *serMap{};
    QTimer *gameTimer{};

    int gameMode{};
    double gameSpeed{};

    QByteArray baPlayersInfo;

    int cntPlayer{}, cntReadied{};
    bool flagGameStarted{}, flagGameOvered{};

//    std::map<QWebSocket *, PlayerInfo> clients;
    QMap<QWebSocket *, PlayerInfo> clients;
    std::vector<int> teamInfo;
};

#endif // SERVER_H
