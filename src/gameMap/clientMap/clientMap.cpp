#include "clientMap.h"

ClientMap::ClientMap(int width, int length, int cntTeam, int cntGnl, const std::vector<int> &teamInfo) {
    init(width, length, cntTeam, cntGnl, teamInfo);
}

void ClientMap::calcStat(const std::vector<int> &roundLose) {
    using std::vector;
    using std::pair;
    typedef pair<Statistics, vector<Statistics>> Data;

    vector<Statistics> statPlayer(cntPlayer);

    for (int i = 0; i < cntPlayer; i++) {
        statPlayer[i].id = i + 1;
        statPlayer[i].roundLose = roundLose[i];
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

void ClientMap::init(int _width, int _length, int _cntTeam, int _cntGnl, const std::vector<int> &_teamInfo) {
    using std::vector;

    width = _width;
    length = _length;
    cntTeam = _cntTeam;
    cntPlayer = _cntGnl;
    map = vector<vector<Cell>>(width + 1, vector<Cell>(length + 1));
    round = 0;
    idTeam = _teamInfo;
}

void ClientMap::import(const std::string &strMap) {
    using std::string;

    std::vector<int> numbers;
    for (int num = 0, i = 0; i < strMap.size(); i++)
        if (strMap[i] == '_') {
            numbers.push_back(num);
            num = 0;
        } else
            num = num * 10 + strMap[i] - '0';

    if (numbers[0] && numbers.size() != 2 * numbers[4] + 6 + numbers[1] * numbers[2] * 3 ||
        !numbers[0] && numbers.size() != 2 + cntPlayer + width * length * 3) {
        std::cout << "In function ClientMap::import: input is invalid" << std::endl;
        return;
    }

    if (numbers[0]) {
        std::vector<int> _idTeam;
        for (int i = 5; i <= numbers[4] + 4; i++)
            _idTeam.push_back(numbers[i]);
        init(numbers[1], numbers[2], numbers[3], numbers[4], _idTeam);
    }

    auto it = numbers[0] ? numbers.begin() + numbers[4] + 5 : numbers.begin() + 1;
    std::vector<int> roundLose(cntPlayer);
    round = *(it++);
    for (int &i: roundLose)
        i = *(it++);
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            Cell &c = map[i][j];
            switch (*it) {
                case 0:
                    c.type = CellType::land;
                    break;
                case 1:
                    c.type = CellType::general;
                    break;
                case 2:
                    c.type = CellType::city;
                    break;
                case 3:
                    c.type = CellType::mountain;
                    break;
                case 4:
                    c.type = CellType::swamp;
                    break;
                default:
                    std::cout << "In function ClientMap::import: invalid CellType" << std::endl;
            }
            c.belonging = *++it;
            c.number = *++it;
            it++;
        }

    calcStat(roundLose);
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
        if (p.first.roundLose == INT_MAX)
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
