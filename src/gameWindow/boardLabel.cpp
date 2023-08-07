#include "gameWindow.h"

void GameWindow::BoardLabel::init(QWidget *parent, QFont &font, QGridLayout *layout, int row) {
    lbName = new QLabel(parent);
    lbName->setFont(font);
    lbName->show();
    lbName->setObjectName("Rank");
    layout->addWidget(lbName, row, 0);
    lbArmy = new QLabel(parent);
    lbArmy->setFont(font);
    lbArmy->setObjectName("Rank");
    lbArmy->show();
    layout->addWidget(lbArmy, row, 1);
    lbLand = new QLabel(parent);
    lbLand->setFont(font);
    lbLand->setObjectName("Rank");
    lbLand->show();
    layout->addWidget(lbLand, row, 2);
}

void GameWindow::BoardLabel::updateContent(const QString &strName, const QString &strArmy,
                                           const QString &strLand) const {
    lbName->setText(strName);
    lbArmy->setText(strArmy);
    lbLand->setText(strLand);
}
