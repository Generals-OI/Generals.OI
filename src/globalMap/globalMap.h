#ifndef GLOBAL_MAP_H
#define GLOBAL_MAP_H

#include "point.h"

#include <vector>
#include <iomanip>
#include <algorithm>
#include <iostream>

const int maxPlayerNum = 16;

enum CellType {
    land, crown, castle, mountain
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
    int crownCnt{}, teamCnt{}, round{};
    std::vector<std::vector<Cell>> info;
    std::vector<int> teamInfo;
    std::vector<std::pair<Statistics, std::vector<Statistics>>> stat;

    explicit GlobalMap(int, int, int, int, const std::vector<int> &);

    GlobalMap() = default;

    void import(const std::string &);

    bool gameOver();

    void print();
};

#endif // GLOBAL_MAP_H
