#ifndef CLIENT_MAP_H
#define CLIENT_MAP_H

#include "point.h"
#include "basicMap.h"

#include <iomanip>
#include <algorithm>
#include <iostream>
#include <climits>

const int maxPlayerNum = 16;

struct Statistics {
    int army{}, land{}, id{}, roundLose{INT_MAX};

    bool operator>(Statistics cmp) const;
};

class ClientMap : public BasicMap {
protected:
    void calcStat(const std::vector<int> &);

public:
    int cntPlayer{}, cntTeam{}, round{};
    std::vector<int> idTeam;
    std::vector<std::pair<Statistics, std::vector<Statistics>>> stat;

    ClientMap(int, int, int, int, const std::vector<int> &);

    ClientMap() = default;

    ClientMap(ClientMap &&) = default;

    ClientMap &operator=(const ClientMap &) = default;

    void importCM(const QVector<qint32> &);

    void loadDiff(const QVector<qint32> &);

    bool gameOver();

    void print();
};

#endif // CLIENT_MAP_H
