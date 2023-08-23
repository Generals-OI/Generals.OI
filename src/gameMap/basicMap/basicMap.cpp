#include "basicMap.h"

Cell::Cell(int number, int belonging, CellType type)
        : number(number), belonging(belonging), type(type) {}

BasicMap::BasicMap(int width, int length) : width(width), length(length),
                                            map(width + 1, std::vector<Cell>(length + 1)) {}

QVector<qint32> BasicMap::exportBM() {
    QVector<qint32> result({width, length});
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            result.push_back(map[i][j].number);
            result.push_back(map[i][j].belonging);
            result.push_back(map[i][j].type);
        }

    return result;
}

void BasicMap::importBM(const QVector<qint32> &vecMap) {
    using std::vector;

    auto it = vecMap.begin();
    width = *it++;
    length = *it++;
    map = vector<vector<Cell>>(width + 1, vector<Cell>(length + 1));
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            map[i][j] = Cell(*it, *(it + 1), CellType(*(it + 2)));
            it += 3;
        }
}

QVariantList toVariantList(const QVector<qint32> &vec) {
    QVariantList result;
    for (auto i: vec)
        result.push_back(i);
    return result;
}

QVector<qint32> toVectorInt(const QVariantList &vl) {
    QVector<qint32> result;
    for (const QVariant &i: vl)
        result.push_back(i.toInt());
    return result;
}
