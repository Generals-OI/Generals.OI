#ifndef GAME_INFO_H
#define GAME_INFO_H

#include <QObject>

#include "point.h"

const QString strCell[] = {"Land", "General", "City", "Mountain", "Swamp"};

const QString strArrow[] = {"Up", "Down", "Left", "Right"};

const int direction4[4][2] = {{-1, 0},
                              {0,  -1},
                              {1,  0},
                              {0,  1}};

struct MoveInfo {
    int startX, startY, direction;
    bool flag50p;

    MoveInfo();

    MoveInfo(Point start, int direction, bool flag50p);

    MoveInfo(int startX, int startY, int direction, bool flag50p);
};

struct PlayerInfo {
    QString nickName;
    int idPlayer{}, idTeam{};
    bool isSpect{}, isReadied{};

    PlayerInfo() = default;

    PlayerInfo(QString, int, int);

    PlayerInfo(QString, int, int, bool, bool);
};

enum GameMode {
    nearsighted = 1,
    mistyVeil = 2,
    crystalClear = 4,
    silentWar = 1024,
    leapfrog = 2048,
    cityState = 4096,
    allowTeaming = 65536
};

#endif // GAME_INFO_H
