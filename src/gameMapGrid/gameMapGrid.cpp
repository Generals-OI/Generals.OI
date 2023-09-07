#include "gameMapGrid.h"

GameMapGrid::GameMapGrid(int width, int height, QWidget *parent) 
	: width(width), height(height), QWidget(parent) {
	mapLayout = new QGridLayout(this);
	wgtButton = new QWidget(parent);
	buttonLayout = new QGridLayout(wgtButton);
	lbMapBgd = new QLabel(this);

	btnFocus = QVector<QVector<GameButton *>>(height + 1, QVector<GameButton *>(width + 1));
    lbObstacle = lbMain = lbColor = QVector<QVector<QLabel *>>(height + 1, QVector<QLabel *>(width + 1));
    for (int i = 0; i < 4; i++) lbArrow[i] = lbMain;
}
