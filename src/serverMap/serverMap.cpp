#include "serverMap.h"

void ServerMap::move(const int playerId, const Point start,
                     const int deltaX, const int deltaY,
                     const bool flag50p) {
    Point end(start.x + deltaX, start.y + deltaY);

    if (end.x < 1 || end.x > width || end.y < 1 || end.y > length || abs(deltaX) + abs(deltaY) != 1)
        return;

    const Cell startInfo = info[start.x][start.y], endInfo = info[end.x][end.y];

    if (!playerId || startInfo.belonging != playerId || endInfo.type == CellType::mountain)
        return;

    const int moveNum = flag50p ? startInfo.number / 2 : startInfo.number - 1;

    if (moveNum < 0)
        return;

    info[start.x][start.y].number -= moveNum;
    Cell *cell = &info[end.x][end.y];

    if (!endInfo.belonging) {
        *cell = Cell<%abs(moveNum - endInfo.number),
                      moveNum > endInfo.number ? startInfo.belonging : endInfo.belonging,
                      endInfo.type%>;
    } else {
        if (teamInfo[startInfo.belonging] == teamInfo[endInfo.belonging]) {
            *cell = Cell<%moveNum + endInfo.number,
                          endInfo.type == CellType::crown ? endInfo.belonging : startInfo.belonging,
                          endInfo.type%>;
        } else {
            if (moveNum <= endInfo.number) {
                *cell = Cell<%endInfo.number - moveNum, endInfo.belonging, endInfo.type%>;
            } else {
                switch (endInfo.type) {
                    case CellType::land:
                    case CellType::castle: {
                        *cell = Cell<%moveNum - endInfo.number, startInfo.belonging, endInfo.type%>;
                        break;
                    }
                    case CellType::crown: {
                        *cell = Cell<%moveNum - endInfo.number, startInfo.belonging, CellType::castle%>;
                        for (int i = 1; i <= width; i++)
                            for (int j = 1; j <= length; j++)
                                if (info[i][j].belonging == endInfo.belonging)
                                    info[i][j] = Cell<%(info[i][j].number + 1) / 2, startInfo.belonging,
                                                            info[i][j].type%>;
                        break;
                    }
                    default:;
                }
            }
        }
    }
}

void ServerMap::addRound() {
    if (round % 2 == 0)
        for (int i = 1; i <= width; i++)
            for (int j = 1; j <= length; j++)
                if ((info[i][j].type == CellType::crown || info[i][j].type == CellType::castle) &&
                    info[i][j].belonging)
                    info[i][j].number++;
    if (round % 50 == 0)
        for (int i = 1; i <= width; i++)
            for (int j = 1; j <= length; j++)
                if (info[i][j].belonging)
                    info[i][j].number++;

    std::vector<bool>lost(crownCnt+1);
    calcStat();
    for(int i=1;i<=width;i++) {
        for(int j=1;j<=length;j++) {

        }
    }

    round++;
}
