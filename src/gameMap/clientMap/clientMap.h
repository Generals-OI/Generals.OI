#ifndef CLIENT_MAP_H
#define CLIENT_MAP_H

#include "basicMap.h"

#include <iomanip>
#include <algorithm>
#include <iostream>
#include <climits>

struct Statistics {
    int army{}, land{}, id{}, roundLose{INT_MAX};

    bool operator>(Statistics cmp) const;
};

class ClientMap : public BasicMap {
private:
    bool alive{true}, teamAlive{true};

    void calcStat(const std::vector<int> &);

public:
    int cntPlayer{}, cntTeam{}, round{}, idSelf{};
    std::vector<int> idTeammates;
    std::vector<int> idTeam;
    std::vector<std::pair<Statistics, std::vector<Statistics>>> stat;

    ClientMap(int, int, int, int, const std::vector<int> &, int);

    ClientMap() = default;

    ClientMap(const ClientMap &) = default;

    ClientMap(ClientMap &&) = default;

    ClientMap &operator=(const ClientMap &) = default;

    void importCM(const QVector<qint32> &);

    void loadDiff(const QVector<qint32> &);

    bool gameOver();

    bool isAlive() const;

    bool isTeamAlive() const;

    void print();
};

#endif // CLIENT_MAP_H
