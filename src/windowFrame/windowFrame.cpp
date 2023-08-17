#include "windowFrame.h"

WindowFrame::WindowFrame(QWidget *widget, QWidget *parent)
        : QMainWindow(parent), contentWidget(widget) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    QPalette wndPalette(palette());
#if (QT_VERSION_MAJOR < 6)
    wndPalette.setColor(QPalette::Background, QColor(34, 34, 34));
#else
    wndPalette.setColor(QPalette::Window, QColor(34, 34, 34));
#endif
    setPalette(wndPalette);

    mainWidget = new QWidget(this);

    verticalLayout = new QVBoxLayout(mainWidget);

    titleBar = new TitleBar(mainWidget, 60);
    titleBar->setFixedHeight(60);
    titleBar->setTarget(this);

    verticalLayout->addWidget(titleBar);
    verticalLayout->addWidget(contentWidget);

    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    setCentralWidget(mainWidget);
}

void WindowFrame::setTitle(const QString &title) {
    titleBar->lbTitle->setText(title);
    setWindowTitle(title);
}