#include "gameButton.h"

GameButton::GameButton(int x, int y, QWidget *parent, QWidget *target) : QPushButton(parent) {
    wParent = parent;
    wTarget = target;
    btnPos = Point{x, y};
    connect(this, &QPushButton::clicked, this, &GameButton::onClicked);
}

void GameButton::onClicked() {
    emit focused(btnPos.x, btnPos.y);
    qDebug() << QString("[gameButton.cpp] Button (%1, %2) clicked.")
            .arg(QString::number(btnPos.x), QString::number(btnPos.y));
}

void GameButton::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        mPressed = true;
    if (wParent && wTarget) {
#if (QT_VERSION_MAJOR < 6)
        pStart = event->globalPos() - wParent->pos();
#else
        pStart = event->globalPosition().toPoint() - wParent->pos();
#endif
    }
    return QPushButton::mousePressEvent(event);
}

void GameButton::mouseReleaseEvent(QMouseEvent *event) {
    mPressed = false;
    return QPushButton::mouseReleaseEvent(event);
}

void GameButton::mouseMoveEvent(QMouseEvent *event) {
    if (mPressed && wParent && wTarget) {
#if (QT_VERSION_MAJOR < 6)
        QPoint pCur = event->globalPos();
#else
        QPoint pCur = event->globalPosition().toPoint();
#endif
        wParent->move(pCur - pStart);
        wTarget->move(pCur - pStart);
    }
}
