#ifndef GLOBAL_MAP_H
#define GLOBAL_MAP_H

#include "point.h"

#include <vector>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <climits>

extern const int maxPlayerNum;

enum CellType {
    land, general, city, mountain, swamp
};

struct Cell {
    int number;
    int belonging;
    CellType type;

    explicit Cell(int = 0, int = 0, CellType = land);
};

struct Statistics {
    int army{}, land{}, id{}, roundLose{INT_MAX};

    bool operator>(Statistics cmp) const;
};

class GlobalMap {
protected:
    void init(int, int, int, int, const std::vector<int> &);

    void calcStat(const std::vector<int> &);

public:
    int width{}, length{};
    int cntGnl{}, cntTeam{}, round{};
    std::vector<std::vector<Cell>> map;
    std::vector<int> idTeam;
    std::vector<std::pair<Statistics, std::vector<Statistics>>> stat;

    GlobalMap(int, int, int, int, const std::vector<int> &);

    GlobalMap() = default;

    GlobalMap(GlobalMap &&) = default;

    GlobalMap(const GlobalMap &) = default;

    GlobalMap& operator=(const GlobalMap &) = default;

    void import(const std::string &);

    bool gameOver();

    void print();
};

#endif // GLOBAL_MAP_H
