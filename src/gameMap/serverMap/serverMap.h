#ifndef SERVER_MAP_H
#define SERVER_MAP_H

#include <utility>

#include "clientMap.h"
#include "gameInformation.h"

class ServerMap : public ClientMap {
private:
    std::vector<int> roundLose;

    std::vector<int> loseInfo;

    std::vector<std::vector<bool>> flagDiff;
public:
    explicit ServerMap(ClientMap &&);

    ServerMap() = default;

    ServerMap(ServerMap &&) = default;

    ServerMap(const ServerMap &) = default;

    ServerMap &operator=(const ServerMap &) = default;

    ServerMap &operator=(ServerMap &&) = default;

    QVector<qint32> toVectorSM();

    QVector<qint32> exportDiff();

    void loadByteArray(QByteArray &);

    QByteArray toByteArray();

    // Moves army after checking validity
    bool move(int, Point, int, int, bool, int);

    // Calculates value in stat
    void calcStat();

    // Adds game round, returns id of newly-lost players and their reason of losing
    std::vector<std::pair<int, int>> addRound();

    // Make player with specified ID surrender
    void surrender(int);
};

#endif // SERVER_MAP_H
