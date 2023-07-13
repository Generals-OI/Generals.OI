#include "globalMap.h"

Cell::Cell(int number, int belonging, CellType type)
        : number(number), belonging(belonging), type(type) {}

GlobalMap::GlobalMap(int width, int length, int teamCnt, int crownCnt, const std::vector<int> &teamInfo) {
    init(width, length, teamCnt, crownCnt, teamInfo);
}

void GlobalMap::calcStat(const std::vector<int> &roundLose) {
    using std::vector;
    using std::pair;
    typedef pair<Statistics, vector<Statistics>> Data;

    vector<Statistics> statPlayer(crownCnt);

    for (int i = 0; i < crownCnt; i++) {
        statPlayer[i].id = i + 1;
        statPlayer[i].roundLose = roundLose[i];
    }

    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            const Cell c = info[i][j];
            if (c.belonging) {
                statPlayer[c.belonging - 1].army += c.number;
                statPlayer[c.belonging - 1].land++;
            }
        }

    stat = vector<Data>(teamCnt);
    for (int i = 0; i < crownCnt; i++) {
        Data &d = stat[teamInfo[i] - 1];
        d.first.army += statPlayer[i].army;
        d.first.land += statPlayer[i].land;
        d.second.push_back(statPlayer[i]);
    }
    for (int i = 0; i < teamCnt; i++) {
        stat[i].first.id = i + 1;
        std::sort(stat[i].second.begin(), stat[i].second.end(), std::greater<Statistics>());
        stat[i].first.roundLose = stat[i].second[0].roundLose;
    }
    std::sort(stat.begin(), stat.end(), [](Data &x, Data &y) {
        return x.first > y.first;
    });
}

void GlobalMap::init(int _width, int _length, int _teamCnt, int _crownCnt,
                     const std::vector<int> &_teamInfo) {
    using std::vector;

    width = _width;
    length = _length;
    teamCnt = _teamCnt;
    crownCnt = _crownCnt;
    info = vector<vector<Cell>>(width + 1, vector<Cell>(length + 1));
    round = 0;
    teamInfo = _teamInfo;
}

void GlobalMap::import(const std::string &strInfo) {
    using std::string;

    std::vector<int> numbers;
    for (int num = 0, i = 0; i < strInfo.size(); i++)
        if (strInfo[i] == '_') {
            numbers.push_back(num);
            num = 0;
        } else
            num = num * 10 + strInfo[i] - '0';

    if (numbers[0] && numbers.size() != 2 * numbers[4] + 6 + width * length * 3 ||
        !numbers[0] && numbers.size() != 2 + crownCnt + width * length * 3) {
        std::cout << "In function GlobalMap::import: Input is invalid" << std::endl;
        return;
    }

    if (numbers[0]) {
        std::vector<int> _teamInfo;
        for (int i = 5; i <= numbers[4] + 4; i++)
            _teamInfo.push_back(numbers[i]);
        init(numbers[1], numbers[2], numbers[3], numbers[4], _teamInfo);
    }

    auto pos = numbers[0] ? numbers.begin() + numbers[4] + 5 : numbers.begin() + 1;
    std::vector<int> roundLose(crownCnt);
    round = *(pos++);
    for (int &i: roundLose)
        i = *(pos++);
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            Cell &c = info[i][j];
            switch (*pos) {
                case 0:
                    c.type = CellType::land;
                    break;
                case 1:
                    c.type = CellType::crown;
                    break;
                case 2:
                    c.type = CellType::castle;
                    break;
                case 3:
                    c.type = CellType::mountain;
                    break;
                default:
                    std::cout << "In function GlobalMap::import: invalid CellType" << std::endl;
            }
            c.belonging = *++pos;
            c.number = *++pos;
            pos++;
        }

    calcStat(roundLose);
}

void GlobalMap::print() {
    using std::cout;
    using std::endl;
    using std::setw;

    for (int i = 1; i <= width; i++) {
        for (int j = 1; j <= length; j++) {
            switch (info[i][j].type) {
                case CellType::crown:
                    cout << setw(3) << info[i][j].belonging << "W";
                    break;
                case CellType::land:
                    cout << setw(3) << info[i][j].number << ".";
                    break;
                case CellType::castle:
                    cout << setw(3) << info[i][j].number << "^";
                    break;
                case CellType::mountain:
                    cout << setw(3) << info[i][j].number << "M";

            }
        }
        cout << endl;
    }
}

bool GlobalMap::gameOver() {
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
