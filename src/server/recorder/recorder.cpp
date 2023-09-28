#include "recorder.h"

bool Recorder::addRecord(int id, int x, int y, int dx, int dy, bool flag) {
    bool b[messageSize] = {};
    if (id == -1) { // 回合分隔符
        std::fill(b, b + messageSize, true);
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
        b[18] = flag;// 读入是否50%
    }
    for (int i = 0; i < messageSize; i++) {
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
    players.clear();
    moves.clear();
    auto it = data.begin();
    gameMode = 0;
    for (int i = 3; i >= 0; i--) {
        gameMode += *it << (i * 8);
        it++;
    }
    n = *it;
    it++;
    for (int i = 1; i <= n; i++) {
        int idTeam = *it;
        it++;

        QByteArray name;
        while (*it != '\0') {
            name.append(*it);
            it++;
        }

        players.append({QString::fromLocal8Bit(name), idTeam});
        it++;
    }

    QVector<RecordInfo> roundMoves;
    RecordInfo move;

    bool b[19] = {};
    while (true) {
        for (int i = 0; i < messageSize; i++) {
            if (pos == 0) {
                if (it == data.end())
                    goto end;
                c = *it;
                it++;
            }
            b[i] = (c & (1 << (7 - pos))) > 0;
            if (++pos > 7) {
                pos = 0;
            }
        }

        bool isMove = false;
        for (int i = 0; i < 19; i++) {
            if (!b[i]) {
                isMove = true;
                break;
            }
        }
        if (!isMove) {
            moves.append(roundMoves);
            roundMoves.clear();
        } else {
            move.startX = move.startY = move.direction = move.idPlayer = 0;
            for (int i = 5, j = 1; i >= 0; i--) {
                if (b[i]) {
                    move.startX = move.startX + j;
                }
                j = j * 2;
            }
            move.startX++;
            for (int i = 11, j = 1; i >= 6; i--) {
                if (b[i]) {
                    move.startY = move.startY + j;
                }
                j = j * 2;
            }
            move.startY++;
            for (int i = 15, j = 1; i >= 12; i--) {
                if (b[i]) {
                    move.idPlayer = move.idPlayer + j;
                }
                j = j * 2;
            }
            move.idPlayer++;
            if (!b[16] && !b[17]) {
                move.direction = 0;
            } else if (!b[16] && b[17]) {
                move.direction = 2;
            } else if (b[16] && !b[17]) {
                move.direction = 1;
            } else if (b[16] && b[17]) {
                move.direction = 3;
            } else continue;
            move.flag50p = b[18];
            roundMoves.append(move);
        }
    }

    end:
    if (!roundMoves.empty())
        moves.append(roundMoves);
    return true;
}

void Recorder::init(QVector<QPair<QString, int>> playerInfo, int mode) {
    for (int i = 0; i <= 3; i++) {
        data.append(char(mode >> 24));
        mode <<= 8;
    }
    n = (int) playerInfo.size();
    data.append(char(n));
    for (int i = 0; i < n; i++) {
        data.append(char(playerInfo[i].second)).append(playerInfo[i].first.toLocal8Bit()).append('\0');
    }
}

void Recorder::surrender(int id) {
    bool b[messageSize] = {};
    for (int i = 0; i < 12; i++) {
        b[i] = true;
    }
    for (int i = 15, _id = id - 1; i >= 12; i--) {
        if (_id & 1) {
            b[i] = true;
        } else {
            b[i] = false;
        }
        _id = _id >> 1;
    }
    for (int i = 0; i < messageSize; i++) {
        if (b[i]) {
            c = c + (1 << (7 - pos));
        }
        if (++pos >= 8) {
            pos = 0;
            data.append(c);
            c = '\0';
        }
    }
}

bool Recorder::isSurrender(RecordInfo move) {
    return move.startX >= 64;
}
