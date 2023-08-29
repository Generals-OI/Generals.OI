#include "gameWindow.h"

void GameWindow::BoardLabel::init(QWidget *parent, QFont &font, QGridLayout *layout, int row) {
    lbName = create(parent, font);
    layout->addWidget(lbName, row, 0);
    lbArmy = create(parent, font);
    layout->addWidget(lbArmy, row, 1);
    lbLand = create(parent, font);
    layout->addWidget(lbLand, row, 2);
}

QLabel *GameWindow::BoardLabel::create(QWidget *parent, QFont &font) {
    auto target = new QLabel(parent);
    target->setFont(font);
    target->setAlignment(Qt::AlignCenter);
    target->setObjectName("Rank");
    target->show();
    return target;
}

void GameWindow::BoardLabel::updateContent(const QString &strName, const QString &strArmy,
                                           const QString &strLand) const {
    lbName->setText(strName);
    lbArmy->setText(strArmy);
    lbLand->setText(strLand);
}
