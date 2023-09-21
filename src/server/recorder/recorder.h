#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <algorithm>
#include <QVector>

#include "gameInformation.h"

class Recorder {
public:
    void init(QVector<QPair<QString, int>> playerInfo, int mode);

    bool addRecord(int id, int x, int y, int dx, int dy, bool flag);

    QByteArray exportRecords();

    bool importRecord(QByteArray &record);

private:
    QByteArray data{};
    int pos{};
    char c{};
    int n{};

public:
    QVector<QPair<QString, int>> players;
    struct RecordInfo;
    QVector<QVector<RecordInfo>> moves;
    int gameMode{};
};

struct Recorder::RecordInfo {
    int startX{}, startY{}, direction{}, idPlayer{};
    bool flag50p{};
};

#endif // RECORDER_H
