#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <QObject>

struct PlayerInfo {
public:
    QString nickName;
    int idPlayer{}, idTeam{};
    bool isSpect{}, isReadied{};

    PlayerInfo() = default;

    PlayerInfo(QString, int, int);

    PlayerInfo(QString, int, int, bool, bool);
};

#endif // PLAYER_INFO_H
