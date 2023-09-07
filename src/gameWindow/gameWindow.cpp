#include "gameWindow.h"

extern QString strFontRegular, strFontMedium, strFontBold;

GameWindow::GameWindow(QWebSocket *socket, QWidget *parent) : QWidget(parent) {
    dpi = qApp->primaryScreen()->logicalDotsPerInch() / 96.0;
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    setWindowTitle("Generals.OI - Game Window");

    if (parent)
        wndGeometry = parent->geometry();
    else
        wndGeometry = qApp->primaryScreen()->geometry();
    setGeometry(wndGeometry);
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

    webSocket = socket;
    connect(webSocket, &QWebSocket::binaryMessageReceived, this, &GameWindow::processMessage);
}

void GameWindow::setNickname(const QString &newNickname) {
    nickName = newNickname;
}

void GameWindow::init() {
    width = cltMap.length;
    height = cltMap.width;
    screenWidth = wndGeometry.width();
    screenHeight = wndGeometry.height();

    // TODO: Another solution may be better
    int totWidth = width + rnkWidth + itvWidth;
    unitSize = std::min(int(screenWidth / totWidth), int(screenHeight / height));
    rnkUnitWidth = unitSize * 2;
    chatFontSize = unitSize / 3;
    minUnitSize = int(unitSize * 0.75);

    mapLeft = (screenWidth - unitSize * totWidth) / 2;
    mapTop = (screenHeight - unitSize * height) / 2;
    rnkLeft = screenWidth - unitSize * rnkWidth;
    rnkTop = 0;

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

    visMain = std::vector<std::vector<bool>>(height + 1, std::vector<bool>(width + 1));
    for (int i = 0; i < 4; i++) 
        cntArrow[i] = std::vector<std::vector<int>>(height + 1, std::vector<int>(width + 1));
    fontType = std::vector<std::vector<int>>(height + 1, std::vector<int>(width + 1));

    endWindow = new EndWindow(this);
    surrenderWindow = new SurrenderWindow(this);
    connect(surrenderWindow, &SurrenderWindow::surrendered, this, &GameWindow::onSurrender);
    connect(endWindow, &EndWindow::watch, this, &GameWindow::onSpectate);

    QSizePolicy spMap(QSizePolicy::Preferred, QSizePolicy::Expanding);

    gameMapGrid = new GameMapGrid(width, height, this);
    gameMapGrid->setGeometry(mapLeft, mapTop, unitSize * width, unitSize * height);
    gameMapGrid->wgtButton->setGeometry(mapLeft, mapTop, unitSize * width, unitSize * height);

    // TODO: Change Spacing if necessary
    gameMapGrid->mapLayout->setSpacing(2);
    gameMapGrid->mapLayout->setContentsMargins(0, 0, 0, 0);
    gameMapGrid->buttonLayout->setSpacing(0);
    gameMapGrid->buttonLayout->setContentsMargins(0, 0, 0, 0);

    gameMapGrid->lbMapBgd->setObjectName("Background");
    gameMapGrid->lbMapBgd->setSizePolicy(spMap);
    gameMapGrid->lbMapBgd->show();
    gameMapGrid->mapLayout->addWidget(gameMapGrid->lbMapBgd, 1, 1, height, width);

    const QString strCell[] = {"Land", "General", "City", "Mountain", "Swamp"};
    const QString strArrow[] = {"Up", "Down", "Left", "Right"};

    for (int i = 1; i <= height; i++) {
        for (int j = 1; j <= width; j++) {
            Cell *cell = &cltMap.map[i][j];
            QLabel *lbO = gameMapGrid->lbObstacle[i][j] = new QLabel(gameMapGrid);
            QLabel *lbC = gameMapGrid->lbColor[i][j] = new QLabel(gameMapGrid);
            QLabel *lbM = gameMapGrid->lbMain[i][j] = new QLabel(gameMapGrid);
            GameButton *btnF = gameMapGrid->btnFocus[i][j] = new GameButton(i, j, gameMapGrid->wgtButton, gameMapGrid);

            lbO->setSizePolicy(spMap);
            lbC->setSizePolicy(spMap);
            lbM->setSizePolicy(spMap);
            btnF->setSizePolicy(spMap);
            lbM->setAlignment(Qt::AlignCenter);

            for (int k = 0; k < 4; k++) {
                QLabel *lbA = gameMapGrid->lbArrow[k][i][j] = new QLabel(gameMapGrid);
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

            gameMapGrid->mapLayout->addWidget(lbO, i, j, 1, 1);
            gameMapGrid->mapLayout->addWidget(lbC, i, j, 1, 1);
            gameMapGrid->mapLayout->addWidget(lbM, i, j, 1, 1);
            for (auto &k: gameMapGrid->lbArrow) gameMapGrid->mapLayout->addWidget(k[i][j], i, j, 1, 1);
            gameMapGrid->buttonLayout->addWidget(btnF, i, j, 1, 1);
        }
    }

    int sumRow = cltMap.cntPlayer + cltMap.cntTeam;
    wgtBoard = new QWidget(this);
    wgtBoard->setGeometry(rnkLeft, rnkTop, unitSize * rnkWidth, unitSize * (sumRow + 2));
    boardLayout = new QGridLayout(wgtBoard);
    boardLayout->setSpacing(2);
    lbBoard = QVector<BoardLabel>(sumRow + 1);

    lbRound = new QLabel(wgtBoard);
    lbRound->setObjectName("Rank");
    lbRound->setAlignment(Qt::AlignCenter);
    lbRound->setFont(boardFont);
    lbRound->show();
    boardLayout->addWidget(lbRound, 0, 0, 1, 3);

    for (int i = 0; i <= sumRow; i++)
        lbBoard[i].init(wgtBoard, boardFont, boardLayout, i + 1);
    lbBoard[0].updateContent("Name", "Army", "Land");
    lbBoard[0].lbName->setStyleSheet("background-color: rgba(255, 255, 255, 96);");

    teChats = new QTextEdit(this);
    leChat = new QLineEdit(this);
    new Highlighter(teChats->document(), cntPlayer, playersInfo, chatFont);

    auto teLeft = mapLeft + (width + 2) * unitSize, teTop = wgtBoard->geometry().bottom() + unitSize;
    teChats->setGeometry(teLeft, teTop, screenWidth - teLeft, screenHeight - teTop - unitSize);
    leChat->setGeometry(teLeft, screenHeight - unitSize, screenWidth - teLeft, unitSize);

    teChats->setFont(chatFont);
    leChat->setFont(chatFont);

    connect(leChat, &QLineEdit::returnPressed, this, &GameWindow::sendChatMessage);

    teChats->show();
    leChat->show();
    leChat->setEnabled(false);

    for (auto &i: lbShadow) {
        i = new QLabel(this);
        i->setObjectName("Shadow");
        if (idPlayer != -1) i->show();
        else i->hide();
    }

    focus = new Focus;
    focus->init(width, height);

    updateWindow(true);

    lbFocus = new QLabel(this);
    lbFocus->setObjectName("Focus");
    if (idPlayer != -1) {
        updateFocus(true, -1, 1, 1);
        lbFocus->show();
        lbFocus->setFocus();
    } else
        lbFocus->hide();

    gameMapGrid->wgtButton->raise();
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
        case Qt::Key_G:
            focusGeneral();
            break;
        case Qt::Key_Return:
            if (idPlayer != -1) {
                if (!leChat->isEnabled() && !gameEnded) {
                    leChat->setEnabled(true);
                    leChat->setFocus();
                } else {
                    leChat->setEnabled(false);
                    lbFocus->setFocus();
                }
            }
            break;
        case Qt::Key_0:
            mapLeft -= width;
            mapTop -= height;
            unitSize += 2;
            resized = true;
            break;
        case Qt::Key_9:
            if (unitSize - 1 < minUnitSize)
                break;
            mapLeft += width;
            mapTop += height;
            unitSize -= 2;
            resized = true;
            break;
        case Qt::Key_Escape:
            if (!surrendered && idPlayer != -1) {
                surrenderWindow->show();
                surrenderWindow->raise();
            }
            break;
    }

    if (idDirection != -1 && idPlayer != -1 && !surrendered) {
        if (event->modifiers() == Qt::ShiftModifier) {
            auto pos = *focus;
            if (pos.move(dtDirection[idDirection].x, dtDirection[idDirection].y))
                updateFocus(true, -1, pos.x, pos.y);
        } else
            updateFocus(false, idDirection);
        flagHalf = false;
    }

    if (resized) {
        calcMapFontSize();
        setGameFieldGeometry(QRect(mapLeft, mapTop, unitSize * width, unitSize * height));
        if (idPlayer != -1) {
            updateFocus(true, -1, focus->x, focus->y);
            flagHalf = !flagHalf;
        }
        qDebug() << "[gameWindow.cpp] Current unit size:" << unitSize;
    }

    updateWindow(resized);
}

void GameWindow::setGameFieldGeometry(QRect geometry) const {
    gameMapGrid->setGeometry(geometry);
    gameMapGrid->wgtButton->setGeometry(geometry);
}

QRect GameWindow::mapPosition(const int x, const int y) {
    mapLeft = gameMapGrid->x(), mapTop = gameMapGrid->y();
    return {mapLeft + (y - 1) * unitSize, mapTop + (x - 1) * unitSize, unitSize, unitSize};
}

void GameWindow::cancelMove(bool flagFront) {
    if (!dqMsg.empty()) {
        MoveInfo data = flagFront ? dqMsg.front() : dqMsg.back();
        flagFront ? dqMsg.pop_front() : dqMsg.pop_back();

        if ((--cntArrow[data.direction][data.startX][data.startY]) == 0)
            gameMapGrid->lbArrow[data.direction][data.startX][data.startY]->hide();
        if (!flagFront && idPlayer != -1)
            updateFocus(true, -1, data.startX, data.startY);
    }
}

void GameWindow::clearMove() {
    while (!dqMsg.empty())
        cancelMove();
}

void GameWindow::updateFocus(const bool clicked, const int id, const int x, const int y) {
    int delta = unitSize / 15;
    const int dir[4][2] = {{-1, 0},
                           {0,  -1},
                           {1,  0},
                           {0,  1}};

    if (clicked) {
        flagHalf ^= focus->x == x && focus->y == y;
        focus->set(x, y);
    } else {
        auto _focus = *focus;
        if (focus->move(dtDirection[id].x, dtDirection[id].y)) {
            gameMapGrid->lbArrow[id][_focus.x][_focus.y]->show();
            cntArrow[id][_focus.x][_focus.y]++;
            dqMsg.emplace_back(_focus, id, flagHalf);
        }
    }

    for (int i = 0; i < 4; i++) {
        auto pos = *focus;
        auto isLegal = pos.move(dir[i][0], dir[i][1]);
        if (idPlayer != -1 && isLegal &&
            (!(isPositionVisible(pos.x, pos.y) || !clicked && !(gameMode & GameMode::mistyVeil)) ||
             cltMap.map[pos.x][pos.y].type != CellType::mountain)) {
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
    // TODO: spectate globally when team lost
    if (idPlayer == -1 || (gameMode & GameMode::crystalClear) || gameEnded)
        return true;
    if (gameMode & GameMode::mistyVeil)
        return idTeam == cltMap.idTeam[cltMap.map[x][y].belonging - 1];
    const int direction[9][2] = {-1, -1, -1, 0, -1, 1, 0, -1, 0, 0, 0, 1, 1, -1, 1, 0, 1, 1};

    for (auto k: direction) {
        int _x = x + k[0], _y = y + k[1];
        if (focus->valid(_x, _y)) {
            if (idTeam == cltMap.idTeam[cltMap.map[_x][_y].belonging - 1])
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
            auto cell = &cltMap.map[i][j];
            auto _cell = &_cltMap.map[i][j];
            auto lbO = gameMapGrid->lbObstacle[i][j];
            auto lbM = gameMapGrid->lbMain[i][j];
            auto lbC = gameMapGrid->lbColor[i][j];

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
    lbRound->setText(QString("Round: ").append(QString::number(cltMap.round)));

    // TODO: Respond to Game Modifiers (silentWar)
    for (const auto &stat: cltMap.stat) {
        const auto &teamStat = stat.first;
        lbBoard[++curRow].updateContent(QString("Team %1").arg(teamStat.id),
                                        QString::number(teamStat.army), QString::number(teamStat.land));
        lbBoard[curRow].lbName->setStyleSheet("background-color: rgba(255, 255, 255, 96);");
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
        auto gameInfo = toVectorInt(msgData.toVariantList());
        gameMode = gameInfo[gameInfo.size() - 1];
#if (QT_VERSION_MAJOR < 6)
        cltMap.importCM(gameInfo.mid(0, gameInfo.size() - 1));
#else
        cltMap.importCM(gameInfo.first(gameInfo.size() - 1));
#endif
        qDebug() << "[gameWindow.cpp] ClientMap loaded";
        _cltMap = cltMap;
        gotInitMsg = true;
        init();
        focusGeneral();
    } else if (gotPlayerInfoMsg && gotInitMsg && gotPlayersInfoMsg) {
        if (msgType == "Chat") {
            teChats->append(QString("%1: %2").arg(msgData.at(0).toString(), msgData.at(1).toString()));
        } else if (msgType == "UpdateMap") {
            cltMap.loadDiff(toVectorInt(msgData.toVariantList()));
            updateWindow();

            if (!gameWindowShown) {
                gameWindowShown = true;
                show();
            }

            if (cltMap.gameOver()) {
                gameEnded = true;
                updateWindow(true);
                endWindow->gameEnded();

                if (idPlayer != -1) {
                    if (cltMap.stat[0].first.id == idTeam)
                        endWindow->updateText("You Won!",
                                              "This is your crowning glory.\nYou showed your formidable capacity.");
                    else if (!surrendered)
                        endWindow->updateText("You Lost.", "You were captured\nand your efforts were in vain.");
                } else {
                    endWindow->updateText("Game Over.", "You witnessed fierce battles with ingenious tactics. "
                                                        "Hope you find it rewarding.");
                }
                endWindow->show();
            }

            if (moved)
                cancelMove(true);
            bool move = !dqMsg.empty();

            if (move && !gameEnded) {
                auto moveData = dqMsg.front();
                QJsonArray jsonData;
                jsonData.push_back(idPlayer);
                jsonData.push_back(moveData.startX);
                jsonData.push_back(moveData.startY);
                jsonData.push_back(dtDirection[moveData.direction].x);
                jsonData.push_back(dtDirection[moveData.direction].y);
                jsonData.push_back(moveData.flag50p);
                webSocket->sendBinaryMessage(generateMessage("Move", jsonData));
            }

            moved = move;
        }
    }
}

void GameWindow::onGameButtonFocused(const int &x, const int &y) {
    if (idPlayer != -1)
        updateFocus(true, -1, x, y);
    leChat->setEnabled(false);
}

void GameWindow::calcMapFontSize() {
    for (int i = 0; i < fontSizeCount; i++) {
        mapFont[i].setFamily(strFontMedium);
        mapFont[i].setPointSize(int(unitSize * mapFontSizePct[i] / dpi));
    }
}

void GameWindow::sendChatMessage() {
    auto msg = leChat->text();
    if (!msg.isEmpty())
        webSocket->sendBinaryMessage(generateMessage("Chat", {nickName, msg}));
    leChat->clear();
}

GameWindow::~GameWindow() {
    delete focus;
}

void GameWindow::onSurrender() {
    webSocket->sendBinaryMessage(generateMessage("Surrender", {idPlayer}));
    surrendered = true;
    clearMove();
    endWindow->updateText("You Surrendered.", "You abandoned your general\nand gave up the fight.");
    endWindow->show();
}

void GameWindow::onSpectate() {
    if (surrendered)
        spectated = true;
}

void GameWindow::focusGeneral() {
    // TODO: Lower time complexity
    int x = 0, y = 0;
    for (int i = 1; i <= cltMap.width && !x; i++)
        for (int j = 1; j <= cltMap.length; j++)
            if (cltMap.map[i][j].type == CellType::general && cltMap.map[i][j].belonging == idPlayer) {
                x = i;
                y = j;
                break;
            }
    if (idPlayer != -1)
        updateFocus(true, 0, x, y);
}
