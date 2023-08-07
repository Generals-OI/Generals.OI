#include "serverMap.h"

bool
ServerMap::move(const int idPlayer, const Point pntStart, const int deltaX, const int deltaY, const bool flagHalf) {
    Point pntEnd(pntStart.x + deltaX, pntStart.y + deltaY);

    if (pntEnd.x < 1 || pntEnd.x > width || pntEnd.y < 1 || pntEnd.y > length || abs(deltaX) + abs(deltaY) != 1)
        return false;

    const Cell cStart = map[pntStart.x][pntStart.y], cEnd = map[pntEnd.x][pntEnd.y];

    if (!idPlayer || cStart.belonging != idPlayer || cEnd.type == CellType::mountain)
        return false;

    const int num = flagHalf ? cStart.number / 2 : cStart.number - 1;

    if (num < 0)
        return false;

    map[pntStart.x][pntStart.y].number -= num;
    Cell &cell = map[pntEnd.x][pntEnd.y];

    if (!cEnd.belonging) {
        cell = Cell{abs(num - cEnd.number),
                    num > cEnd.number ? cStart.belonging : 0,
                    cEnd.type};
    } else {
        if (idTeam[cStart.belonging - 1] == idTeam[cEnd.belonging - 1]) {
            cell = Cell{num + cEnd.number,
                        cEnd.type == CellType::general ? cEnd.belonging : cStart.belonging,
                        cEnd.type};
        } else {
            if (num <= cEnd.number) {
                cell = Cell{cEnd.number - num, cEnd.belonging, cEnd.type};
            } else {
                switch (cEnd.type) {
                    case CellType::land:
                    case CellType::swamp:
                    case CellType::city: {
                        cell = Cell{num - cEnd.number, cStart.belonging, cEnd.type};
                        break;
                    }
                    case CellType::general: {
                        cell = Cell{num - cEnd.number, cStart.belonging, CellType::city};
                        for (int i = 1; i <= width; i++)
                            for (int j = 1; j <= length; j++)
                                if (map[i][j].belonging == cEnd.belonging)
                                    map[i][j] = Cell{(map[i][j].number + 1) / 2, cStart.belonging,
                                                     map[i][j].type};
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
            for (int j = 1; j <= length; j++) {
                if ((map[i][j].type == CellType::general || map[i][j].type == CellType::city) &&
                    map[i][j].belonging)
                    map[i][j].number++;
                else if (map[i][j].type == CellType::swamp && map[i][j].belonging) {
                    if (map[i][j].number > 1)
                        map[i][j].number--;
                    else {
                        map[i][j].number = 0;
                        map[i][j].belonging = 0;
                    }
                }
            }
    if (round % 50 == 0)
        for (int i = 1; i <= width; i++)
            for (int j = 1; j <= length; j++)
                if (map[i][j].belonging)
                    map[i][j].number++;

    GlobalMap::calcStat(roundLose);
    round++;
}


void ServerMap::surrender(int id) {
    roundLose[id - 1] = round;
}

ServerMap::ServerMap(GlobalMap &&globMap) : GlobalMap(std::move(globMap)) {
    roundLose = std::vector<int>(cntGnl, INT_MAX);
}

std::string ServerMap::exportMap(bool flagComplete) {
    using std::string;
    using std::vector;

    vector<int> numbers;

    numbers.push_back(flagComplete);
    if (flagComplete) {
        numbers.push_back(width);
        numbers.push_back(length);
        numbers.push_back(cntTeam);
        numbers.push_back(cntGnl);
        for (const auto i: idTeam)
            numbers.push_back(i);
    }

    numbers.push_back(round);
    for (auto i: roundLose)
        numbers.push_back(i);
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            switch (map[i][j].type) {
                case CellType::land:
                    numbers.push_back(0);
                    break;
                case CellType::general:
                    numbers.push_back(1);
                    break;
                case CellType::city:
                    numbers.push_back(2);
                    break;
                case CellType::mountain:
                    numbers.push_back(3);
                    break;
                case CellType::swamp:
                    numbers.push_back(4);
            }
            numbers.push_back(map[i][j].belonging);
            numbers.push_back(map[i][j].number);
        }

    string result;
    for (auto i: numbers)
        result.append(std::to_string(i) + '_');
    return result;
}
