#include "clientMap.h"
#include <QDebug>

extern const int maxRound;

ClientMap::ClientMap(int width, int length, int cntPlayer, int cntTeam, const std::vector<int> &idTeam, int idSelf)
        : BasicMap(width, length), cntPlayer(cntPlayer), cntTeam(cntTeam), idTeam(idTeam), idSelf(idSelf) {
    for (int i = 1; i <= cntPlayer; i++)
        if (idTeam[idSelf - 1] == idTeam[i - 1])
            idTeammates.push_back(i);
}

void ClientMap::calcStat(const std::vector<int> &roundLose) {
    using std::vector;
    using std::pair;
    typedef pair<Statistics, vector<Statistics>> Data;

    if (roundLose[idSelf - 1] != maxRound)
        alive = false;

    vector<Statistics> statPlayer(cntPlayer);

    teamAlive = false;
    for (int i = 0; i < cntPlayer; i++) {
        statPlayer[i].id = i + 1;
        statPlayer[i].roundLose = roundLose[i];

        if (roundLose[i] == maxRound && idTeam[i] == idTeam[idSelf])
            teamAlive = true;
    }

    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            const Cell c = map[i][j];
            if (c.belonging) {
                statPlayer[c.belonging - 1].army += c.number;
                statPlayer[c.belonging - 1].land++;
            }
        }

    stat = vector<Data>(cntTeam);
    for (int i = 0; i < cntPlayer; i++) {
        Data &d = stat[idTeam[i] - 1];
        d.first.army += statPlayer[i].army;
        d.first.land += statPlayer[i].land;
        d.second.push_back(statPlayer[i]);
    }
    for (int i = 0; i < cntTeam; i++) {
        stat[i].first.id = i + 1;
        std::sort(stat[i].second.begin(), stat[i].second.end(), std::greater<Statistics>());
        stat[i].first.roundLose = stat[i].second[0].roundLose;
    }
    std::sort(stat.begin(), stat.end(), [](Data &x, Data &y) {
        return x.first > y.first;
    });
}

void ClientMap::importCM(const QVector<qint32> &vecMap) {
    int p = 2 + vecMap[0] * vecMap[1] * 3;
    importBM(vecMap.mid(0, p));
    auto it = vecMap.begin() + p;
    cntPlayer = *it++;
    cntTeam = *it++;
    idTeam.clear();
    for (int i = 1; i <= cntPlayer; i++)
        idTeam.push_back(*it++);

    alive = teamAlive = true;
    round = 0;

    calcStat(std::vector<int>(cntPlayer, maxRound));
}

void ClientMap::loadDiff(const QVector<qint32> &diff) {
    auto it = diff.begin();
    std::vector<int> roundLose;
    for (int i = 1; i <= cntPlayer; i++) {
        roundLose.push_back(*it++);

    }
    while (it != diff.end()) {
        map[*it][*(it + 1)] = Cell(*(it + 2), *(it + 3), CellType(*(it + 4)));
        it += 5;
    }
    calcStat(roundLose);
    round++;
}

bool ClientMap::isAlive() const {
    return alive;
}

bool ClientMap::isTeamAlive() const {
    return teamAlive;
}

void ClientMap::print() {
    using std::cout;
    using std::endl;
    using std::setw;

    for (int i = 1; i <= width; i++) {
        for (int j = 1; j <= length; j++) {
            switch (map[i][j].type) {
                case CellType::general:
                    cout << setw(3) << map[i][j].belonging << "W";
                    break;
                case CellType::land:
                    cout << setw(3) << map[i][j].number << ".";
                    break;
                case CellType::city:
                    cout << setw(3) << map[i][j].number << "^";
                    break;
                case CellType::mountain:
                    cout << setw(3) << map[i][j].number << "M";
                    break;
                case CellType::swamp:
                    cout << setw(3) << map[i][j].number << "_";
            }
        }
        cout << endl;
    }
}

bool ClientMap::gameOver() {
    int cnt = 0;
    for (const auto &p: stat)
        if (p.first.roundLose == maxRound)
            cnt++;
    return cnt == 1;
}

bool Statistics::operator>(Statistics cmp) const {
    if (roundLose == cmp.roundLose && land == cmp.land && army == cmp.army)
        return id < cmp.id;
    if (roundLose == cmp.roundLose)
        return army != cmp.army ? army > cmp.army : land > cmp.land;
    return roundLose > cmp.roundLose;
}
