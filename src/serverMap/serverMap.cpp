#include "serverMap.h"

void ServerMap::move(const int playerId, const Point start,
                     const int deltaX, const int deltaY,
                     const bool flag50p) {
    Point end(start.x + deltaX, start.y + deltaY);

    if (end.x < 1 || end.x > gMap.width || end.y < 1 || end.y > gMap.length || abs(deltaX) + abs(deltaY) != 1)
        return;

    const Cell startInfo = gMap.info[start.x][start.y], endInfo = gMap.info[end.x][end.y];

    if (!playerId || startInfo.belonging != playerId || endInfo.type == CellType::mountain)
        return;

    const int moveNum = flag50p ? startInfo.number / 2 : startInfo.number - 1;

    if (moveNum < 0)
        return;

    gMap.info[start.x][start.y].number -= moveNum;
    Cell *cell = &gMap.info[end.x][end.y];

    if (!endInfo.belonging) {
        *cell = Cell<%abs(moveNum - endInfo.number),
                      moveNum > endInfo.number ? startInfo.belonging : endInfo.belonging,
                      endInfo.type%>;
    } else {
        if (gMap.teamInfo[startInfo.belonging] == gMap.teamInfo[endInfo.belonging]) {
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
                        for (int i = 1; i <= gMap.width; i++)
                            for (int j = 1; j <= gMap.length; j++)
                                if (gMap.info[i][j].belonging == endInfo.belonging)
                                    gMap.info[i][j] = Cell<%(gMap.info[i][j].number + 1) / 2, startInfo.belonging,
                                                            gMap.info[i][j].type%>;
                        break;
                    }
                    default:;
                }
            }
        }
    }
}

void ServerMap::addRound() {
    if (gMap.round % 2 == 0)
        for (int i = 1; i <= gMap.width; i++)
            for (int j = 1; j <= gMap.length; j++)
                if ((gMap.info[i][j].type == CellType::crown || gMap.info[i][j].type == CellType::castle) &&
                    gMap.info[i][j].belonging)
                    gMap.info[i][j].number++;
    if (gMap.round % 50 == 0)
        for (int i = 1; i <= gMap.width; i++)
            for (int j = 1; j <= gMap.length; j++)
                if (gMap.info[i][j].belonging)
                    gMap.info[i][j].number++;
    gMap.round++;
}
