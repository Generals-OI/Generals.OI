#ifndef SERVER_H
#define SERVER_H

#include "serverMap.h"
#include "mapGenerator.h"
#include "processJson.h"
#include "recorder.h"
#include "serverWindow.h"
#include "windowFrame.h"

#include <QWebSocket>
#include <QWebSocketServer>
#include <QWidget>
#include <QApplication>
#include <QTimer>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QFile>

class Server : public QWidget, public ProcessJson {
Q_OBJECT

public:
    explicit Server();

    ~Server() override;

signals:

    void sendMessage(const QByteArray &msg);

public slots:

    void broadcastMessage();

private:
    void onCreateServer(int mode, double speed);

    void onNewConnection();

    void clearClient();

    bool checkNickname(const QString &newNickname);

    void updateStatus();

    int getEmptyTeam();

private:
    ServerWindow *serverWindow;
    WindowFrame *serverWindowFrame;

    QWebSocketServer *server;
    QHostAddress address;
    ServerMap *serMap{};
    QTimer *gameTimer{};

    int gameMode{};
    double gameSpeed{};
    bool flagServerReadied{}, flagPlayed{};

    QByteArray baPlayersInfo;

    int cntPlayer{}, cntReadied{};
    bool flagGameStarted{}, flagGameOvered{};

    QMap<QWebSocket *, PlayerInfo> clients;
    QVector<QWebSocket *> clientsIndex;
    QList<QString> nicknames;
    QVector<int> teamMbrCnt;

    QByteArray gameMapData;
    Recorder recorder;
};

#endif // SERVER_H
