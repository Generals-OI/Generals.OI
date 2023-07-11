#ifndef GLOBAL_MAP_H
#define GLOBAL_MAP_H

#include "point.h"

#include <vector>
#include <iomanip>
#include <algorithm>
#include <iostream>

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
    int land{}, army{};

    bool operator<(Statistics cmp) const;

    bool operator==(Statistics cmp) const;

    bool operator>(Statistics cmp) const;
};

class GlobalMap {
public:
    int width{}, length{};
    int crownCnt{}, teamCnt{}, round{};
    std::vector<std::vector<Cell>> info;
    std::vector<int> teamInfo;
    std::vector<Statistics> statPlayer, statTeam;
    std::vector<int> idLoser;

    explicit GlobalMap(int, int, int, int, const std::vector<int> &, int= 0);

    explicit GlobalMap() = default;

    void init(int, int, int, int, const std::vector<int> &, int= 0);

    std::string export2Str(bool);

    void import(const std::string &);

    void print();
};

#endif //GLOBAL_MAP_H
