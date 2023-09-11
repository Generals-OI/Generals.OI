#ifndef GAME_MAP_GRID_H
#define GAME_MAP_GRID_H

#include <QWidget>
#include <QObject>
#include <QGridLayout>
#include <QLabel>

#include "gameButton.h"

const QString strCell[] = {"Land", "General", "City", "Mountain", "Swamp"};
const QString strArrow[] = {"Up", "Down", "Left", "Right"};
const int direction4[4][2] = {{-1, 0},
                              {0,  -1},
                              {1,  0},
                              {0,  1}};

class GameMapGrid : public QWidget {
Q_OBJECT

public:
    GameMapGrid(int width, int height, QWidget *parent);

    ~GameMapGrid() override = default;

    int width{}, height{};
    QGridLayout *mapLayout{}, *buttonLayout{};
    QVector<QVector<GameButton *>> btnFocus;
    QWidget *wButton{}, *wFocus{};
    QLabel *lbMapBgd{}, *lbFocus{}, *lbShadow[4]{};
    QVector<QVector<QLabel *>> lbObstacle, lbColor, lbMain, lbArrow[4];
};

#endif // GAME_MAP_GRID_H
