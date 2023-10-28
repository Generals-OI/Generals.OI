#ifndef SERVER_TRAY_H
#define SERVER_TRAY_H

#include <QWidget>
#include <QMenu>
#include <QWidgetAction>
#include <QSystemTrayIcon>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ServerTray : public QSystemTrayIcon {
Q_OBJECT

public:
    explicit ServerTray(QWidget *parent);

    ~ServerTray() override = default;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QWidget *target;
    QMenu *menu;

    QWidget *wTop;
    QWidgetAction *waTop;
    QLabel *lTop;
    QPushButton *pbLink;

    QWidget *wBottom;
    QWidgetAction *waBottom;
    QPushButton *pbShow;
    QPushButton *pbQuit;
    QPushButton *pbPause;

//    QAction *aShow;
//    QAction *aPause;
//    QAction *aQuit;
};


#endif // SERVER_TRAY_H
