#include "startWindow.h"
#include "ui_startWindow.h"

extern QString strFontBold;

StartWindow::StartWindow(QWidget *parent)
        : QWidget(parent), ui(new Ui::StartWindow) {

    ui->setupUi(this);

    socket = new QWebSocket;
    pbTeams = QVector<TeamButton *>(maxPlayerNum + 1);

    ui->lbTeam->hide();
    for (int i = 0; i <= maxPlayerNum; i++) {
        pbTeams[i] = new TeamButton(this, i + 1);
        pbTeams[i]->hide();
        ui->hlTeam->insertWidget(i + 1, pbTeams[i]);
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
}

void StartWindow::onCreateServer() {
    // Note that this function only works in cmake mode `Release`
    auto res = QProcess::startDetached(qApp->arguments()[0], QStringList("-s"));
    qDebug() << "[startWindow.cpp] Create Process Status:" << res;
}

void StartWindow::onConnected() {
    auto nickname = ui->leNickName->text();
    auto nicknameLen = nickname.toLocal8Bit().length();
    if (!(3 <= nicknameLen && nicknameLen <= 15)) {
        ui->lbMessage->setText("[Disconnected]\n Error: Illegal nickname length");
        return;
    }

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
}

void StartWindow::onConnectClicked() {
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
        for (int i = 0, totTeam = 0; i < maxPlayerNum; i++) {
            auto teamData = msgData.at(i + 1).toArray();
            if (teamData.isEmpty()) pbTeams[i]->hide();
            else pbTeams[i]->show();
            QString strTeam = QString("Team %1").arg(QString::number(++totTeam));
            for (auto nickname: teamData)
                strTeam.append(QString("\n%1").arg(nickname.toString()));
            pbTeams[i]->setText(strTeam);
        }
        pbTeams[maxPlayerNum]->show();
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

    auto screen = qApp->primaryScreen()->geometry();
    auto window = QSize(800, 750);
    wTarget->setGeometry((screen.width() - window.width()) / 2, (screen.height() - window.height()) / 2,
                         window.width(), window.height());
}

void StartWindow::onTeamButtonChosen(int idButton) {
    socket->sendBinaryMessage(generateMessage("ChooseTeam", {idButton}));
}
