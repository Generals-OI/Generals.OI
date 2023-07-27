#include "serverSettingsWindow.h"
#include "ui_serverSettingsWindow.h"

extern QString strFontRegular;

ServerSettingsWindow::ServerSettingsWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::ServerSettingsWindow) {
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

    ui->titleBar->lbTitle->setText("Generals.OI - Create Server");
    ui->titleBar->setParent(this);

    connect(ServerSettingsWindow::ui->btnCreateServer, &QPushButton::clicked,
            this, &ServerSettingsWindow::onCreateButtonClicked);
}

void ServerSettingsWindow::onCreateButtonClicked() {
    int gameMode =  ui->bgGameMode->checkedId() ;
    int gameSpeed = ui->bgGameSpeed->checkedId();
    int teamEnabled = ui->bgTeaming->checkedId();
    hide();
    new Server(gameMode, gameSpeed);
}

ServerSettingsWindow::~ServerSettingsWindow() {
    delete ui;
}
