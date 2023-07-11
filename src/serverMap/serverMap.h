#ifndef SERVER_MAP_H
#define SERVER_MAP_H

#include "globalMap.h"

class ServerMap {
public:
    GlobalMap gMap;

    // Move troops
    void move(int, Point, int, int, bool);

    // Add game round
    void addRound();
};

// Generate random and valid GlobalMap
GlobalMap generate(int, int, const std::vector<int> &);

#endif // SERVER_MAP_H
