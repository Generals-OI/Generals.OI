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

QByteArray BasicMap::vectorToByteArray(const QVector<qint32> &vec) {
    QByteArray result;
    char tmp{};
    int p = 3;
    auto addNum = [&](int x) {
        int i = 1;
        while (i <= x)
            i *= 3;
        for (i /= 3; i >= 1; i /= 3) {
            int j = x / i;
            x %= i;
            tmp = tmp | ((j >= 2 ? 1 : 0) << p * 2 + 1) | ((j == 1 ? 1 : 0) << p * 2);
            if (!p) {
                p = 3;
                result.append(tmp);
                tmp = 0;
            } else
                p--;
        }
        tmp = tmp | (1 << p * 2 + 1) | (1 << p * 2);
        if (!p) {
            p = 3;
            result.append(tmp);
            tmp = 0;
        } else
            p--;
    };
    addNum(vec.size());
    for (auto i:vec)
        addNum(i);
    if (tmp)
        result.append(tmp);
    return result;
}

QVector<qint32> BasicMap::byteArrayToVector(QByteArray &ba) {
    auto it = ba.begin();
    quint8 x = *it;
    quint8 p = 128;

    auto nextPos = [&]() -> quint8 {
        if (!p) {
            p = 128;
            it++;
            x = *it;
        }
        int val = ((x & p) ? 2 : 0) + ((x & (p >> 1)) ? 1 : 0);
        p >>= 2;
        return val;
    };
    auto nextNum = [&]() -> int {
        int num = 0;
        quint8 val;
        while ((val = nextPos()) != 3)
            num = num * 3 + val;
        return num;
    };

    int size = nextNum();
    QVector<qint32> result;
    for (int i = 0; i < size; i++)
        result.append(nextNum());
    ba.remove(0, it - ba.begin() + 1);
    return result;
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
