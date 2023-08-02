#ifndef START_WINDOW_H
#define START_WINDOW_H

#include <QProcess>
#include <regex>

#include "gameWindow.h"
#include "titleBar.h"
#include "teamButton.h"

namespace Ui {
    class StartWindow;
}

class StartWindow : public QWidget {
Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);

    ~StartWindow() override;

    void setTarget(QWidget *);

private:
    static void onCreateServer();

    void onConnected();

    void onDisconnected();

    void onConnectClicked();

    void onReadyClicked();

    void onMessageReceived(const QString &);

private:
    Ui::StartWindow *ui;
    QWidget *wTarget{};
    QWebSocket *socket{};
    GameWindow *gameWindow{};
    QVector<TeamButton *> pbTeams;
};

#endif // START_WINDOW_H
