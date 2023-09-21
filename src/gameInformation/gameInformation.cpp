#include "gameInformation.h"

#include <utility>

PlayerInfo::PlayerInfo(QString nickname, int idPlayer, int idTeam)
        : nickname(std::move(nickname)), idPlayer(idPlayer), idTeam(idTeam), isSpec(false), isReadied(false) {}

PlayerInfo::PlayerInfo(QString nickname, int idPlayer, int idTeam, bool isSpect, bool isReadied)
        : nickname(std::move(nickname)), idPlayer(idPlayer), idTeam(idTeam), isSpec(isSpect), isReadied(isReadied) {}