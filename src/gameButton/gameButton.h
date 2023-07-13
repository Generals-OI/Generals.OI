#ifndef GAME_BUTTON_H
#define GAME_BUTTON_H

#include <QPushButton>
#include <QMouseEvent>

#include "point.h"

#include <QDebug>

class GameButton : public QPushButton {
Q_OBJECT

public:
    GameButton(int, int, QWidget * = nullptr);

signals:

    void focused(const int &, const int &);

protected:
//    void mousePressEvent(QMouseEvent *) override;

//    void mouseReleaseEvent(QMouseEvent *) override;

//    void mouseMoveEvent(QMouseEvent *) override;

    void onClicked();

private:
//    bool mPressed{};
//    QPoint pStart{};
//    QWidget *wTarget{};
    Point btnPos;
};

#endif // GAME_BUTTON_H
