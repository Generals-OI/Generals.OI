#include "gameButton.h"

GameButton::GameButton(int x, int y, QWidget *parent) : QPushButton(parent) {
//    wTarget = parent;
    btnPos = Point{x, y};
    connect(this, &QPushButton::clicked, this, &GameButton::onClicked);
}

void GameButton::onClicked() {
    emit focused(btnPos.x, btnPos.y);
    qDebug() << QString("[gameButton.cpp] Button (%1, %2) clicked.")
            .arg(QString::number(btnPos.x), QString::number(btnPos.y));
}
/*
void GameButton::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        mPressed = true;
    if (wTarget) {
#if (QT_VERSION_MAJOR < 6)
        pStart = event->globalPos() - wTarget->pos();
#else
        pStart = event->globalPosition().toPoint() - wTarget->pos();
#endif
    }
}

void GameButton::mouseReleaseEvent(QMouseEvent *) {
    mPressed = false;
}

void GameButton::mouseMoveEvent(QMouseEvent *event) {
    if (mPressed && wTarget) {
#if (QT_VERSION_MAJOR < 6)
        QPoint pCur = event->globalPos();
#else
        QPoint pCur = event->globalPosition().toPoint();
#endif
        wTarget->move(pCur - pStart);
    }
}
*/