#include "endWindow.h"
#include "ui_endWindow.h"

EndWindow::EndWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::EndWindow) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
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

    connect(ui->pbWatch, &QPushButton::clicked, this, &EndWindow::onWatchButtonClicked);
    connect(ui->pbExit, &QPushButton::clicked, qApp, &QApplication::quit);
}

EndWindow::~EndWindow() {
    delete ui;
}

void EndWindow::updateButtonText(const QString &strWatch, const QString &strExit) {
    if (!strWatch.isEmpty()) ui->pbWatch->setText(strWatch);
    if (!strExit.isEmpty()) ui->pbExit->setText(strExit);
}

void EndWindow::updateText(const QString &strTitle, const QString &strContent) {
    ui->lbTitle->setText(strTitle);
    ui->lbContent->setText(strContent);
}

void EndWindow::gameEnded() {
    ui->pbWatch->setEnabled(false);
}

void EndWindow::onWatchButtonClicked() {
    hide();
    emit watch();
}
