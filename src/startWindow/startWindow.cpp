#include "startWindow.h"
#include "ui_startWindow.h"

extern QString strFontBold;

StartWindow::StartWindow(QWidget *parent)
        : QWidget(parent), ui(new Ui::StartWindow) {
    QFile cssFile(":/qss/WindowWidgets.qss");
    if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(cssFile.readAll());
        cssFile.close();
    }

    ui->setupUi(this);

    socket = new QWebSocket;

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
    ui->pbConnect->setText("Disconnect");
    ui->pbConnect->setEnabled(true);

    ui->pbReady->setEnabled(true);
    ui->leServerAddress->setEnabled(false);
    ui->leNickName->setEnabled(false);

    auto nickName = ui->leNickName->text();
    if (nickName.isEmpty()) {
        nickName = QString("%1 %2").arg(socket->peerAddress().toString(), QString::number(socket->peerPort()));
        qDebug() << QString("[startWindow.cpp] No nick name set, defaulting to %1.").arg(nickName);
    }

    if (gameWindow == nullptr) {
        qDebug() << "[startWindow.cpp] Creating game window.";
        gameWindow = new GameWindow(socket, nickName, nullptr);
    } else
        socket->sendBinaryMessage(generateMessage("Connected", {nickName}));
}

void StartWindow::onDisconnected() {
    qDebug() << "[startWindow.cpp] Disconnected from server.";
    ui->pbConnect->setText("Connect");
    ui->pbConnect->setEnabled(true);
    ui->lbMessage->setText("Disconnected");
    ui->pbReady->setEnabled(false);
    socket->close();
}

void StartWindow::onConnectClicked() {
    static std::regex reg(R"((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.)"
                          R"((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5]))");
    if (ui->pbConnect->text() == "Connect") {
        ui->pbConnect->setEnabled(false);
        auto strAddr = ui->leServerAddress->text();
        if (std::regex_match(strAddr.toStdString(), reg) || strAddr == "localhost") {
            ui->pbConnect->setText("Cancel");
            ui->lbMessage->setText(QString("Connecting to: %1").arg(strAddr));
            socket->open(QUrl(QString("ws://%1:32767").arg(strAddr)));
        } else
            ui->lbMessage->setText("Error: Wrong server address format");
    } else {
        socket->close();
    }
}

void StartWindow::onReadyClicked() {
    // TODO: Add team info and transfer it to server
    socket->sendBinaryMessage(generateMessage("Readied", {0}));
    ui->pbReady->setEnabled(false);
    ui->pbConnect->setFocus();
}

void StartWindow::onMessageReceived(const QByteArray &msg) {
    static bool inited = false, hid = false;
    auto json = loadJson(msg);
    auto msgType = json.first.toString();
    auto msgData = json.second.toArray();

    if (msgType == "Status") {
        ui->lbMessage->setText(msgData.at(0).toString());
    } else if (!inited && msgType == "InitMap") {
        inited = true;
    } else if (inited && !hid && msgType == "UpdateMap") {
        hid = true;
        wTarget->hide();
    }
}

void StartWindow::setTarget(QWidget *widget) {
    wTarget = widget;
}
