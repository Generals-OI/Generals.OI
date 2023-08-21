#include "surrenderWindow.h"
#include "ui_surrenderWindow.h"


SurrenderWindow::SurrenderWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::SurrenderWindow) {
    ui->setupUi(this);
    connect(ui->pbNo, &QPushButton::clicked, this, &QWidget::hide);
    connect(ui->pbYes, &QPushButton::clicked, this, &SurrenderWindow::surrender);
    hide();
}

void SurrenderWindow::surrender() {
    emit surrendered();
    hide();
}

SurrenderWindow::~SurrenderWindow() {
    delete ui;
}
