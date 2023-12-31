#ifndef START_WINDOW_H
#define START_WINDOW_H

#include <QProcess>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "gameWindow.h"
#include "teamButton.h"
#include "processJson.h"

#include <regex>

namespace Ui {
    class StartWindow;
}

class StartWindow : public QWidget, public ProcessJson {
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

    void onMessageReceived(const QByteArray &);

    void onTeamButtonChosen(int idButton);

    void mediateWindow(bool useDefault = false);

    enum WebSocketStatus {
        Disconnected, Connecting, Connected
    };

    Ui::StartWindow *ui;
    QWidget *wTarget{};
    QFontMetrics *fmTeamBtn;

    static const int btnLinesNum = 2, btnPerLine = 9;
    QHBoxLayout *hlTeamButtons[btnLinesNum]{};
    QVector<TeamButton*> pbTeams;
    int cntTeam[btnLinesNum]{}, sumBtnLen[btnLinesNum]{};

    QWebSocket *socket{};
    WebSocketStatus socketStatus = WebSocketStatus::Disconnected;

    GameWindow *gameWindow{};

    bool gotInitMsg{}, wndHidden{};
};

#endif // START_WINDOW_H
