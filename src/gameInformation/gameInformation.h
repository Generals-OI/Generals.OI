#ifndef GAME_INFO_H
#define GAME_INFO_H

#include <QObject>

struct PlayerInfo {
    QString nickName;
    int idPlayer{}, idTeam{};
    bool isSpect{}, isReadied{};

    PlayerInfo() = default;

    PlayerInfo(QString, int, int);

    PlayerInfo(QString, int, int, bool, bool);
};

enum GameMode {
    /// Client
    nearsightedness = 1,
    mistyVeil = 2,
    crystalClear = 4,
    silentWar = 128,
    /// Server
    leapfrog = 2048,
    cityState = 4096,
    allowTeaming = 65536
};

#endif // GAME_INFO_H
