#include "endWindow.h"

extern QString strFontBold;

EndWindow::EndWindow(QWidget *parent, bool flag)
        : QWidget{parent} {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    auto wndWidth = parent->width();
    auto wndHeight = parent->height();

    auto wgtHeight = wndHeight / 20;
    auto wgtWidth = wndHeight / 5;
    auto fontSize = wgtHeight / 2;
    auto wgtSep = fontSize;

    auto subWndWidth = wgtWidth * 2;
    auto subWndHeight = (wgtHeight + wgtSep) * 6;

    auto midX = wndWidth / 2, midY = wndHeight / 2;
    auto subX = subWndWidth / 2, subY = subWndHeight / 2;
    auto subLeft = midX - subWndWidth / 2, subTop = midY - subWndHeight / 2;
    auto wgtLeft = subX - wgtWidth / 2, wgtTop = subY - wgtHeight - wgtSep * 3 / 2;

    setGeometry(subLeft, subTop, subWndWidth, subWndHeight);

    QFont font(strFontBold, fontSize);
    setFont(font);

    QFile qssFile(":/qss/WindowWidgets.qss");
    if (qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(qssFile.readAll());
        qssFile.close();
    }

    QPalette wndPalette(palette());
#if (QT_VERSION_MAJOR < 6)
    wndPalette.setColor(QPalette::Background, QColor(221, 221, 221));
#else
    wndPalette.setColor(QPalette::Window, QColor(221, 221, 221));
#endif
    wndPalette.setColor(QPalette::WindowText, QColor(34, 128, 128));
    setAutoFillBackground(true);
    setPalette(wndPalette);

    lbGeneral = new QLabel(this);
    lbInfo = new QLabel(this);
    btnWatch = new QPushButton(this);
    btnExit = new QPushButton(this);

    lbInfo->setText(flag ? "You Won!" : "You Lost.");
    btnWatch->setText("Watch");
    btnExit->setText("Exit");

    lbInfo->setFont(font);
    btnWatch->setFont(font);
    btnExit->setFont(font);
    lbInfo->setAlignment(Qt::AlignCenter);

#define geo_expr wgtLeft, (wgtTop = wgtTop + wgtHeight + wgtSep), wgtWidth, wgtHeight

    lbGeneral->setStyleSheet(QString("border-image: url(:/img/General-%1.png);").arg(flag ? "Won" : "Lost"));

    lbGeneral->setGeometry(wgtLeft, wgtSep, wgtWidth, wgtWidth * 33 / 42);
    lbInfo->setGeometry(geo_expr);
    btnWatch->setGeometry(geo_expr);
    btnExit->setGeometry(geo_expr);

    lbGeneral->show();
    lbInfo->show();
    btnWatch->show();
    btnExit->show();
}
