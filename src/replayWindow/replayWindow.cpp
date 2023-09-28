#include "replayWindow.h"
#include "ui_controlPanel.h"

ReplayWindow::ReplayWindow(QWidget *parent) : QWidget(parent), uiCtrlPanel(new Ui::ControlPanel) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setGeometry(QApplication::primaryScreen()->geometry());

    socket = new QWebSocket;
    timer = new QTimer(this);
    gameWindow = new GameWindow(socket, this);
    wCtrlPanel = new QWidget(gameWindow);
    uiCtrlPanel->setupUi(wCtrlPanel);

    connect(uiCtrlPanel->pbSubmit, &QPushButton::clicked, this, &ReplayWindow::updateSettings);
    connect(uiCtrlPanel->pbStatus, &QPushButton::clicked, this, &ReplayWindow::changeStatus);

    QString replayFile;
    while (replayFile.isEmpty())
        replayFile = QFileDialog::getOpenFileName(
                this, "Choose replay file",
                QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first(),
                "All files (*.*)"
        );
    QFile file(replayFile);
    QByteArray byteArray;
    if (file.open(QIODevice::ReadOnly)) {
        byteArray = file.readAll();
        file.close();
    } else {
        qDebug() << "[replayWindow.cpp] Cannot load replay file";
        QApplication::quit();
    }

    // Preload & Backup serverMap
    serverMap.loadByteArray(byteArray);
    recorder.importRecord(byteArray);
    gameMode |= recorder.gameMode;

    for (int i = 0; i < recorder.moves.size(); i++) {
        if (i % chunkSize == 0) serverMaps.append(serverMap);
        for (auto move: recorder.moves[serverMap.round])
            serverMap.move(move.idPlayer, Point(move.startX, move.startY),
                           direction4[move.direction][0], direction4[move.direction][1], move.flag50p, gameMode);
        serverMap.addRound();
    }

    serverMap = serverMaps.at(0);

    QJsonArray playersInfoData;
    playersInfoData.push_back(recorder.players.size());

    for (int i = 0; i < recorder.players.size(); i++) {
        QJsonArray playerInfoData;
        playerInfoData.append(recorder.players[i].first);
        playerInfoData.append(i + 1);
        playerInfoData.append(recorder.players[i].second);
        playersInfoData.append(playerInfoData);
    }

    emit socket->binaryMessageReceived(generateMessage("PlayerInfo", {-1, -1}));
    emit socket->binaryMessageReceived(generateMessage("PlayersInfo", playersInfoData));

    emit socket->binaryMessageReceived(generateMessage(
            "InitGame", QJsonArray::fromVariantList(toVariantList(serverMap.toVectorSM())) + gameMode));

    gameWindow->leChat->hide();
    gameWindow->teChats->hide();

    // TODO: Place control panel correctly
    wCtrlPanel->setGeometry(gameWindow->teChats->geometry());
    wCtrlPanel->raise();
    uiCtrlPanel->sbRound->setRange(1, recorder.moves.size() - 1);
    // TODO: Add view options to `cbView`

    connect(timer, &QTimer::timeout, this, &ReplayWindow::sendMap);
    timer->start(250);
    uiCtrlPanel->pbStatus->setText("Pause");
    uiCtrlPanel->sbRound->setEnabled(false);
}

void ReplayWindow::updateSettings() {
    // TODO: update identity in GameWindow
    // gameWindow->changeIdentity(uiCtrlPanel->cbView->currentIndex());

    int round = uiCtrlPanel->sbRound->value() - 1;
    if (round != serverMap.round) {
        if (!paused) changeStatus();
        serverMap = serverMaps.at(round / chunkSize);
        for (int i = round / chunkSize * chunkSize; i < round; i++) {
            for (auto move: recorder.moves[serverMap.round])
                serverMap.move(move.idPlayer, Point(move.startX, move.startY),
                               direction4[move.direction][0], direction4[move.direction][1], move.flag50p, gameMode);
            serverMap.addRound();
        }
        
        // TODO: Let @gfy1729 transfer `round` in his code (optional)
        gameWindow->cltMap.round = round;
        sendMap();
    }

    speed = uiCtrlPanel->sbSpeed->value();
    timer->setInterval(int(500 / speed));
}

void ReplayWindow::changeStatus() {
    paused = !paused;
    if (paused) {
        timer->stop();
        uiCtrlPanel->pbStatus->setText("Continue");
        uiCtrlPanel->sbRound->setEnabled(true);
    } else {
        timer->start(int(500 / speed));
        uiCtrlPanel->pbStatus->setText("Pause");
        uiCtrlPanel->sbRound->setEnabled(false);
    }
}

void ReplayWindow::sendMap() {
    if (serverMap.round + 1 >= recorder.moves.size())
        return;

    for (auto move: recorder.moves[serverMap.round]) {
        qDebug() << "[replayWindow.cpp] sendMap():" << move.idPlayer << move.startX << move.startY
                 << direction4[move.direction][0] << direction4[move.direction][1] << move.flag50p << gameMode;
        serverMap.move(move.idPlayer, Point(move.startX, move.startY),
                       direction4[move.direction][0], direction4[move.direction][1], move.flag50p, gameMode);
    }

    serverMap.addRound();
    uiCtrlPanel->sbRound->setValue(serverMap.round);
    emit socket->binaryMessageReceived(generateMessage(
            "UpdateMap", QJsonArray::fromVariantList(toVariantList(serverMap.exportDiff()))));
}

ReplayWindow::~ReplayWindow() {
    delete uiCtrlPanel;
}
