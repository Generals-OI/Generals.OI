#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <algorithm>
#include <QVector>

#include "gameInformation.h"

class Recorder {
    struct RecordInfo;

public:
    void init(QVector<QPair<QString, int>> playerInfo, int mode);

    bool addRecord(int id, int x, int y, int dx, int dy, bool flag);

    void surrender(int id);

    QByteArray exportRecords();

    bool importRecord(QByteArray &record);

    static bool isSurrender(RecordInfo move);

    void addMessage(int round, QString message);

private:
    static const int messageSize = 19;
    QByteArray data{};
    int pos{};
    char c{};
    int n{};
    QByteArray chat{};

public:
    QVector<QPair<QString, int>> players;
    QVector<QVector<RecordInfo>> moves;
    QVector<QVector<QString>> chats;
    int gameMode{};
};

struct Recorder::RecordInfo {
    int startX{}, startY{}, direction{}, idPlayer{};
    bool flag50p{};
};

#endif // RECORDER_H
