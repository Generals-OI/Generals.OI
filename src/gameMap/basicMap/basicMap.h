#ifndef BASIC_MAP_H
#define BASIC_MAP_H

#include <vector>

#include <QObject>
#include <QVector>
#include <QVariantList>
#include <QDebug>

enum CellType {
    land, general, city, mountain, swamp
};

struct Cell {
    int number;
    int belonging;
    CellType type;

    explicit Cell(int number = 0, int belonging = 0, CellType type = land);

    bool operator!=(Cell c) const {
        return number!=c.number || belonging!=c.belonging || type!=c.type;
    }
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

protected:
    QVector<qint32> exportBM();

    void importBM(const QVector<qint32> &);

    static QByteArray vectorToByteArray(const QVector<qint32> &);

    static QVector<qint32> byteArrayToVector(QByteArray &);
};

QVariantList toVariantList(const QVector<qint32> &);

QVector<qint32> toVectorInt(const QVariantList &);

#endif // BASIC_MAP_H
