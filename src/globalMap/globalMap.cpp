#include "globalMap.h"

Cell::Cell(int number, int belonging, CellType type)
        : number(number), belonging(belonging), type(type) {}

GlobalMap::GlobalMap(int width, int length, int teamCnt, int crownCnt, const std::vector<int> &teamInfo, int round) {
    init(width, length, teamCnt, crownCnt, teamInfo, round);
}

void
GlobalMap::init(int _width, int _length, int _teamCnt, int _crownCnt, const std::vector<int> &_teamInfo, int _round) {
    width = _width;
    length = _length;
    teamCnt = _teamCnt;
    crownCnt = _crownCnt;
    info = std::vector<std::vector<Cell>>(width + 1, std::vector<Cell>(length + 1));
    round = _round;
    teamInfo = _teamInfo;
    statPlayer = std::vector<Statistics>(crownCnt + 1);
    statTeam = std::vector<Statistics>(teamCnt + 1);
}

std::string GlobalMap::export2Str(bool fetchFullInfo) {
    using std::string;

    auto toString = [](int num) -> string {
        string res;
        for (int i = abs(num); i; i /= 10)
            res += char(i % 10 + '0');
        if (!num)
            res = "0";
        else {
            if (num < 0)
                res += '-';
            std::reverse(res.begin(), res.end());
        }
        return res;
    };

    string result = fetchFullInfo ? "1_" : "0_";
    if (fetchFullInfo) {
        result.append(toString(width) + "_" + toString(length) + "_"
                      + toString(teamCnt) + "_" + toString(crownCnt) + "_");
        for (int i = 1; i <= crownCnt; i++)
            result.append(toString(teamInfo[i]) + "_");
    }

    result.append(toString(round) + "_");
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++) {
            switch (info[i][j].type) {
                case CellType::land:
                    result.append("0_");
                    break;
                case CellType::crown:
                    result.append("1_");
                    break;
                case CellType::castle:
                    result.append("2_");
                    break;
                case CellType::mountain:
                    result.append("3_");
                    break;
            }
            result.append(toString(info[i][j].belonging) + "_" + toString(info[i][j].number) + "_");
        }

    return result;
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

    if (numbers[0]) {
        std::vector<int> _teamInfo(1, 0);
        for (int i = 5; i <= numbers[4] + 4; i++)
            _teamInfo.push_back(numbers[i]);
        init(numbers[1], numbers[2], numbers[3], numbers[4], _teamInfo);
    }

    if (numbers[0] && numbers.size() != numbers[4] + 6 + width * length * 3 ||
        !numbers[0] && numbers.size() != 2 + width * length * 3) {
        std::cout << "In function GlobalMap::import: Input is invalid" << std::endl;
        return;
    }

    const int posStart = numbers[0] ? numbers[4] + 5 : 1;
    round = numbers[posStart];
    for (int i = posStart + 1; i < numbers.size(); i++) {
        const Point point(((i - posStart + 2) / 3 + length - 1) / length,
                          (i - posStart + 2)
                          / 3 % length == 0 ? length : (i - posStart + 2) / 3 % length);
        Cell &cell = info[point.x][point.y];
        if ((i - posStart) % 3 == 1) {
            switch (numbers[i]) {
                case 0:
                    cell.type = CellType::land;
                    break;
                case 1:
                    cell.type = CellType::crown;
                    break;
                case 2:
                    cell.type = CellType::castle;
                    break;
                case 3:
                    cell.type = CellType::mountain;
                    break;
                default:
                    std::cout << "In function GlobalMap::import: Wrong CellType in input string" << std::endl;
            }
        } else if ((i - posStart) % 3 == 2)
            cell.belonging = numbers[i];
        else
            cell.number = numbers[i];
    }

    statPlayer = std::vector<Statistics>(crownCnt + 1);
    statTeam = std::vector<Statistics>(teamCnt + 1);
    idLoser.clear();
    std::vector<bool> alive(crownCnt + 1, false);

    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++)
            if (info[i][j].belonging) {
                int bel = info[i][j].belonging;
                statPlayer[bel].land++;
                statTeam[teamInfo[bel]].land++;
                statPlayer[bel].army += info[i][j].number;
                statTeam[teamInfo[bel]].army += info[i][j].number;
                if (info[i][j].type == CellType::crown)
                    alive[info[i][j].belonging] = true;
            }
    for (int i = 1; i <= crownCnt; i++)
        if (!alive[i])
            idLoser.push_back(i);
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

bool Statistics::operator>(Statistics cmp) const {
    return army != cmp.army ? army > cmp.army : land > cmp.land;
}

bool Statistics::operator==(Statistics cmp) const {
    return army == cmp.army && land == cmp.land;
}

bool Statistics::operator<(Statistics cmp) const {
    return army != cmp.army ? army < cmp.army : land < cmp.land;
}
