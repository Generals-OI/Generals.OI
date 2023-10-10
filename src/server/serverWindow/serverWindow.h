#ifndef SERVER_WINDOW_H
#define SERVER_WINDOW_H

#include "gameInformation.h"

#include <QWidget>
#include <QPushButton>
#include <QNetworkInterface>

namespace Ui {
    class ServerWindow;
}

class ServerWindow : public QWidget {
Q_OBJECT

public:
    explicit ServerWindow(QWidget * = nullptr);

    ~ServerWindow() override = default;

    void setTarget(QWidget *);

    void showMessage(const QString &msg);

signals:
    void createServer(int gameMode, double gameSpeed);

protected:
    void onCreateButtonClicked();

public:
    Ui::ServerWindow *ui;
    QWidget *wTarget{};
    QPushButton *pbCreateServer;
};

#endif // SERVER_WINDOW_H
