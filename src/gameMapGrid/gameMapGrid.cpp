#include "gameMapGrid.h"

GameMapGrid::GameMapGrid(int width, int height, QWidget *parent)
        : width(width), height(height), QWidget(parent) {
    mapLayout = new QGridLayout(this);
    wButton = new QWidget(this);
    buttonLayout = new QGridLayout(wButton);

    btnFocus = QVector<QVector<GameButton *>>(height + 1, QVector<GameButton *>(width + 1));
    lbObstacle = lbMain = lbColor = QVector<QVector<QLabel *>>(height + 1, QVector<QLabel *>(width + 1));
    for (auto &i: lbArrow) i = lbMain;

    QSizePolicy spMap(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // TODO: Change Spacing if necessary
    mapLayout->setSpacing(2);
    mapLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    lbMapBgd = new QLabel(this);
    lbMapBgd->setObjectName("Background");
    lbMapBgd->setSizePolicy(spMap);
    lbMapBgd->show();
    mapLayout->addWidget(lbMapBgd, 1, 1, height, width);

    for (int i = 1; i <= height; i++) 
        for (int j = 1; j <= width; j++) {
            QLabel *lbO = lbObstacle[i][j] = new QLabel(this);
            QLabel *lbC = lbColor[i][j] = new QLabel(this);
            QLabel *lbM = lbMain[i][j] = new QLabel(this);
            GameButton *btnF = btnFocus[i][j] = new GameButton(i, j, wButton, this);

            lbO->setSizePolicy(spMap);
            lbC->setSizePolicy(spMap);
            lbM->setSizePolicy(spMap);
            btnF->setSizePolicy(spMap);
            lbM->setAlignment(Qt::AlignCenter);

            for (int k = 0; k < 4; k++) {
                QLabel *lbA = lbArrow[k][i][j] = new QLabel(this);
                lbA->setSizePolicy(spMap);
                lbA->setStyleSheet(QString("border-image: url(:/img/Arrow-%1.png);").arg(strArrow[k]));
                lbA->hide();
            }

            mapLayout->addWidget(lbO, i, j, 1, 1);
            mapLayout->addWidget(lbC, i, j, 1, 1);
            mapLayout->addWidget(lbM, i, j, 1, 1);
            for (auto &k: lbArrow) mapLayout->addWidget(k[i][j], i, j, 1, 1);
            buttonLayout->addWidget(btnF, i, j, 1, 1);
        }

    wFocus = new QWidget(this);
    for (auto &i: lbShadow) {
        i = new QLabel(wFocus);
        i->setObjectName("Shadow");
    }
    lbFocus = new QLabel(wFocus);
    lbFocus->setObjectName("Focus");

    mapLayout->addWidget(wButton, 1, 1, height, width);
    wButton->raise();
}
