#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include <random>

#include "serverMap.h"

class RandomMapGenerator {
public:
    static ServerMap randomMap(int, int, const std::vector<int> &, int);

private:
    static constexpr int infinity = USHRT_MAX;
    static constexpr int direction8[8][2] = {{1,  1},
                                             {1,  0},
                                             {1,  -1},
                                             {0,  1},
                                             {0,  -1},
                                             {-1, 1},
                                             {-1, 0},
                                             {-1, -1}};

    static std::mt19937 rnd;
    static ServerMap servMap;
    static std::vector<std::vector<int>> teamMbr;
    static std::vector<Point> posList, pntGeneral;
    static std::vector<int> teamList;

    RandomMapGenerator() = default;

    static int randInt(int, int);

    /* Manhattan distance for **class `Point`**
     * Distinguish this with mDistance: mDistance is for `PointLf`, which can be implicitly converted from `Point`
     */
    static int mDistanceI(Point, Point);

    static bool valid(Point);

    static void init(int, int, const std::vector<int> &, int);

    static void setGeneral(int, int, int);

    static void setObstacle(int, int, int);

    static void setCity(int, int, int);
};

#endif // MAP_GENERATOR_H
