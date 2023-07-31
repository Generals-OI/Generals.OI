#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include "serverMap.h"

namespace MapGenerator {
    ServerMap randomMap(int, int, const std::vector<int> &);
}

#endif // MAP_GENERATOR_H
