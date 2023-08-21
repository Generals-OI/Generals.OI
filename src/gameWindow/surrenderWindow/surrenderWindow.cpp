#include "surrenderWindow.h"
#include "ui_surrenderWindow.h"


SurrenderWindow::SurrenderWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::SurrenderWindow) {
    ui->setupUi(this);

    QFile qssFile(":/qss/WindowWidgets.qss");
    if (qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(qssFile.readAll());
        qssFile.close();
    }

    QPalette wndPalette(palette());
#if (QT_VERSION_MAJOR < 6)
    wndPalette.setColor(QPalette::Background, QColor(36, 36, 36));
#else
    wndPalette.setColor(QPalette::Window, QColor(36, 36, 36));
#endif
    setAutoFillBackground(true);
    setPalette(wndPalette);

    move(parent->geometry().center() - rect().center());

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
