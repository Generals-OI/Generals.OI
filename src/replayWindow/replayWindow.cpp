#include "replayWindow.h"
#include "ui_controlPanel.h"

ReplayWindow::ReplayWindow(QWidget *parent) : QWidget(parent), uiCtrlPanel(new Ui::ControlPanel) {
    socket = new QWebSocket;
    timer = new QTimer(this);
    gameWindow = new GameWindow(socket, this);
    wCtrlPanel = new QWidget(this);
    uiCtrlPanel->setupUi(wCtrlPanel);

    connect(uiCtrlPanel->pbSubmit, &QPushButton::clicked, this, &ReplayWindow::updateSettings);
    connect(uiCtrlPanel->pbStatus, &QPushButton::clicked, this, &ReplayWindow::updateStatus);

    // TODO: Load replay files & preload serverMaps and save several backups

    // TODO: Hide ChatBoxes and place ControlPanel

    // TODO: Generate & Send players' info
//    std::vector<int> teamInfo(cntPlayer), newTeamId(maxPlayerNum + 1);
//    int totTeam = 0;
//    for (int i = 1; i <= maxPlayerNum; i++) {
//        while (i <= maxPlayerNum && !teamMbrCnt[i])
//            i++;
//        if (i <= maxPlayerNum)
//            newTeamId[i] = ++totTeam;
//    }
//    for (auto &player: clients)
//        player.idTeam = teamInfo[player.idPlayer - 1] = newTeamId[player.idTeam];
//
//    QJsonArray playersInfoData;
//    playersInfoData.push_back(cntPlayer);
//
//    for (auto it = clients.begin(); it != clients.end(); it++) {
//        auto itSocket = it.key();
//        auto itInfo = it.value();
        // TODO: Send player info (Spectator in this case)
//        itSocket->sendBinaryMessage(generateMessage("PlayerInfo", {itInfo.idPlayer, itInfo.idTeam}));
//
//        if (!itInfo.isSpect) {
//            QJsonArray playerInfoData;
//            playerInfoData.push_back(itInfo.nickName);
//            playerInfoData.push_back(itInfo.idPlayer);
//            playerInfoData.push_back(itInfo.idTeam);
//
//            playersInfoData.push_back(playerInfoData);
//        }
//    }
//
//    baPlayersInfo = generateMessage("PlayersInfo", playersInfoData);
//    emit sendMessage(baPlayersInfo);

    // TODO: Send serverMap & gameMode
//    emit sendMessage(generateMessage(
//            "InitGame", QJsonArray::fromVariantList(toVariantList(serMap->toVectorSM())) + gameMode));

    connect(timer, &QTimer::timeout, this, &ReplayWindow::sendMap);
    timer->start(250);
}

void ReplayWindow::updateSettings() {
    // TODO: update identity in GameWindow
    // gameWindow->changeIdentity(uiCtrlPanel->cbView->currentIndex());
    speed = uiCtrlPanel->sbSpeed->value();
    timer->setInterval(int(500 / speed));
}

void ReplayWindow::updateStatus() {
    paused = !paused;
    if (paused) timer->stop();
    else timer->setInterval(int(500 / speed));
}

void ReplayWindow::sendMap() {
    // TODO: get & process all moves from recorder
    // TODO: send new map to gameWindow
}

ReplayWindow::~ReplayWindow() {
    delete uiCtrlPanel;
}

