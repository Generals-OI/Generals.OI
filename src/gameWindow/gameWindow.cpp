#include "gameWindow.h"

extern QString strFontRegular, strFontMedium, strFontBold;

GameWindow::GameWindow(QWebSocket *socket, QString name, QWidget *parent) : QWidget(parent) {
    dpi = qApp->primaryScreen()->logicalDotsPerInch() / 96.0;
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    setWindowTitle("Generals.OI - Game Window");

    screenGeometry = qApp->primaryScreen()->geometry();
    setGeometry(screenGeometry);
    showFullScreen();
    setAutoFillBackground(true);

    QFile qssFile(":/qss/GameWindowWidgets.qss");
    if (qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(qssFile.readAll());
        qssFile.close();
    } else
        qDebug() << "[gameWindow.cpp] Unable to load QSS file.";

    QPalette wndPalette(palette());
#if (QT_VERSION_MAJOR < 6)
    wndPalette.setColor(QPalette::Background, QColor(34, 34, 34));
#else
    wndPalette.setColor(QPalette::Window, QColor(34, 34, 34));
#endif
    setPalette(wndPalette);
    hide();

    gongPlayer = new QMediaPlayer(this);
#if (QT_VERSION_MAJOR < 6)
    gongPlayer->setMedia(QUrl("qrc:/aud/GongSound.mp3"));
    gongPlayer->setVolume(100);
#else
    QAudioOutput audioOutput;
    audioOutput.setVolume(100);
    gongPlayer->setAudioOutput(&audioOutput);
    gongPlayer->setSource(QUrl("qrc:/aud/GongSound.mp3"));
#endif

//    gongSoundEffect = new QSoundEffect(this);
//    gongSoundEffect->setSource(QUrl(":/aud/GongSound.wav"));
//    gongSoundEffect->setLoopCount(1);
//    gongSoundEffect->setVolume(1);

    nickName = std::move(name);
    webSocket = socket;

    connect(webSocket, &QWebSocket::binaryMessageReceived, this, &GameWindow::processMessage);
    webSocket->sendBinaryMessage(generateMessage("Connected", {nickName}));
}

void GameWindow::init() {
    width = globMap.length;
    height = globMap.width;
    screenWidth = screenGeometry.width();
    screenHeight = screenGeometry.height();

    // TODO: Another solution may be better
    int totWidth = width + rnkWidth + itvWidth;
    unitSize = std::min(int(screenWidth / totWidth), int(screenHeight / height));
    rnkUnitWidth = unitSize * 2;
    chatFontSize = unitSize / 3;
    minUnitSize = int(unitSize * 0.75);

    mapLeft = (screenWidth - unitSize * totWidth) / 2;
    mapTop = (screenHeight - unitSize * height) / 2;
    rnkLeft = screenWidth - rnkUnitWidth * 4;
    rnkTop = mapTop;

    qDebug() << "[gameWindow.cpp] Unit Size:" << unitSize;

    QPalette lbMainPalette;
    lbMainPalette.setColor(QPalette::WindowText, Qt::white);

    calcMapFontSize();
    boardFont.setFamily(strFontBold);
    boardFont.setPointSize(int(unitSize * mapFontSizePct[0] / dpi));
    boardFont.setStyleStrategy(QFont::PreferAntialias);
    chatFont.setFamily(strFontRegular);
    chatFont.setPointSize(int(chatFontSize / dpi));
    chatFont.setStyleStrategy(QFont::PreferAntialias);

    btnFocus = std::vector<std::vector<GameButton *>>(height + 1, std::vector<GameButton *>(width + 1));
    lbObstacle = lbMain = lbColor = std::vector<std::vector<QLabel *>>(height + 1, std::vector<QLabel *>(width + 1));
    visMain = std::vector<std::vector<bool>>(height + 1, std::vector<bool>(width + 1));
    for (int i = 0; i < 4; i++) {
        lbArrow[i] = std::vector<std::vector<QLabel *>>(height + 1, std::vector<QLabel *>(width + 1));
        cntArrow[i] = std::vector<std::vector<int>>(height + 1, std::vector<int>(width + 1));
    }
    fontType = std::vector<std::vector<int>>(height + 1, std::vector<int>(width + 1));

    wgtMap = new QWidget(this);
    wgtMap->setGeometry(mapLeft, mapTop, unitSize * width, unitSize * height);

    wgtButton = new QWidget(this);
    wgtButton->setGeometry(mapLeft, mapTop, unitSize * width, unitSize * height);

    QSizePolicy spMap(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // TODO: Change Spacing if it is necessary
    mapLayout = new QGridLayout(wgtMap);
    mapLayout->setSpacing(2);
    mapLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout = new QGridLayout(wgtButton);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    lbMapBgd = new QLabel(this);
    lbMapBgd->setObjectName("Background");
    lbMapBgd->setSizePolicy(spMap);
    lbMapBgd->show();
    mapLayout->addWidget(lbMapBgd, 1, 1, height, width);

    const QString strCell[] = {"Land", "General", "City", "Mountain", "Swamp"};
    const QString strArrow[] = {"Up", "Down", "Left", "Right"};

    for (int i = 1; i <= height; i++) {
        for (int j = 1; j <= width; j++) {
            Cell *cell = &globMap.map[i][j];
            QLabel *lbO = lbObstacle[i][j] = new QLabel(wgtMap);
            QLabel *lbC = lbColor[i][j] = new QLabel(wgtMap);
            QLabel *lbM = lbMain[i][j] = new QLabel(wgtMap);
            GameButton *btnF = btnFocus[i][j] = new GameButton(i, j, wgtButton, wgtMap);

            lbO->setSizePolicy(spMap);
            lbC->setSizePolicy(spMap);
            lbM->setSizePolicy(spMap);
            btnF->setSizePolicy(spMap);

            for (int k = 0; k < 4; k++) {
                QLabel *lbA = lbArrow[k][i][j] = new QLabel(wgtMap);
                lbA->setSizePolicy(spMap);
                lbA->setStyleSheet(QString("border-image: url(:/img/Arrow-%1.png);").arg(strArrow[k]));
                lbA->hide();
            }

            if (cell->type == CellType::mountain || cell->type == CellType::city)
                lbO->setObjectName("Obstacle");
            else if (cell->type == CellType::swamp)
                lbO->setObjectName("Swamp");
            else
                lbO->setObjectName("Land");
            lbM->setObjectName(strCell[(int) cell->type]);

            connect(btnF, &GameButton::focused, this, &GameWindow::onGameButtonFocused);

            lbM->setPalette(lbMainPalette);
            lbM->setFont(mapFont[0]);

            lbC->show(), lbO->show(), lbM->show(), btnF->show();

            mapLayout->addWidget(lbO, i, j, 1, 1);
            mapLayout->addWidget(lbC, i, j, 1, 1);
            mapLayout->addWidget(lbM, i, j, 1, 1);
            for (auto &k: lbArrow) mapLayout->addWidget(k[i][j], i, j, 1, 1);
            buttonLayout->addWidget(btnF, i, j, 1, 1);
        }
    }

    int sumRow = globMap.cntGnl + globMap.cntTeam;
    wgtBoard = new QWidget(this);
    wgtBoard->setGeometry(rnkLeft, rnkTop, rnkUnitWidth * 4, unitSize * (sumRow + 2));
    boardLayout = new QGridLayout(wgtBoard);
    boardLayout->setSpacing(2);
    lbBoard = QVector<BoardLabel>(sumRow + 1);

    lbRound = new QLabel(wgtBoard);
    lbRound->setObjectName("Rank");
    lbRound->setFont(boardFont);
    lbRound->show();
    boardLayout->addWidget(lbRound, 0, 0, 1, 3);

    for (int i = 0; i <= sumRow; i++)
        lbBoard[i].init(wgtBoard, boardFont, boardLayout, i + 1);
    lbBoard[0].updateContent("Name", "Army", "Land");
    lbBoard[0].lbName->setStyleSheet("background-color: rgba(255, 255, 255, 50);");

    teChats = new QTextEdit(this);
    leChat = new QLineEdit(this);
    new Highlighter(teChats->document(), cntPlayer, playersInfo);

    auto teLeft = mapLeft + (width + 2) * unitSize, teTop = wgtBoard->geometry().bottom() + unitSize;
    teChats->setGeometry(teLeft, teTop, screenWidth - teLeft, screenHeight - teTop - unitSize);
    leChat->setGeometry(teLeft, screenHeight - unitSize, screenWidth - teLeft, unitSize);

    teChats->setFont(chatFont);
    leChat->setFont(chatFont);

    connect(leChat, &QLineEdit::returnPressed, this, &GameWindow::sendChatMessage);

    teChats->show();
    leChat->show();

    for (auto &i: lbShadow) {
        i = new QLabel(this);
        i->setObjectName("Shadow");
        i->show();
    }

    focus = new Focus;
    focus->init(width, height);

    updateWindow(true);

    lbFocus = new QLabel(this);
    lbFocus->setObjectName("Focus");
    updateFocus(true, -1, 1, 1);
    lbFocus->show();
    lbFocus->setFocus();

    wgtButton->raise();
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
    int idDirection = -1;
    bool resized = false;

    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
            idDirection = 0;
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            idDirection = 1;
            break;
        case Qt::Key_Left:
        case Qt::Key_A:
            idDirection = 2;
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            idDirection = 3;
            break;
        case Qt::Key_Q:
            clearMove();
            break;
        case Qt::Key_E:
            cancelMove();
            break;
        case Qt::Key_Z:
            flagHalf = !flagHalf;
            break;
        case Qt::Key_Return:
            if (!leChat->hasFocus()) {
                leChat->setEnabled(true);
                leChat->setFocus();
            } else {
                leChat->setEnabled(false);
                lbFocus->setFocus();
            }
            break;
        case Qt::Key_0:
            mapLeft -= width / 2;
            mapTop -= height / 2;
            unitSize += 1;
            resized = true;
            break;
        case Qt::Key_9:
            if (unitSize <= minUnitSize)
                break;
            mapLeft += width / 2;
            mapTop += height / 2;
            unitSize -= 1;
            resized = true;
            break;
    }

    if (idDirection != -1) {
        updateFocus(false, idDirection);
        flagHalf = false;
    }

    if (resized) {
        calcMapFontSize();
        setGameFieldGeometry(QRect(mapLeft, mapTop, unitSize * width, unitSize * height));
        updateFocus(true, -1, focus->x, focus->y);
        flagHalf = !flagHalf;
        qDebug() << "[gameWindow.cpp] Current unit size:" << unitSize;
    }

    updateWindow(resized);
}

void GameWindow::setGameFieldGeometry(QRect geometry) const {
    wgtMap->setGeometry(geometry);
    wgtButton->setGeometry(geometry);
}

QRect GameWindow::mapPosition(const int x, const int y) {
    mapLeft = wgtMap->x(), mapTop = wgtMap->y();
    return {mapLeft + (y - 1) * unitSize, mapTop + (x - 1) * unitSize, unitSize, unitSize};
}

void GameWindow::cancelMove(bool flagFront) {
    if (!dqMsg.empty()) {
        MoveInfo data = flagFront ? dqMsg.front() : dqMsg.back();
        flagFront ? dqMsg.pop_front() : dqMsg.pop_back();

        if ((--cntArrow[data.direction][data.startX][data.startY]) == 0)
            lbArrow[data.direction][data.startX][data.startY]->hide();
        if (!flagFront)
            updateFocus(true, -1, data.startX, data.startY);
    }
}

void GameWindow::clearMove() {
    while (!dqMsg.empty())
        cancelMove();
}

void GameWindow::updateFocus(const bool flag, const int id, const int x, const int y) {
    int delta = unitSize / 15;
    const int dir[4][2] = {{-1, 0},
                           {0,  -1},
                           {1,  0},
                           {0,  1}};

    if (flag) {
        flagHalf ^= focus->x == x && focus->y == y;
        focus->set(x, y);
    } else {
        auto _focus = *focus;
        if (focus->move(dtDirection[id].x, dtDirection[id].y)) {
            lbArrow[id][_focus.x][_focus.y]->show();
            cntArrow[id][_focus.x][_focus.y]++;
            dqMsg.emplace_back(_focus, id, flagHalf);
        }
    }

    for (int i = 0; i < 4; i++) {
        auto pos = *focus;
        auto isLegal = pos.move(dir[i][0], dir[i][1]);
        if (isLegal && (!visMain[pos.x][pos.y] || globMap.map[pos.x][pos.y].type != CellType::mountain)) {
            auto mPos = mapPosition(pos.x, pos.y);
            lbShadow[i]->setGeometry(mPos.x(), mPos.y(), mPos.width(), mPos.height());
            lbShadow[i]->show();
        } else
            lbShadow[i]->hide();
    }

    auto pos = mapPosition(focus->x, focus->y);
    lbFocus->setGeometry(pos.x() - delta, pos.y() - delta, pos.width() + delta * 2, pos.height() + delta * 2);
    lbFocus->setFocus();
}

bool GameWindow::isPositionVisible(int x, int y) {
    if (idPlayer == -1 || (gameMode & GameMode::crystalClear))
        return true;
    if (gameMode & GameMode::mistyVeil)
        return idTeam == globMap.idTeam[globMap.map[x][y].belonging - 1];
    const int direction[9][2] = {-1, -1, -1, 0, -1, 1, 0, -1, 0, 0, 0, 1, 1, -1, 1, 0, 1, 1};

    for (auto k: direction) {
        int _x = x + k[0], _y = y + k[1];
        if (focus->valid(_x, _y)) {
            if (idTeam == globMap.idTeam[globMap.map[_x][_y].belonging - 1])
                return true;
        }
    }
    return false;
}

void GameWindow::updateWindow(bool forced) {
    auto calcFontType = [](int number) {
        if (number < 1000) return 0;
        return std::min(int(log10(number)) - 2, fontSizeCount - 1);
    };

    for (int i = 1; i <= height; i++) {
        for (int j = 1; j <= width; j++) {
            auto cell = &globMap.map[i][j];
            auto _cell = &_globMap.map[i][j];
            auto lbO = lbObstacle[i][j];
            auto lbM = lbMain[i][j];
            auto lbC = lbColor[i][j];

            auto vis = isPositionVisible(i, j);
            auto flagNum = cell->number != _cell->number, flagVis = vis != visMain[i][j],
                    flagType = cell->type != _cell->type, flagBelonging = cell->belonging != _cell->belonging;

            if (flagType)
                lbM->setStyleSheet("border-image: url(:/img/City.png);");

            if (flagNum || flagVis || forced) {
                auto fType = calcFontType(cell->number);
                if (fType != fontType[i][j] || forced) {
                    fontType[i][j] = fType;
                    lbM->setFont(mapFont[fType]);
                }
                if (cell->type == CellType::city || cell->type == CellType::general || cell->belonging)
                    lbM->setText(QString::number(cell->number));
                else
                    lbM->setText("");
            }

            if (vis) {
                if (flagBelonging || flagVis || forced) {
                    if (cell->belonging || cell->type == CellType::land)
                        lbC->setStyleSheet(QString("background-color: %1;").arg(strColor[cell->belonging]));
                    else if (cell->type == CellType::mountain)
                        lbC->setStyleSheet("background-color: rgb(187, 187, 187);");
                    else
                        lbC->setStyleSheet("background-color: rgb(128, 128, 128);");
                }

                *_cell = *cell;
                visMain[i][j] = vis;

                lbM->show(), lbC->show(), lbO->hide();
            } else {
                lbM->hide(), lbC->hide(), lbO->show();
            }
        }
    }

    int curRow = 0;
    lbRound->setText(QString("Round: ").append(QString::number(globMap.round)));

    // TODO: Response to Game Modifiers (silentWar)
    for (const auto &stat: globMap.stat) {
        const auto &teamStat = stat.first;
        lbBoard[++curRow].updateContent(QString("Team %1").arg(teamStat.id),
                                        QString::number(teamStat.army), QString::number(teamStat.land));
        lbBoard[curRow].lbName->setStyleSheet("background-color: rgba(255, 255, 255, 50);");
        for (const auto &playerStat: stat.second) {
            lbBoard[++curRow].updateContent(playersInfo[playerStat.id].nickName,
                                            QString::number(playerStat.army), QString::number(playerStat.land));
            lbBoard[curRow].lbName->setStyleSheet(QString("background-color: %1").arg(strColor[playerStat.id]));
        }
    }
}

void GameWindow::processMessage(const QByteArray &msg) {
    auto json = loadJson(msg);
    auto msgType = json.first.toString();
    auto msgData = json.second.toArray();
    qDebug() << "[gameWindow.cpp] Received:" << msgType;

    if (msgType == "PlayerInfo") {
        idPlayer = msgData.at(0).toInt();
        idTeam = msgData.at(1).toInt();
        gotPlayerInfoMsg = true;
        gongPlayer->play();
//        gongSoundEffect->play();
    } else if (msgType == "PlayersInfo") {
        cntPlayer = msgData.at(0).toInt();
        for (int i = 1; i <= cntPlayer; i++) {
            auto playerData = msgData.at(i).toArray();
            auto nick = playerData.at(0).toString();
            int player = playerData.at(1).toInt();
            int team = playerData.at(2).toInt();
            playersInfo[player] = PlayerInfo(nick, player, team);
        }
        gotPlayersInfoMsg = true;
    } else if (msgType == "InitGame") {
        gameMode = msgData.at(0).toInt();
        globMap.import(msgData.at(1).toString().toStdString());
        _globMap = globMap;
        gotInitMsg = true;
        init();
    } else if (gotPlayerInfoMsg && gotInitMsg && gotPlayersInfoMsg) {
        if (msgType == "Chat") {
            teChats->append(QString("%1: %2").arg(msgData.at(0).toString(), msgData.at(1).toString()));
        } else if (!gameEnded && msgType == "UpdateMap") {
            globMap.import(msgData.at(0).toString().toStdString());
            updateWindow();

            if (!gameWindowShown) {
                gameWindowShown = true;
                show();
            }

            if (globMap.gameOver()) {
                bool flagWon = globMap.stat[0].first.id == idTeam;
                endWindow = new EndWindow(this, flagWon);
                endWindow->show();
                endWindow->btnWatch->setDisabled(true);
                connect(endWindow->btnExit, &QPushButton::clicked, qApp, &QApplication::quit);
                gameEnded = true;
            }

            if (moved)
                cancelMove(true);
            bool move = !dqMsg.empty();

            if (move) {
                auto moveData = dqMsg.front();
                QJsonArray jsonData;
                jsonData.push_back(idPlayer);
                jsonData.push_back(moveData.startX);
                jsonData.push_back(moveData.startY);
                jsonData.push_back(dtDirection[moveData.direction].x);
                jsonData.push_back(dtDirection[moveData.direction].y);
                jsonData.push_back(moveData.flag50p);

                if (idPlayer != -1) // Spectators are not allow to move
                    webSocket->sendBinaryMessage(generateMessage("Move", jsonData));
            }

            moved = move;
        }
    }
}

void GameWindow::onGameButtonFocused(const int &x, const int &y) {
    updateFocus(true, -1, x, y);
}

void GameWindow::calcMapFontSize() {
    for (int i = 0; i < fontSizeCount; i++) {
        mapFont[i].setFamily(strFontMedium);
        mapFont[i].setPointSize(int(unitSize * mapFontSizePct[i] / dpi));
    }
}

void GameWindow::sendChatMessage() {
    auto msg = leChat->text();
    if (msg.size()) {
        webSocket->sendBinaryMessage(generateMessage("Chat", {nickName, msg}));
    }

    leChat->clear();
    leChat->setEnabled(false);
    lbFocus->setFocus();
}

GameWindow::~GameWindow() {
    delete focus;
}
