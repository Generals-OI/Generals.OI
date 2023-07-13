#include "serverMap.h"

bool
ServerMap::move(const int playerId, const Point pntStart, const int deltaX, const int deltaY, const bool flagHalf) {
    Point pntEnd(pntStart.x + deltaX, pntStart.y + deltaY);

    if (pntEnd.x < 1 || pntEnd.x > width || pntEnd.y < 1 || pntEnd.y > length || abs(deltaX) + abs(deltaY) != 1)
        return false;

    const Cell cStart = info[pntStart.x][pntStart.y], cEnd = info[pntEnd.x][pntEnd.y];

    if (!playerId || cStart.belonging != playerId || cEnd.type == CellType::mountain)
        return false;

    const int num = flagHalf ? cStart.number / 2 : cStart.number - 1;

    if (num < 0)
        return false;

    info[pntStart.x][pntStart.y].number -= num;
    Cell &cell = info[pntEnd.x][pntEnd.y];

    if (!cEnd.belonging) {
        cell = Cell{abs(num - cEnd.number),
                    num > cEnd.number ? cStart.belonging : 0,
                    cEnd.type};
    } else {
        if (teamInfo[cStart.belonging] == teamInfo[cEnd.belonging]) {
            cell = Cell{num + cEnd.number,
                        cEnd.type == CellType::crown ? cEnd.belonging : cStart.belonging,
                        cEnd.type};
        } else {
            if (num <= cEnd.number) {
                cell = Cell{cEnd.number - num, cEnd.belonging, cEnd.type};
            } else {
                switch (cEnd.type) {
                    case CellType::land:
                    case CellType::castle: {
                        cell = Cell{num - cEnd.number, cStart.belonging, cEnd.type};
                        break;
                    }
                    case CellType::crown: {
                        cell = Cell{num - cEnd.number, cStart.belonging, CellType::castle};
                        for (int i = 1; i <= width; i++)
                            for (int j = 1; j <= length; j++)
                                if (info[i][j].belonging == cEnd.belonging)
                                    info[i][j] = Cell{(info[i][j].number + 1) / 2, cStart.belonging,
                                                      info[i][j].type};
                        roundLose[cEnd.belonging - 1] = round;
                        break;
                    }
                    default:;
                }
            }
        }
    }
    return true;
}

void ServerMap::calcStat() {
    GlobalMap::calcStat(roundLose);
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

    GlobalMap::calcStat(roundLose);
    round++;
}


void ServerMap::surrender(int id) {
    roundLose[id - 1] = round;
}

ServerMap::ServerMap(const GlobalMap &globMap) : GlobalMap(globMap) {
    roundLose = std::vector<int>(crownCnt, INT_MAX);
}

std::string ServerMap::export2Str(bool fetchFullInfo) {
    using std::string;
    using std::vector;

    vector<int> numbers;

    numbers.push_back(fetchFullInfo);
    if (fetchFullInfo) {
        numbers.push_back(width);
        numbers.push_back(length);
        numbers.push_back(teamCnt);
        numbers.push_back(crownCnt);
        for (const auto i: teamInfo)
            numbers.push_back(i);
    }

    numbers.push_back(round);
    for (auto i: roundLose)
        numbers.push_back(i);
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            switch (info[i][j].type) {
                case CellType::land:
                    numbers.push_back(0);
                    break;
                case CellType::crown:
                    numbers.push_back(1);
                    break;
                case CellType::castle:
                    numbers.push_back(2);
                    break;
                case CellType::mountain:
                    numbers.push_back(3);
                    break;
            }
            numbers.push_back(info[i][j].belonging);
            numbers.push_back(info[i][j].number);
        }

    string result;
    for (auto i: numbers)
        result.append(std::to_string(i) + '_');
    return result;
}
