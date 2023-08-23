#ifndef BASIC_MAP_H
#define BASIC_MAP_H

#include <vector>

#include <QVariantList>

enum CellType {
    land, general, city, mountain, swamp
};

struct Cell {
    int number;
    int belonging;
    CellType type;

    explicit Cell(int = 0, int = 0, CellType = land);
};

class BasicMap {
public:
    int width{}, length{};
    std::vector<std::vector<Cell>> map;

    BasicMap() = default;

    BasicMap(int, int);

    BasicMap(const BasicMap &) = default;

    BasicMap(BasicMap &&) = default;

    BasicMap &operator=(BasicMap &&) = default;

    BasicMap &operator=(const BasicMap &) = default;

    QVector<qint32> exportBM();

    void importBM(const QVector<qint32> &);
};

QVariantList toVariantList(const QVector<qint32> &);

QVector<qint32> toVectorInt(const QVariantList &);

#endif // BASIC_MAP_H
