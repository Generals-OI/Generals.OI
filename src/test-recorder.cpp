#include "recorder.h"
#include <iostream>
#include <QDebug>

int main() {
    Recorder r;
    int id, x, y, dx, dy;
    bool flag;
    id = 1, x = 1, y = 1, dx = 1, dy = 0, flag = 0;
    r.addRecord(id, x, y, dx, dy, flag);
    id = 1, x = 1, y = 1, dx = 0, dy = 1, flag = 1;
    r.addRecord(id, x, y, dx, dy, flag);
    id = 16, x = 32, y = 13, dx = -1, dy = 0, flag = 0;
    r.addRecord(id, x, y, dx, dy, flag);
    id = 3, x = 45, y = 54, dx = 0, dy = 1, flag = 1;
    r.addRecord(id, x, y, dx, dy, flag);
    //qDebug() << r.exportRecords();
    QByteArray _r = r.exportRecords();
    if (r.importRecord(_r)) {
        id = x = y = dx = dy = 0;
        flag = false;
        while (!r.getNextRecord(id, x, y, dx, dy, flag)) {
            qDebug() << id << x << y << dx << dy << flag;
            id = x = y = dx = dy = 0;
            flag = false;
        }
        //qDebug() << id << x << y << dx << dy << flag;
    }
}
