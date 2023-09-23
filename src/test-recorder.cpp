#include "recorder.h"
#include "serverMap.h"
#include <iostream>
#include <QDebug>
#include <QFile>
#include <windows.h>

int main() {
    Recorder r1, r2;
    QVector<QPair<QString, int>> players{{"First",  1},
                                         {"Second", 2}};
    r1.init(players);

    r1.addRecord(1, 10, 8, -1, 0, false);
    r1.addRecord(2, 15, 7, 1, 0, true);
    r1.addRecord(-1, 0, 0, 0, 0, false);

    r1.addRecord(1, 10, 11, 1, 0, true);
    r1.addRecord(2, 15, 43, 0, -1, true);
    r1.addRecord(-1, 0, 0, 0, 0, false);

    r1.addRecord(1, 6, 8, -1, 0, false);
    r1.addRecord(2, 8, 9, 0, 1, false);
    r1.addRecord(-1, 0, 0, 0, 0, false);

    r1.addRecord(1, 12, 15, 1, 0, true);
    r1.addRecord(2, 17, 19, 1, 0, true);
    r1.addRecord(-1, 0, 0, 0, 0, false);

    auto ba = r1.exportRecords();
    int cnt = 0;
    for (auto i: ba)
        qDebug() << (cnt++) << (unsigned) (i) << i;
    r2.importRecord(ba);

    for (const auto &player: r2.players)
        qDebug() << player.first << player.second;
    for (const auto &round: r2.moves) {
        qDebug() << "===";
        for (const auto &move: round)
            qDebug() << move.idPlayer << move.startX << move.startY << direction4[move.direction][0]
                     << direction4[move.direction][1] << move.flag50p;
    }

    /*QFile file("D:/replay.rp");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray ba = file.readAll();
        file.close();
        ServerMap map;
        map.loadByteArray(ba);
        map.print();
        puts("===\n===");
        if (r.importRecord(ba)) {
            for (auto i:r.players) {
                std::cout << i.first.toStdString() << ' ' << i.second << std::endl;
            }
            for (auto i:r.moves) {
                for (auto j:i) {
                    std::cout << j.idPlayer << ' ' << j.startX << ' ' << j.startY << ' ' << j.direction << ' '
                              << j.flag50p << std::endl;
                }
            }
        }
    }
*/

}
