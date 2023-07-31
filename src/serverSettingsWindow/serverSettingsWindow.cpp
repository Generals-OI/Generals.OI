#include "serverSettingsWindow.h"
#include "ui_serverSettingsWindow.h"

extern QString strFontRegular;

ServerSettingsWindow::ServerSettingsWindow(QWidget *parent)
        : QWidget(parent), ui(new Ui::ServerSettingsWindow) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    QPalette wndPalette(palette());
#if (QT_VERSION_MAJOR < 6)
    wndPalette.setColor(QPalette::Background, QColor(34, 34, 34));
#else
    wndPalette.setColor(QPalette::Window, QColor(34, 34, 34));
#endif
    wndPalette.setColor(QPalette::WindowText, QColor(0, 128, 128));
    setPalette(wndPalette);

    ui->setupUi(this);

    connect(ServerSettingsWindow::ui->btnCreateServer, &QPushButton::clicked,
            this, &ServerSettingsWindow::onCreateButtonClicked);
}

void ServerSettingsWindow::onCreateButtonClicked() {
    hide();

    int gameMode = -ui->bgGameMode->checkedId();
    int gameSpeedOption = -ui->bgGameSpeed->checkedId();
    int teamEnabled = -ui->bgTeaming->checkedId();

    const double speedOptions[] = {0, 0, 1, 1.5, 2, 3, 5};
    auto gameSpeed = speedOptions[gameSpeedOption];

    new Server(gameMode, gameSpeed);
}
