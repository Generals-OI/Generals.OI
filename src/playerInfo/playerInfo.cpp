#include "playerInfo.h"

#include <utility>

PlayerInfo::PlayerInfo(QString nickName, int idPlayer, int idTeam)
        : nickName(std::move(nickName)), idPlayer(idPlayer), idTeam(idTeam), isSpect(false), isReadied(false) {}

PlayerInfo::PlayerInfo(QString nickName, int idPlayer, int idTeam, bool isSpect, bool isReadied)
        : nickName(std::move(nickName)), idPlayer(idPlayer), idTeam(idTeam), isSpect(isSpect), isReadied(isReadied) {}