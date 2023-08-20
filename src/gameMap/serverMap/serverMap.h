#ifndef SERVER_MAP_H
#define SERVER_MAP_H

#include "globalMap.h"
#include "gameInformation.h"

class ServerMap : public GlobalMap {
private:
    std::vector<int> roundLose;
public:
    explicit ServerMap(GlobalMap &&);

    ServerMap() = default;

    ServerMap(ServerMap &&) = default;

    std::string exportMap(bool);

    // Moves army after checking validity
    bool move(int, Point, int, int, bool, int);

    // Calculates value in stat
    void calcStat();

    // Adds game round
    void addRound();

    // Make player with specified ID surrender
    void surrender(int);
};

#endif // SERVER_MAP_H
