#include "basicMap.h"

Cell::Cell(int number, int belonging, CellType type)
        : number(number), belonging(belonging), type(type) {}

QVector<int> BasicMap::toVector() {
    QVector<int> result({width, length});
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            result.push_back(map[i][j].number);
            result.push_back(map[i][j].belonging);
            result.push_back(map[i][j].type);
        }

    return result;
}

void BasicMap::import(const QVector<int> &vecMap) {
    auto it = vecMap.begin();
    width = *it++;
    length = *it++;
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            map[i][j] = Cell(*it, *(it + 1), CellType(*(it + 2)));
            it += 3;
        }
}