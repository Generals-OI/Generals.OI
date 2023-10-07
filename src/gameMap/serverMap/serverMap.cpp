#include "serverMap.h"

extern const int maxPlayerNum = 16;
extern const int maxRound = 1000000;

ServerMap::ServerMap(int width, int length, int cntPlayer, int cntTeam, const std::vector<int> &idTeam)
        : BasicMap(width, length), cntPlayer(cntPlayer), cntTeam(cntTeam), idTeam(idTeam),
          roundLose(cntPlayer, maxRound), loseInfo(cntPlayer), flagDiff(width + 1, std::vector<bool>(length + 1)) {}

bool
ServerMap::move(const int idPlayer, const Point pntStart, const int deltaX, const int deltaY, const bool flagHalf,
                const int gameMode) {
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
                    case CellType::city:
                        cell = Cell{num - cEnd.number, cStart.belonging, cEnd.type};
                        break;
                    case CellType::general:
                        loseInfo[cEnd.belonging - 1] = cStart.belonging;
                        for (int i = 1; i <= width; i++)
                            for (int j = 1; j <= length; j++)
                                if (map[i][j].belonging == cEnd.belonging)
                                    flagDiff[i][j] = true;
                        if (gameMode & GameMode::leapfrog) {
                            for (int i = 1; i <= width; i++)
                                for (int j = 1; j <= length; j++) {
                                    if (map[i][j].belonging == cStart.belonging &&
                                        map[i][j].type == CellType::general) {
                                        map[i][j].type = CellType::city;
                                        flagDiff[i][j] = true;
                                    }
                                    if (map[i][j].belonging == cEnd.belonging && Point(i, j) != pntEnd)
                                        map[i][j] = Cell{(map[i][j].number + 1) / 2, cStart.belonging, map[i][j].type};
                                }
                            cell = Cell{num - cEnd.number, cStart.belonging, CellType::general};
                        } else {
                            cell = Cell{num - cEnd.number, cStart.belonging, CellType::city};

                            for (int i = 1; i <= width; i++)
                                for (int j = 1; j <= length; j++)
                                    if (map[i][j].belonging == cEnd.belonging)
                                        map[i][j] = Cell{(map[i][j].number + 1) / 2, cStart.belonging,
                                                         map[i][j].type};
                        }
                        roundLose[cEnd.belonging - 1] = round;
                        break;
                    default:;
                }
            }
        }
    }

    flagDiff[pntStart.x][pntStart.y] = flagDiff[pntEnd.x][pntEnd.y] = true;
    return true;
}

std::vector<std::pair<int, int>> ServerMap::addRound() {
    if (round % 2 == 0)
        for (int i = 1; i <= width; i++)
            for (int j = 1; j <= length; j++) {
                if ((map[i][j].type == CellType::general || map[i][j].type == CellType::city) &&
                    map[i][j].belonging) {
                    map[i][j].number++;
                    flagDiff[i][j] = true;
                } else if (map[i][j].type == CellType::swamp && map[i][j].belonging) {
                    if (map[i][j].number > 1)
                        map[i][j].number--;
                    else {
                        map[i][j].number = 0;
                        map[i][j].belonging = 0;
                    }
                    flagDiff[i][j] = true;
                }
            }
    if (round % 50 == 0 && round)
        for (int i = 1; i <= width; i++)
            for (int j = 1; j <= length; j++)
                if (map[i][j].belonging) {
                    map[i][j].number++;
                    flagDiff[i][j] = true;
                }

    std::vector<std::pair<int, int>> result;
    for (int i = 0; i < cntPlayer; i++)
        if (roundLose[i] == round)
            result.emplace_back(i + 1, loseInfo[i]);

    round++;
    return result;
}


void ServerMap::surrender(int id) {
    roundLose[id - 1] = round;
    loseInfo[id - 1] = id;
}

QVector<qint32> ServerMap::toVectorSM() {
    QVector<qint32> result = exportBM();
    result.push_back(cntPlayer);
    result.push_back(cntTeam);
    for (auto i: idTeam)
        result.push_back(i);
    return result;
}

QVector<qint32> ServerMap::exportDiff() {
    QVector<qint32> result;
    for (auto i: roundLose)
        result.append(i);
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++)
            if (flagDiff[i][j]) {
                result.append(i);
                result.append(j);
                result.append(map[i][j].number);
                result.append(map[i][j].belonging);
                result.append(map[i][j].type);
            }
    flagDiff = std::vector<std::vector<bool>>(width + 1, std::vector<bool>(length + 1));
    return result;
}

void ServerMap::importSM(const QVector<qint32> &vec) {
    BasicMap::importBM(vec.mid(0, vec[0] * vec[1] * 3 + 2));
    auto it = vec.begin() + vec[0] * vec[1] * 3 + 2;
    cntPlayer = *it++;
    cntTeam = *it++;
    idTeam.resize(cntPlayer);
    for (int &i : idTeam)
        i = *it++;
    roundLose.resize(cntPlayer, maxRound);
    loseInfo.resize(cntPlayer, 0);
    flagDiff = std::vector<std::vector<bool>>(width + 1, std::vector<bool>(length + 1, false));
}

void ServerMap::loadByteArray(QByteArray &ba) {
    importSM(byteArrayToVector(ba));
}

QByteArray ServerMap::toByteArray() {
    return vectorToByteArray(toVectorSM());
}

void ServerMap::copyWithDiff(const ServerMap &serMap) {
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            if (map[i][j] != serMap.map[i][j]) {
                map[i][j] = serMap.map[i][j];
                flagDiff[i][j] = true;
            }
        }
    roundLose = serMap.roundLose;
    loseInfo = serMap.loseInfo;
    round = serMap.round;
}

bool ServerMap::gameOver() const {
    bool alive[maxPlayerNum]{};
    int cnt = 0;
    for (int i = 0; i < cntPlayer; i++)
        if (!alive[idTeam[i] - 1] && roundLose[i] == maxRound) {
            cnt++;
            alive[idTeam[i] - 1] = true;
        }
    return cnt == 1;
}
