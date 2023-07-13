#ifndef SERVER_MAP_H
#define SERVER_MAP_H

#include "globalMap.h"

class ServerMap : public GlobalMap {
private:
    std::vector<int> roundLose;
public:
    explicit ServerMap(const GlobalMap &);

    ServerMap() = default;

    std::string export2Str(bool);

    // Moves army after checking validity
    bool move(int, Point, int, int, bool);

    // Calculates value in stat
    void calcStat();

    // Adds game round
    void addRound();

    // Make player with specified ID surrender
    void surrender(int);
};

// Generate random and valid ServerMap
ServerMap generate(int, int, const std::vector<int> &);

#endif // SERVER_MAP_H
