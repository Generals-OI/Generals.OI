#ifndef GAME_MAP_GRID_H
#define GAME_MAP_GRID_H

#include <QWidget>
#include <QObject>
#include <QGridLayout>
#include <QLabel>

#include "gameButton.h"

class GameMapGrid : public QWidget {
Q_OBJECT

public:
    GameMapGrid(int width, int height, QWidget *parent);

    ~GameMapGrid() override = default;

    int width{}, height{};
    QGridLayout *mapLayout{}, *buttonLayout{};
    QVector<QVector<GameButton *>> btnFocus;
    QWidget *wgtButton{};
    QLabel *lbMapBgd{};
    QVector<QVector<QLabel *>> lbObstacle, lbColor, lbMain, lbArrow[4];
};

#endif // GAME_MAP_GRID_H
