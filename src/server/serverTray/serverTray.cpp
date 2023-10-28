#include "serverTray.h"

ServerTray::ServerTray(QWidget *parent)
/*: QSystemTrayIcon(parent)*/ {
    setIcon(QIcon(":/img/Icon-Blue.ico"));
    setToolTip("Generals.OI Server");
    installEventFilter(this);

    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);

    target = parent;
    menu = new QMenu(parent);

    wTop = new QWidget();
    waTop = new QWidgetAction(menu);
    lTop = new QLabel("Generals.OI Server");
    lTop->setSizePolicy(sp);
    pbLink = new QPushButton(QIcon(":/img/GitHub.png"), "GitHub"); // TODO: Insert "GitHub" icon
    pbLink->setCursor(Qt::PointingHandCursor);
    pbLink->setSizePolicy(sp);

    auto *vlTop = new QVBoxLayout();
//    hlTop->setContentsMargins(5, 5, 5, 5);
//    hlTop->setSpacing(5);
    vlTop->addWidget(lTop, 5);
    vlTop->addWidget(pbLink, 5);
    wTop->setLayout(vlTop);
    wTop->installEventFilter(this);
    waTop->setDefaultWidget(wTop);

    wBottom = new QWidget();
    waBottom = new QWidgetAction(menu);
    pbShow = new QPushButton("Show Window"); // TODO: Insert "Show" icon
    pbShow->setSizePolicy(sp);
    pbPause = new QPushButton("Pause"); // TODO: Insert "Pause" icon
    pbPause->setSizePolicy(sp);
    pbQuit = new QPushButton("Quit"); // TODO: Insert "Quit" icon
    pbQuit->setSizePolicy(sp);

    auto *vlBottom = new QVBoxLayout();
//    vlBottom->setContentsMargins(5, 5, 5, 5);
//    vlBottom->setSpacing(5);
    vlBottom->addWidget(pbShow, 5);
    vlBottom->addWidget(pbPause, 5);
    vlBottom->addWidget(pbQuit, 5);
    wBottom->setLayout(vlBottom);
    wBottom->installEventFilter(this);
    waBottom->setDefaultWidget(wBottom);

    menu->addAction(waTop);
    menu->addSeparator();
    menu->addAction(waBottom);
    setContextMenu(menu);
    show();
}

bool ServerTray::eventFilter(QObject *obj, QEvent *event) {
    if (/*obj == wTop &&*/ event->type() == QEvent::Paint) {
        QPainter painter((QWidget *) obj);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(36, 36, 36));
        painter.drawRect(((QWidget *) obj)->rect());
    }
    return QSystemTrayIcon::eventFilter(obj, event);
}
