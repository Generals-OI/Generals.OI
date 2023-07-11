#ifndef START_WINDOW_H
#define START_WINDOW_H

#include "gameWindow.h"
#include "titleBar.h"

#include <regex>

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);
    ~StartWindow() override;

private:
    void onConnected();
    void onDisconnected();
    void onConnectClicked();
    void onReadyClicked();
    void onMessageReceived(const QString &);

private:
    QWebSocket *socket{};
    GameWindow *gameWindow{};

    TitleBar *titleBar{};
    QLineEdit *leServerAddress{}, *leNickName{};
    QPushButton *btnConnect{}, *btnReady{};
    QLabel *lbBackground{}, *lbMessage{};
};

#endif // START_WINDOW_H
