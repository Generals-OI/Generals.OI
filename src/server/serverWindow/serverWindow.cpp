#include "serverWindow.h"
#include "ui_serverWindow.h"

extern QString strFontRegular;

ServerWindow::ServerWindow(QWidget *parent)
        : QWidget(parent), ui(new Ui::ServerWindow) {
    ui->setupUi(this);
    pbCreateServer = ui->btnCreateServer;

    QString strAddr;
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for (const auto &addr: addresses)
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && !addr.toString().startsWith("127.0")) {
            if (!strAddr.isEmpty()) strAddr.append("; ");
            strAddr.append(addr.toString());
        }
    ui->lbAddressContent->setText(strAddr);

    connect(pbCreateServer, &QPushButton::clicked, this, &ServerWindow::onCreateButtonClicked);
}

void ServerWindow::setTarget(QWidget *target) {
    wTarget = target;
}

void ServerWindow::showMessage(const QString &msg) {
    ui->lbMessage->setText(msg);
}

void ServerWindow::onCreateButtonClicked() {
    // wTarget->hide();

    const int viewTypeOptions[] = {0, 0, GameMode::nearsighted, GameMode::mistyVeil, GameMode::crystalClear};
    const double speedOptions[] = {0, 0, 1, 1.5, 2, 3, 5, 10};
    const int teamOptions[] = {0, 0, GameMode::allowTeaming, 0};

    int viewType = viewTypeOptions[-ui->bgViewType->checkedId()];
    int modifiers = (GameMode::silentWar * ui->cbSilentWar->isChecked()) |
                    (GameMode::leapfrog * ui->cbLeapfrog->isChecked()) |
                    (GameMode::cityState * ui->cbCityState->isChecked());
    double gameSpeed = speedOptions[-ui->bgGameSpeed->checkedId()];
    int teamingEnabled = teamOptions[-ui->bgTeaming->checkedId()];

    emit createServer(viewType | modifiers | teamingEnabled, gameSpeed);
    pbCreateServer->setEnabled(false);
}
