#include "replayWindow.h"
#include "ui_controlPanel.h"

ReplayWindow::ReplayWindow(QWidget *parent) : QWidget(parent), uiCtrlPanel(new Ui::ControlPanel) {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setGeometry(QApplication::primaryScreen()->geometry());

    socket = new QWebSocket;
    timer = new QTimer(this);
    gameWindow = new GameWindow(socket, this);
    wCtrlPanel = new QWidget(gameWindow);

    QFile qssFile(":/qss/WindowWidgets.qss");
    if (qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        wCtrlPanel->setStyleSheet(qssFile.readAll());
        qssFile.close();
    } else
        qDebug() << "[replayWindow.cpp] Unable to load QSS file.";

    uiCtrlPanel->setupUi(wCtrlPanel);
    connect(uiCtrlPanel->pbSubmit, &QPushButton::clicked, this, &ReplayWindow::updateSettings);
    connect(uiCtrlPanel->pbStatus, &QPushButton::clicked, this, &ReplayWindow::changeStatus);

    QString strReplayFile;
    // TODO: Choose a proper directory for replay files
    strReplayFile = QFileDialog::getOpenFileName(
            this, "Choose replay file",
            /*QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first()*/
            ".",
            "All files (*.*)"
    );
    if (strReplayFile.isEmpty()) QApplication::quit();
    QFile replayFile(strReplayFile);
    QByteArray byteArray;
    if (replayFile.open(QIODevice::ReadOnly)) {
        byteArray = replayFile.readAll();
        replayFile.close();
    } else {
        qDebug() << "[replayWindow.cpp] Cannot load replay file";
        qApp->quit();
    }

    // Preload & Backup serverMap
    serverMap.loadByteArray(byteArray);
    recorder.importRecord(byteArray);
    gameMode |= recorder.gameMode;

    for (int i = 0; i < recorder.moves.size(); i++) {
        if (i % chunkSize == 0) serverMaps.append(serverMap);
        for (auto move: recorder.moves[serverMap.round]) {
            if (Recorder::isSurrender(move))
                serverMap.surrender(move.idPlayer);
            else
                serverMap.move(move.idPlayer, Point(move.startX, move.startY),
                               direction4[move.direction][0], direction4[move.direction][1], move.flag50p, gameMode);
        }
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

    emit socket->binaryMessageReceived(generateMessage("GameMode", {gameMode, recorder.moves.size() - 1}));
    emit socket->binaryMessageReceived(generateMessage(
            "InitGame", QJsonArray::fromVariantList(toVariantList(serverMap.toVectorSM()))));

    gameWindow->leChat->hide();
    gameWindow->teChats->hide();

    // TODO: Place control panel correctly
    wCtrlPanel->setGeometry(gameWindow->teChats->geometry());
    wCtrlPanel->raise();
    uiCtrlPanel->sbRound->setRange(1, recorder.moves.size() - 1);

    // TODO: Add view options to `cbView`
    uiCtrlPanel->cbView->addItem("[Global]");
    for (auto &player: recorder.players)
        uiCtrlPanel->cbView->addItem(player.first);

    connect(timer, &QTimer::timeout, this, &ReplayWindow::sendMap);
    timer->start(250);
    uiCtrlPanel->pbStatus->setText("Pause");
    uiCtrlPanel->sbRound->setEnabled(false);
}

void ReplayWindow::updateSettings() {
    // update identity in GameWindow
    int newIdentity = uiCtrlPanel->cbView->currentIndex();
    if (identity != newIdentity) {
        gameWindow->setIdentity(identity = newIdentity);
        qDebug() << "[replayWindow.cpp] New identity:" << identity;
    }

    int round = uiCtrlPanel->sbRound->value() - 1;
    if (round != serverMap.round) {
        if (!paused) changeStatus();
        auto &tempMap = serverMaps.at(round / chunkSize);
//        serverMap.copyWithDiff(tempMap);
        serverMap = tempMap;
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
//        qDebug() << "[replayWindow.cpp] sendMap():" << move.idPlayer << move.startX << move.startY \
                 << direction4[move.direction][0] << direction4[move.direction][1] << move.flag50p << gameMode;
        if (Recorder::isSurrender(move))
            serverMap.surrender(move.idPlayer);
        else
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
