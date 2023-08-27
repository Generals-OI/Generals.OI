#include "startWindow.h"
#include "ui_startWindow.h"

extern QString strFontRegular;

StartWindow::StartWindow(QWidget *parent)
        : QWidget(parent), ui(new Ui::StartWindow) {

    ui->setupUi(this);

    socket = new QWebSocket;
    pbTeams = QVector<TeamButton *>(maxPlayerNum + 1);
    QSizePolicy spButtons(QSizePolicy::Preferred, QSizePolicy::Expanding);
    QFont fButtons(strFontRegular, 14);
    fmTeamBtn = new QFontMetrics(fButtons);

    for (auto &hlTeamButton: hlTeamButtons) {
        hlTeamButton = new QHBoxLayout();
        hlTeamButton->setSpacing(10);
        hlTeamButton->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        hlTeamButton->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        ui->vlTeam->addLayout(hlTeamButton);
    }

    ui->lbTeam->hide();
    for (int i = 0; i <= maxPlayerNum; i++) {
        pbTeams[i] = new TeamButton(this, i + 1);
        pbTeams[i]->setFont(fButtons);
        pbTeams[i]->setSizePolicy(spButtons);
        pbTeams[i]->hide();
        hlTeamButtons[i / btnPerLine]->insertWidget(i % btnPerLine + 1, pbTeams[i]);
        connect(pbTeams[i], &TeamButton::chosen, this, &StartWindow::onTeamButtonChosen);
    }
    pbTeams[maxPlayerNum]->setText("Create and Join\nNew Team");

    setTabOrder(ui->leNickName, ui->leServerAddress);
    setTabOrder(ui->leServerAddress, ui->pbConnect);
    setTabOrder(ui->pbConnect, ui->pbReady);
    setTabOrder(ui->pbReady, ui->pbCreateServer);

    connect(ui->pbConnect, &QPushButton::clicked, this, &StartWindow::onConnectClicked);
    connect(ui->pbReady, &QPushButton::clicked, this, &StartWindow::onReadyClicked);
    connect(ui->pbCreateServer, &QPushButton::clicked, this, &StartWindow::onCreateServer);

    connect(socket, &QWebSocket::connected, this, &StartWindow::onConnected);
    connect(socket, &QWebSocket::disconnected, this, &StartWindow::onDisconnected);
    connect(socket, &QWebSocket::binaryMessageReceived, this, &StartWindow::onMessageReceived);
}

StartWindow::~StartWindow() {
    delete socket;
    delete fmTeamBtn;
}

void StartWindow::onCreateServer() {
    // Note that this function only works in cmake mode `Release`
    auto res = QProcess::startDetached(qApp->arguments()[0], QStringList("-s"));
    qDebug() << "[startWindow.cpp] Create Process Status:" << res;
}

void StartWindow::onConnected() {
    auto nickname = ui->leNickName->text();
    if (gameWindow == nullptr) {
        qDebug() << "[startWindow.cpp] Creating game window.";
        gameWindow = new GameWindow(socket, nickname, nullptr);
    } else
        socket->sendBinaryMessage(generateMessage("Connected", {nickname}));

    ui->pbConnect->setText("Disconnect");
    socketStatus = WebSocketStatus::Connected;

    ui->pbConnect->setEnabled(true);
    ui->pbReady->setEnabled(true);
    ui->leServerAddress->setEnabled(false);
    ui->leNickName->setEnabled(false);

    ui->lbTeam->show();
}

void StartWindow::onDisconnected() {
    qDebug() << "[startWindow.cpp] Disconnected from server.";
    ui->pbConnect->setText("Connect");
    ui->lbMessage->setText("[Disconnected]\n" + ui->lbMessage->text().section("\n", 1));

    ui->pbConnect->setEnabled(true);
    ui->pbReady->setEnabled(false);
    ui->leNickName->setEnabled(true);
    ui->leServerAddress->setEnabled(true);

    socket->close();
    socketStatus = WebSocketStatus::Disconnected;

    ui->lbTeam->hide();
    for (int i = 0; i <= maxPlayerNum; i++)
        pbTeams[i]->hide();
    mediateWindow(true);
}

void StartWindow::onConnectClicked() {
    auto nicknameLen = ui->leNickName->text().toLocal8Bit().length();
    qDebug() << "[startWindow.cpp] Current nickname length:" << nicknameLen;
    if (!(3 <= nicknameLen && nicknameLen <= 15)) {
        ui->lbMessage->setText("[Disconnected]\n Error: Illegal nickname length");
        return;
    }

    static std::regex reg(R"((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.)"
                          R"((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5]))");
//    static QRegularExpression regex(R"((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.)"
//                                    R"((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5]))");
    if (socketStatus == WebSocketStatus::Disconnected) {
        auto strAddr = ui->leServerAddress->text();
        // TODO: Fix the bug of QRegularExpression
        if (/*regex.match(strAddr).hasMatch()*/
                std::regex_match(strAddr.toStdString(), reg) || strAddr.toLower() == "localhost") {
            ui->pbConnect->setText("Cancel");
            ui->lbMessage->setText(QString("[Connecting]\nLocal Server: %1").arg(strAddr));
            socket->open(QUrl(QString("ws://%1:32767").arg(strAddr)));
            socketStatus = WebSocketStatus::Connecting;
        } else {
            ui->lbMessage->setText("Error: Wrong server address format");
        }
    } else {
        socket->close();
        socketStatus = WebSocketStatus::Disconnected;
    }
}

void StartWindow::onReadyClicked() {
    socket->sendBinaryMessage(generateMessage("Readied", QJsonArray()));
    ui->pbReady->setEnabled(false);
    ui->pbConnect->setFocus();
}

void StartWindow::onMessageReceived(const QByteArray &msg) {
    auto json = loadJson(msg);
    auto msgType = json.first.toString();
    auto msgData = json.second.toArray();

    if (msgType == "Status") {
        ui->lbMessage->setText("[Connected]\n" + msgData.at(0).toString());
        for (auto &cnt: cntTeam) cnt = 0;
        for (auto &sum: sumBtnLen) sum = 0;

        for (int i = 0, currentTeam = 0; i < maxPlayerNum; i++) {
            auto teamData = msgData.at(i + 1).toArray();
            if (teamData.isEmpty()) {
                pbTeams[i]->hide();
                pbTeams[i]->setText(QString());
                pbTeams[i]->setMinimumWidth(0);
                continue;
            }
            pbTeams[i]->show();
            cntTeam[i / btnPerLine]++;

            QString strTeam = QString("Team %1").arg(QString::number(++currentTeam));
            int maxWidth = fmTeamBtn->horizontalAdvance(strTeam);
            for (auto value: teamData) {
                auto nickname = value.toString();
                maxWidth = std::max(maxWidth, fmTeamBtn->horizontalAdvance(nickname));
                strTeam.append(QString("\n%1").arg(nickname));
            }
            pbTeams[i]->setText(strTeam);
            pbTeams[i]->setMinimumWidth(maxWidth);
            sumBtnLen[i / btnPerLine] += maxWidth;
        }
        pbTeams[maxPlayerNum]->show();
        mediateWindow();
    } else if (!gotInitMsg && msgType == "InitGame") {
        gotInitMsg = true;
    } else if (gotInitMsg && !wndHidden && msgType == "UpdateMap") {
        wndHidden = true;
        wTarget->hide();
        disconnect(socket, &QWebSocket::binaryMessageReceived, this, &StartWindow::onMessageReceived);
    }
}

void StartWindow::setTarget(QWidget *widget) {
    wTarget = widget;
    mediateWindow(true);
}

void StartWindow::onTeamButtonChosen(int idButton) {
    socket->sendBinaryMessage(generateMessage("ChooseTeam", {idButton}));
}

void StartWindow::mediateWindow(bool useDefault) {
    QSize defaultSize(800, 750);
    if (useDefault) {
        wTarget->setMinimumSize(defaultSize);
        wTarget->resize(defaultSize);
    } else {
        int width = std::max(sumBtnLen[0], sumBtnLen[1]) + std::max(cntTeam[0], cntTeam[1]) * 30;
        QSize minimumSize(std::max(defaultSize.width(), width), 900);
        wTarget->setMinimumSize(minimumSize);
        wTarget->resize(minimumSize);
    }
    wTarget->move(qApp->primaryScreen()->geometry().center() - wTarget->rect().center());
}
