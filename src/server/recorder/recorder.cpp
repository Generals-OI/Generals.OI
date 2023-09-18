#include "recorder.h"
#include <QDebug>

bool Recorder::addRecord(int id, int x, int y, int dx, int dy, bool flag) {
    if (!isAddName) {
        return false;
    }
    bool b[19] = {};
    if (id == -1) { // 回合分隔符
        std::fill(b, b + 19, true);
    } else {
        // 读入x坐标
        for (int i = 5, _x = x - 1; i >= 0; i--) {
            if (_x & 1) {
                b[i] = true;
            } else {
                b[i] = false;
            }
            _x = _x >> 1;
        }
        // 读入y坐标
        for (int i = 11, _y = y - 1; i >= 6; i--) {
            if (_y & 1) {
                b[i] = true;
            } else {
                b[i] = false;
            }
            _y = _y >> 1;
        }
        // 读入id
        for (int i = 15, _id = id - 1; i >= 12; i--) {
            if (_id & 1) {
                b[i] = true;
            } else {
                b[i] = false;
            }
            _id = _id >> 1;
        }
        // 读入方向，上下左右分别对应0123
        if (dx == -1) {
            b[16] = b[17] = false;
        } else if (dx == 1) {
            b[16] = false, b[17] = true;
        } else if (dy == -1) {
            b[16] = true, b[17] = false;
        } else if (dy == 1) {
            b[16] = b[17] = true;
        }
        b[18] = flag;
    }
    // 读入是否50%
    for (int i = 0; i < 19; i++) {
        if (b[i]) {
            c = c + (1 << (7 - pos));
        }
        if (++pos >= 8) {
            pos = 0;
            data.append(c);
            c = '\0';
        }
    }
    return true;
}

QByteArray Recorder::exportRecords() {
    if (pos > 0) { // 若有剩余，补全一位
        data.append(c);
        pos = 0;
        c = '\0';
    }
    return data;
}

bool Recorder::importRecord(QByteArray &record) {
    data.clear();
    data = record;
    return true;
}

bool Recorder::getNextRecord(int &id, int &x, int &y, int &dx, int &dy, bool &flag) {
    bool b[19] = {}, isEnd = false;

    for (int i = 0; i < 19; i++) {
        if (pos == 0) {
            if (++p >= data.size()) {
                isEnd = true;
                c = '\0';
                data.clear();
                p = 0;
                break;
            }
            c = data.at(p);
            //qDebug()<<'!'<<int(c);
        }
        if (c & (1 << (7 - pos))) {
            b[i] = true;
        } else {
            b[i] = false;
        }
        if (++pos > 7) {
            pos = 0;
        }
    }
    bool isSep = true; // 回合分隔符标志
    for (int i = 0; i < 19; i++) {
        if (!b[i]) {
            isSep = false;
        }
        //qDebug()<<i<<b[i];
    }
    if (isSep) {
        id = -1;
    } else {
        for (int i = 5, j = 1; i >= 0; i--) {
            if (b[i]) {
                x = x + j;
            }
            j = j * 2;
        }
        x++;
        for (int i = 11, j = 1; i >= 6; i--) {
            if (b[i]) {
                y = y + j;
            }
            j = j * 2;
        }
        y++;
        for (int i = 15, j = 1; i >= 12; i--) {
            if (b[i]) {
                id = id + j;
            }
            j = j * 2;
        }
        id++;
        if (!b[16] && !b[17]) {
            dx = -1, dy = 0;
        } else if (!b[16] && b[17]) {
            dx = 1, dy = 0;
        } else if (b[16] && !b[17]) {
            dx = 0, dy = -1;
        } else if (b[16] && b[17]) {
            dx = 0, dy = 1;
        }
        flag = b[18];
    }
    return isEnd;
}

void Recorder::addName(QVector<QString> name) {
    n = (int) name.size();
    data.append(char(n * 5 + 1));
    for (int i = 0; i < n; i++) {
        data.append(name[i].toLocal8Bit());
        data.append('\0');
    }
    isAddName = true;
}

void Recorder::getName(QVector<QString> &name) {
    name.clear();
    n = (data.at(0) - 1) / 5;
    p = 1;
    name.push_back("");
    for (int i = 1; i <= n; p++) {
        if (data.at(p) == '\0') {
            if (i < n) {
                name.push_back("");
            }
            i++;
        } else {
            name[name.size() - 1] += data.at(p);
        }
    }
    p--;
    isAddName = false;
}

