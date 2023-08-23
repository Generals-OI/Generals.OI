#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <algorithm>

class Recorder {
public:
    ///添加一行数据
    void addRecord(int id, int x, int y, int dx, int dy, bool flag);

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
};


#endif // RECORDER_H
