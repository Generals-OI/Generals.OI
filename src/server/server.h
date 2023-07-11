#ifndef SERVER_H
#define SERVER_H

#include "serverMap.h"
#include "playerInfo.h"

#include <QWebSocket>
#include <QWebSocketServer>
#include <QWidget>
#include <QTimer>

class Server : public QWidget {
Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);

    ~Server() override;

signals:

    void sendMessage(const QString &msg);

public slots:

    void broadcastMessage();

private:
    void onNewConnection();

    void clearClient();

private:
    QWebSocketServer *server;
    QHostAddress address;
    ServerMap *serMap{};

    int cntPlayer{}, cntReadied{};
    bool flagGameStarted{};

    std::map<QWebSocket *, PlayerInfo> clients;
};

#endif // SERVER_H
