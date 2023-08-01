#include "serverSettingsWindow.h"
#include "ui_serverSettingsWindow.h"

extern QString strFontRegular;

ServerSettingsWindow::ServerSettingsWindow(QWidget *parent)
        : QWidget(parent), ui(new Ui::ServerSettingsWindow) {
    ui->setupUi(this);

    connect(ServerSettingsWindow::ui->btnCreateServer, &QPushButton::clicked,
            this, &ServerSettingsWindow::onCreateButtonClicked);
}

void ServerSettingsWindow::setTarget(QWidget *target) {
    wTarget = target;
}

void ServerSettingsWindow::onCreateButtonClicked() {
    wTarget->hide();

    int gameMode = -ui->bgGameMode->checkedId();
    int gameSpeedOption = -ui->bgGameSpeed->checkedId();
    int teamEnabled = -ui->bgTeaming->checkedId();

    const double speedOptions[] = {0, 0, 1, 1.5, 2, 3, 5};
    auto gameSpeed = speedOptions[gameSpeedOption];

    new Server(gameMode, gameSpeed);
}
