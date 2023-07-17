#include "startWindow.h"

extern QString strFontBold;

StartWindow::StartWindow(QWidget *parent)
        : QMainWindow(parent) {
    auto screenGeometry = qApp->primaryScreen()->geometry();
    auto dpi = qApp->primaryScreen()->logicalDotsPerInch() / 96.0;
    QFont font(strFontBold);
    setFont(font);

    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int maxWidth = int(screenWidth * 0.8);
    int maxHeight = int(screenHeight * 0.8);
    int wndWidth = std::min(maxWidth, int(maxHeight / 0.75));
    int wndHeight = std::min(maxHeight, int(maxWidth * 0.75));
    int verItv = int(wndWidth / 16), horItv = int(wndHeight / 2);
    int leLeft = int(wndWidth / 4), leTop = int(wndWidth / 3);
    int leWidth = int(wndWidth / 2), leHeight = int(leWidth / 10);
    int btnLeft = int(wndHeight / 4);
    int btnWidth = int(wndWidth / 4), btnHeight = leHeight;
    int fontSize = int(wndWidth / (dpi * 40));
    int iconSize = wndHeight / 32;

    setWindowTitle("Generals.OI");
    setWindowIcon(QIcon(":/icon.png"));
    setDockNestingEnabled(false);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    setGeometry((screenWidth - wndWidth) / 2, (screenHeight - wndHeight) / 2, wndWidth, wndHeight);
    setFixedSize(wndWidth, wndHeight);

    socket = new QWebSocket;

    QFile qssFile(":/qss/WindowWidgets.qss");
    if (qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(qssFile.readAll());
        qssFile.close();
    }

    lbBackground = new QLabel(this);
    titleBar = new TitleBar(this, iconSize);
    leNickName = new QLineEdit(this);
    leServerAddress = new QLineEdit(this);
    lbMessage = new QLabel(this);
    btnConnect = new QPushButton(this);
    btnReady = new QPushButton(this);

    lbBackground->setGeometry(0, 0, width(), height());
    lbBackground->setObjectName("Background");

    titleBar->setParent(this);
    titleBar->setGeometry(0, 0, wndWidth, iconSize * 2);
    titleBar->lbTitle->setText("Generals.OI");

    leNickName->setGeometry(leLeft, leTop, leWidth, leHeight);
    leNickName->setPlaceholderText("Nick Name");
    leNickName->setFocus();

    leServerAddress->setGeometry(leLeft, leTop + verItv, leWidth, leHeight);
    leServerAddress->setPlaceholderText("Server");

    lbMessage->setGeometry(btnLeft, leTop + verItv * 2, wndWidth - btnLeft * 2, leHeight);

    btnConnect->setGeometry(btnLeft, leTop + verItv * 3, btnWidth, btnHeight);
    btnConnect->setText("Connect");
    btnConnect->setDefault(true);

    btnReady->setGeometry(btnLeft + horItv, leTop + verItv * 3, btnWidth, btnHeight);
    btnReady->setText("Start");
    btnReady->setEnabled(false);

    font.setPointSize(fontSize);
    leNickName->setFont(font);
    leServerAddress->setFont(font);
    btnConnect->setFont(font);
    btnReady->setFont(font);

    fontSize = int((wndHeight + int(wndWidth / 2)) / (dpi * 80));
    font.setPointSize(fontSize);
    lbMessage->setFont(font);

    setTabOrder(leNickName, leServerAddress);
    setTabOrder(leServerAddress, btnConnect);
    setTabOrder(btnConnect, btnReady);

    connect(btnConnect, &QPushButton::clicked, this, &StartWindow::onConnectClicked);
    connect(socket, &QWebSocket::connected, this, &StartWindow::onConnected);
    connect(socket, &QWebSocket::disconnected, this, &StartWindow::onDisconnected);
    connect(btnReady, &QPushButton::clicked, this, &StartWindow::onReadyClicked);
    connect(socket, &QWebSocket::textMessageReceived, this, &StartWindow::onMessageReceived);
}

StartWindow::~StartWindow() {
    delete socket;
}

void StartWindow::onConnected() {
    btnConnect->setText("Disconnect");
    btnConnect->setEnabled(true);

    btnReady->setEnabled(true);
    leServerAddress->setEnabled(false);
    leNickName->setEnabled(false);

    auto nickName = leNickName->text();
    if (nickName.isEmpty()) {
        nickName = QString("%1 %2").arg(socket->peerAddress().toString(), QString::number(socket->peerPort()));
        qDebug() << QString("[startWindow.cpp] No nick name set, defaulting to %1.").arg(nickName);
    }

    if (gameWindow == nullptr) {
        qDebug() << "[startWindow.cpp] Creating game window.";
        gameWindow = new GameWindow(socket, nickName, nullptr);
    } else
        socket->sendTextMessage(QString("Connected:%1").arg(nickName));
}

void StartWindow::onDisconnected() {
    qDebug() << "[startWindow.cpp] Disconnected from server.";
    btnConnect->setText("Connect");
    btnConnect->setEnabled(true);
    lbMessage->setText("Disconnected");
    btnReady->setEnabled(false);
    socket->close();
}

void StartWindow::onConnectClicked() {
    static std::regex reg(R"((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.)"
                          R"((\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5]))");
    if (btnConnect->text() == "Connect") {
        btnConnect->setEnabled(false);
        auto strAddr = this->leServerAddress->text();
        if (std::regex_match(strAddr.toStdString(), reg) || strAddr == "localhost") {
            btnConnect->setText("Cancel");
            lbMessage->setText(QString("Connecting to: %1").arg(strAddr));
            socket->open(QUrl(QString("ws://%1:32767").arg(strAddr)));
        } else
            lbMessage->setText("Error: Wrong server address format");
    } else {
        socket->close();
    }
}

void StartWindow::onReadyClicked() {
    // TODO: Add team info and transfer it to server
    socket->sendTextMessage(QString("Readied:%1").arg(0));
    btnReady->setEnabled(false);
    btnConnect->setFocus();
}

void StartWindow::onMessageReceived(const QString &msg) {
    static bool inited = false, hid = false;
    QString msgType = msg.section(":", 0, 0);

    if (msgType == "Status") {
        lbMessage->setText(msg.section(":", 1));
    } else if (!inited && msgType == "InitMap") {
        inited = true;
    } else if (inited && !hid && msgType == "UpdateMap") {
        hid = true;
        hide();
    }
}
