#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <algorithm>
#include <QVector>

class Recorder {
public:
    /// add name
    void addName(QVector<QString> name);

    /// get name
    void getName(QVector<QString> &name);

    ///添加一行数据
    bool addRecord(int id, int x, int y, int dx, int dy, bool flag);

    ///加密
    QByteArray exportRecords();

    ///解密，并返回是否成功
    bool importRecord(QByteArray &record);

    ///读取下一行数据（需先解密）
    bool getNextRecord(int &id, int &x, int &y, int &dx, int &dy, bool &flag);

private:
    QByteArray data{};
    int pos{}, p = {};
    char c{};
    int n{};
    bool isAddName{};
};


#endif // RECORDER_H
