#include "mapGenerator.h"
#include "geometry.h"

#include <queue>
#include <chrono>

#include <QDebug>

const int RandomMapGenerator::direction8[8][2] = {{1,  1},
                                                  {1,  0},
                                                  {1,  -1},
                                                  {0,  1},
                                                  {0,  -1},
                                                  {-1, 1},
                                                  {-1, 0},
                                                  {-1, -1}};
unsigned RandomMapGenerator::seed{};
std::mt19937 RandomMapGenerator::rnd;
ServerMap RandomMapGenerator::servMap;
std::vector<std::vector<int>> RandomMapGenerator::teamMbr;
std::vector<Point>RandomMapGenerator::posList, RandomMapGenerator::pntGeneral;
std::vector<int>RandomMapGenerator::teamList;

int RandomMapGenerator::randInt(int l, int r) {
    std::uniform_int_distribution<> distrib(l, r);
    return distrib(rnd);
}

int RandomMapGenerator::mDistanceI(Point p, Point q) {
    return abs(p.x - q.x) + abs(p.y - q.y);
}

bool RandomMapGenerator::valid(Point p) {
    return p.x > 0 && p.x <= servMap.width && p.y > 0 && p.y <= servMap.length;
}

void RandomMapGenerator::init(int cntPlayer, int cntTeam, const std::vector<int> &idTeam, int gameMode) {
    using std::mt19937;
    using std::uniform_int_distribution;
    using std::vector;

    if (cntPlayer < 2 || cntPlayer > maxPlayerNum)
        qDebug() << "[mapGenerator.cpp]RandomMapGenerator::init: wrong cntPlayer";

    seed = (unsigned int) std::chrono::steady_clock::now().time_since_epoch().count();
    qDebug() << "[mapGenerator.cpp]RandomMapGenerator::init: Random seed =" << seed;
    rnd = mt19937(seed);

    int lBound, rBound;
    if (cntTeam <= 8) {
        lBound = int(1.2857 * cntTeam + 12.7143);
        rBound = 2 * cntTeam + 15;
    } else {
        lBound = int(3.7738 * cntTeam - 10.2976);
        rBound = int(3.6309 * cntTeam + 1.7381);
    }
    servMap = ServerMap(ClientMap(randInt(lBound, rBound) + cntPlayer - cntTeam,
                                  randInt(lBound, rBound) + cntPlayer - cntTeam, cntPlayer, cntTeam,
                                  idTeam));

    // Initialize some static arrays

    posList.resize(servMap.width * servMap.length);
    for (int i = 1; i <= servMap.width; i++)
        for (int j = 1; j <= servMap.length; j++)
            posList[(i - 1) * servMap.length + j - 1] = Point(i, j);
    shuffle(posList.begin(), posList.end(), rnd);

    for (auto &i: teamMbr)
        i.clear();
    teamMbr.clear();
    teamMbr = vector<vector<int>>(cntTeam, vector<int>(0));
    for (int i = 0; i < cntPlayer; i++)
        teamMbr[idTeam[i] - 1].push_back(i);
    for (auto &i: teamMbr)
        shuffle(i.begin(), i.end(), rnd);

    teamList.resize(cntTeam);
    for (int i = 0; i < cntTeam; i++)
        teamList[i] = i;
    shuffle(teamList.begin(), teamList.end(), rnd);
}

void RandomMapGenerator::setGeneral(int cntPlayer, int cntTeam, int gameMode) {
    using std::min;
    using std::max;
    using std::vector;
    using std::queue;
    using std::pair;

    int direction4[4][2] = {{-1, 0},
                            {1,  0},
                            {0,  -1},
                            {0,  1}};

    vector<Point> pntCenter(cntTeam);
    pntGeneral = vector<Point>(cntPlayer);

    int minDist = randInt(min(servMap.length, 38), max(servMap.width + servMap.length - 2, 57));
    for (auto t: teamList)
        while (true) {
            bool flagBreak = false;
            for (auto cur: posList) {
                bool flagValid = true;
                for (auto k: teamList)
                    if (t == k)
                        break;
                    else if (mDistanceI(pntCenter[k], cur) < minDist) {
                        flagValid = false;
                        break;
                    }
                if (flagValid && rnd() % minDist < 15) {
                    pntCenter[t] = cur;
                    flagBreak = true;
                    break;
                }
            }
            if (flagBreak)
                break;

            if (minDist == 1) {
                qDebug() << "[mapGenerator.cpp]RandomMapGenerator::setGeneral: Unable to set pntCenter";
                break;
            }

            if (minDist > 15)
                minDist = max(minDist - randInt(7, 12), 15);
            else
                minDist--;
        }

    qDebug() << "[mapGenerator.cpp]RandomMapGenerator::setGeneral: pntCenter has been set";

    shuffle(teamList.begin(), teamList.end(), rnd);
    for (auto t: teamList) {
        const int tSize = (int) teamMbr[t].size();
//        if (tSize == 1) {
//            pntGeneral[teamMbr[t][0]] = pntCenter[t];
//            servMap.map[pntCenter[t].x][pntCenter[t].y] = Cell(0, teamMbr[t][0] + 1, CellType::general);
//            continue;
//        }

        int minEnemyDist = randInt(min(18, servMap.length), max(35, servMap.width + servMap.length));
        int maxCtrDist = randInt(3, minDist);
        int minCtrDist = randInt(1, maxCtrDist);
        auto itPlayer = teamMbr[t].begin();

        while (!pntGeneral[teamMbr[t][tSize - 1]].x) {
            queue<pair<Point, int>> q;
            vector<vector<bool>> visited(servMap.width + 1, vector<bool>(servMap.length + 1));
            q.emplace(pntCenter[t], 0);
            visited[pntCenter[t].x][pntCenter[t].y] = true;
            while (!q.empty()) {
                Point cur = q.front().first, nxt;
                int dist = q.front().second;
                q.pop();

                if (dist > maxCtrDist)
                    break;

                if (dist >= minCtrDist) {
                    bool flagValid = true;
                    for (auto it = teamMbr[t].begin(); it != itPlayer; it++) {
                        if (pntGeneral[*it] == cur) {
                            flagValid = false;
                            break;
                        }
                        for (auto d: direction8)
                            if (valid(Point(cur.x + d[0], cur.y + d[1])) &&
                                pntGeneral[*it] == Point(cur.x + d[0], cur.y + d[1])) {
                                flagValid = false;
                                break;
                            }
                    }
                    if (flagValid) {
                        for (auto tEnemy: teamList) {
                            if (tEnemy == t)
                                break;
                            for (auto enemy: teamMbr[tEnemy])
                                if (mDistanceI(pntGeneral[enemy], cur) < minEnemyDist) {
                                    flagValid = false;
                                    break;
                                }
                            if (!flagValid)
                                break;
                        }
                    }
                    if (flagValid && rnd() % minEnemyDist <= 11) {
                        pntGeneral[*itPlayer] = cur;
                        itPlayer++;
                        if (itPlayer == teamMbr[t].end())
                            break;
                        if (tSize >= 10)
                            minCtrDist += 4;
                        else if (tSize >= 6)
                            minCtrDist += 3;
                        else if (tSize >= 3)
                            minCtrDist += 2;
                        else
                            minCtrDist++;
                    }
                }

                for (auto d: direction4) {
                    nxt = Point(cur.x + d[0], cur.y + d[1]);
                    if (valid(nxt) && !visited[nxt.x][nxt.y]) {
                        q.emplace(nxt, dist + 1);
                        visited[nxt.x][nxt.y] = true;
                    }
                }
                std::shuffle(direction4, direction4 + 4, rnd);
            }

            if (minEnemyDist == 7 && maxCtrDist == infinity && !pntGeneral[teamMbr[t][tSize - 1]].x) {
                qDebug() << "[mapGenerator.cpp]RandomMapGenerator::setGeneral: Unable to set general";
                break;
            }

            if (minEnemyDist <= 13) {
                minEnemyDist--;
                minCtrDist = 0;
                maxCtrDist = infinity;
            } else {
                minEnemyDist = max(minEnemyDist - randInt(4, 9), 13);
                minCtrDist = tSize >= 6 ? maxCtrDist : int(maxCtrDist * (0.4 + tSize * 0.1));
                maxCtrDist += randInt(3, 10);
            }
        }

        for (auto i: teamMbr[t])
            servMap.map[pntGeneral[i].x][pntGeneral[i].y] = Cell{0, i + 1, CellType::general};
    }

    qDebug() << "[mapGenerator.cpp]RandomMapGenerator::setGeneral: generals have been set";
}

void RandomMapGenerator::setObstacle(int cntPlayer, int cntTeam, int gameMode) {
    using std::vector;
    using std::queue;
    using std::deque;
    using std::swap;
    using std::max;
    using std::min;

    constexpr int direction4[4][2] = {{0,  1},
                                      {1,  0},
                                      {0,  -1},
                                      {-1, 0}};

    vector<PointLf> pntGeomCtr(cntTeam);
    vector<vector<int>> idTeamChunk(servMap.width + 1, vector<int>(servMap.length + 1));
    vector<vector<Point>> pntChunkBdr(cntTeam); // the border points of each team chunk
    vector<vector<bool>> possibleMtn(servMap.width + 1, vector<bool>(servMap.length + 1, true));

    for (auto p: pntGeneral)
        possibleMtn[p.x][p.y] = false;
    for (auto t: teamList) {
        if (teamMbr[t].size() == 1) {
            Point general = pntGeneral[teamMbr[t][0]];
            pntGeomCtr[t] = general;
            idTeamChunk[general.x][general.y] = t + 1;
            for (auto d: direction8) {
                Point p(general.x + d[0], general.y + d[1]);
                if (valid(p)) {
                    possibleMtn[p.x][p.y] = false;
                    pntChunkBdr[t].push_back(p);
                }
            }
            continue;
        }

        for (auto p: teamMbr[t]) {
            pntGeomCtr[t].x += pntGeneral[p].x;
            pntGeomCtr[t].y += pntGeneral[p].y;
            possibleMtn[pntGeneral[p].x][pntGeneral[p].y] = false;
        }
        pntGeomCtr[t].x /= (int) teamMbr[t].size();
        pntGeomCtr[t].y /= (int) teamMbr[t].size();

        for (int i = 1; i <= servMap.width; i++)
            for (int j = 1; j <= servMap.length; j++) {
                bool flagInChunk = false;
                LineSegment l1(PointLf(i - 0.5, j - 0.5), PointLf(i + 0.5, j - 0.5)),
                        l2(PointLf(i - 0.5, j - 0.5), PointLf(i - 0.5, j + 0.5)),
                        l3(PointLf(i - 0.5, j + 0.5), PointLf(i + 0.5, j + 0.5)),
                        l4(PointLf(i + 0.5, j - 0.5), PointLf(i + 0.5, j + 0.5));
                for (auto p1 = teamMbr[t].begin(); p1 != teamMbr[t].end() && !flagInChunk; p1++)
                    for (auto p2 = p1 + 1; p2 != teamMbr[t].end(); p2++) {
                        LineSegment a(pntGeneral[*p1], pntGeneral[*p2]),
                                b(pntGeneral[*p1], pntGeomCtr[t]),
                                c(pntGeneral[*p2], pntGeomCtr[t]);
                        if (inTriangle(pntGeneral[*p1], pntGeneral[*p2], pntGeomCtr[t], PointLf(i - 0.5, j - 0.5)) ||
                            intersect(l1, a) || intersect(l1, b) || intersect(l1, c) ||
                            intersect(l2, a) || intersect(l2, b) || intersect(l2, c) ||
                            intersect(l3, a) || intersect(l3, b) || intersect(l3, c) ||
                            intersect(l4, a) || intersect(l4, b) || intersect(l4, c)) {
                            idTeamChunk[i][j] = t + 1;
                            flagInChunk = true;
                            break;
                        }
                    }
            }

        for (int i = 1; i <= servMap.width; i++)
            for (int j = 1; j <= servMap.length; j++)
                if (idTeamChunk[i][j] != t + 1)
                    for (auto d: direction8) {
                        Point point(i + d[0], j + d[1]);
                        if (valid(point) && idTeamChunk[point.x][point.y] == t + 1) {
                            possibleMtn[i][j] = false;
                            pntChunkBdr[t].emplace_back(i, j);
                        }
                    }
    }

    qDebug() << "[mapGenerator.cpp]RandomMapGenerator::setObstacle: finished calculating idTeamChunk";

    struct Edge {
        int u{}, v{};
        int value{};

        Edge() = default;

        Edge(int u, int v, int value)
                : u(u), v(v), value(value) {}
    };

    vector<Edge> edges;
    vector<int> parent(cntTeam + 1);

    auto findParent = [&parent](int k) -> int {
        int par = k, temp;
        while (parent[par] != par)
            par = parent[par];
        while (k != par) {
            temp = parent[k];
            parent[k] = par;
            k = temp;
        }
        return par;
    };

    for (int i = 0; i < cntTeam; i++) {
        parent[i + 1] = i + 1;
        for (int j = i + 1; j < cntTeam; j++)
            edges.emplace_back(i, j, mDistance(pntGeomCtr[i], pntGeomCtr[j]));
    }
    sort(edges.begin(), edges.end(), [](Edge a, Edge b) -> bool {
        if (a.value == b.value)
            return a.u == b.u ?
                   a.v < b.v :
                   a.u < b.u;
        return a.value < b.value;
    });
    for (auto e: edges) {
        int parU = findParent(e.u + 1), parV = findParent(e.v + 1);
        if (parU != parV) {
            parent[parU] = parV;
            Point pntStart = pntChunkBdr[e.u][randInt(0, (int) pntChunkBdr[e.u].size() - 1)],
                    pntEnd = pntChunkBdr[e.v][randInt(0, (int) pntChunkBdr[e.v].size() - 1)];
            vector<Point> path;
            Point cur = pntStart;
            const int k = mDistanceI(pntStart, pntEnd) / 8;
            const int uBound = max(1, min(pntStart.x, pntEnd.x) - k);
            const int dBound = min(servMap.width, max(pntStart.x, pntEnd.x) + k);
            const int lBound = max(1, min(pntStart.y, pntEnd.y) - k);
            const int rBound = min(servMap.length, max(pntStart.y, pntEnd.y) + k);
            for (int i = 1; i <= k + 1; i++) {
                Point nxt;
                if (i == k + 1)
                    nxt = pntEnd;
                else
                    nxt = Point(randInt(uBound, dBound), randInt(lBound, rBound));
                while (cur != nxt) {
                    path.push_back(cur);
                    if ((cur.y == nxt.y || randInt(0, 1)) && cur.x != nxt.x)
                        cur.x += (nxt.x - cur.x) / abs(nxt.x - cur.x);
                    else
                        cur.y += (nxt.y - cur.y) / abs(nxt.y - cur.y);
                }
            }
            path.push_back(cur);
            for (auto p: path)
                possibleMtn[p.x][p.y] = false;
        }
    }
    for (int i = 1; i <= servMap.width; i++)
        for (int j = 1; j <= servMap.length; j++)
            possibleMtn[i][j] = possibleMtn[i][j] && rnd() % 7 <= 3;

    qDebug() << "[mapGenerator.cpp]RandomMapGenerator::setObstacle: possibleMtn has been set";

    vector<vector<int>> root(servMap.width + 2, vector<int>(servMap.length + 2));
    auto depth = root;
    vector<vector<bool>> visited(servMap.width + 2, vector<bool>(servMap.length + 2));
    vector<vector<vector<Point>>> ancestor(servMap.width + 2, vector<vector<Point>>(servMap.length + 2));
    queue<Point> q;
    int cntRoot = 0;

    /* Assume that p and q are connected through another mountain, then
     * calculate the length of the cycle containing p and q
     */
    auto cycleLength = [&ancestor, &root, &depth](Point p, Point q) -> int {
        if (root[p.x][p.y] != root[q.x][q.y])
            return infinity;
        if (depth[p.x][p.y] > depth[q.x][q.y])
            swap(p, q);
        const int dP = depth[p.x][p.y], dQ = depth[q.x][q.y];
        for (int i = (int) ancestor[q.x][q.y].size() - 1; i >= 0; i--) {
            Point _q = ancestor[q.x][q.y][i];
            if (depth[_q.x][_q.y] >= dP)
                q = _q;
        }
        if (p == q)
            return dQ - dP + 2;
        for (int i = (int) ancestor[q.x][q.y].size() - 1; i >= 0; i--) {
            Point _p = ancestor[p.x][p.y][i];
            Point _q = ancestor[q.x][q.y][i];
            if (_p != _q) {
                p = _p;
                q = _q;
            }
        }
        return dP + dQ - 2 * depth[p.x][p.y] + 4;
    };

    auto expandMountains = [&]() {
        while (!q.empty()) {
            const Point cur = q.front();
            q.pop();
            for (auto d: direction8) {
                const Point nxt(cur.x + d[0], cur.y + d[1]);
                if (!valid(nxt) || !possibleMtn[nxt.x][nxt.y] || visited[nxt.x][nxt.y])
                    continue;

                visited[nxt.x][nxt.y] = true;
                bool flagExpand = false;
                if (depth[cur.x][cur.y] <= 2) {
                    flagExpand = rnd() % 2 == 1;
                } else {
                    flagExpand = true;
                    for (int i = 0; i < 8 && flagExpand; i++)
                        for (int j = i + 1; j < 8; j++) {
                            Point a(nxt.x + direction8[i][0], nxt.y + direction8[i][1]);
                            Point b(nxt.x + direction8[j][0], nxt.y + direction8[j][1]);
                            if (valid(a) && valid(b) &&
                                servMap.map[a.x][a.y].type == CellType::mountain &&
                                servMap.map[b.x][b.y].type == CellType::mountain) {
                                int dist = cycleLength(a, b);
                                if (!idTeamChunk[nxt.x][nxt.y] && dist > 8 ||
                                    idTeamChunk[nxt.x][nxt.y] && dist > 5) {
                                    flagExpand = false;
                                    break;
                                }
                            }
                        }

                    flagExpand = flagExpand && rnd() % 7 <= 4;
                }

                if (flagExpand) {
                    for (auto i: direction4) {
                        Point p1(nxt.x + i[0], nxt.y + i[1]);
                        if (!valid(p1) || servMap.map[p1.x][p1.y].type != CellType::general)
                            continue;
                        bool surrounded = true;
                        for (auto j: direction4) {
                            Point p2(p1.x + j[0], p1.y + j[1]);
                            if (valid(p2))
                                surrounded = surrounded &&
                                             (p2 == nxt || servMap.map[p2.x][p2.y].type == CellType::mountain);
                        }
                        if (surrounded) {
                            flagExpand = false;
                            break;
                        }
                    }
                }

                if (flagExpand) {
                    q.push(nxt);
                    depth[nxt.x][nxt.y] = depth[cur.x][cur.y] + 1;
                    root[nxt.x][nxt.y] = root[cur.x][cur.y];
                    servMap.map[nxt.x][nxt.y].type = CellType::mountain;
                    int cnt = (int) log2(depth[nxt.x][nxt.y]) + 1;
                    ancestor[nxt.x][nxt.y].resize(cnt);
                    ancestor[nxt.x][nxt.y][0] = cur;
                    for (int i = 1; i < cnt; i++)
                        ancestor[nxt.x][nxt.y][i] = ancestor[ancestor[nxt.x][nxt.y][i - 1].x]
                        [ancestor[nxt.x][nxt.y][i - 1].y][i - 1];
                }
            }
        }
    };

    deque<Point> dqInit;
    for (int i = 1; i <= servMap.width; i++) {
        root[i][0] = ++cntRoot;
        dqInit.emplace_back(i, 0);
        root[i][servMap.length + 1] = ++cntRoot;
        dqInit.emplace_back(i, servMap.length + 1);
    }
    for (int j = 1; j <= servMap.length; j++) {
        root[0][j] = ++cntRoot;
        dqInit.emplace_back(0, j);
        root[servMap.width + 1][j] = ++cntRoot;
        dqInit.emplace_back(servMap.width + 1, j);
    }
    shuffle(dqInit.begin(), dqInit.end(), rnd);
    q = queue<Point>(dqInit);

    expandMountains();

    shuffle(posList.begin(), posList.end(), rnd);
    for (auto p: posList)
        if (p.x >= 4 && p.y >= 4 && p.x <= servMap.width - 3 && p.y <= servMap.length - 3 &&
            possibleMtn[p.x][p.y] && !visited[p.x][p.y]) {
            visited[p.x][p.y] = true;
            bool flagValid = true;
            for (int i = 0; i < 8 && flagValid; i++)
                for (int j = i + 1; j < 8; j++) {
                    Point a(p.x + direction8[i][0], p.y + direction8[i][1]);
                    Point b(p.x + direction8[j][0], p.y + direction8[j][1]);
                    if (valid(a) && valid(b) &&
                        servMap.map[a.x][a.y].type == CellType::mountain &&
                        servMap.map[b.x][b.y].type == CellType::mountain) {
                        int dist = cycleLength(a, b);
                        if (!idTeamChunk[p.x][p.y] && dist > 8 || idTeamChunk[p.x][p.y] && dist > 5) {
                            flagValid = false;
                            break;
                        }
                    }
                }

            if (flagValid) {
                q.emplace(p.x, p.y);
                depth[p.x][p.y] = 1;
                root[p.x][p.y] = ++cntRoot;
                servMap.map[p.x][p.y].type = CellType::mountain;
                ancestor[p.x][p.y].emplace_back(p.x, p.y);

                expandMountains();
            }
        }

    qDebug() << "[mapGenerator.cpp]RandomMapGenerator::setObstacle: obstacles have been placed";
}

void RandomMapGenerator::setCity(int cntPlayer, int cntTeam, int gameMode) {
    int direction4[4][2] = {{0,  1},
                            {1,  0},
                            {0,  -1},
                            {-1, 0}};

    for (int i = 1; i <= servMap.width; i++)
        for (int j = 1; j <= servMap.length; j++)
            if (servMap.map[i][j].type == CellType::mountain && rnd() % 13 <= 1) {
                servMap.map[i][j].type = CellType::city;
                servMap.map[i][j].number = randInt(40, 50);
            }
    if (gameMode & GameMode::cityState) {
        for (auto gnl: pntGeneral) {
            bool flagSuccess = false;
            std::shuffle(direction4, direction4 + 4, rnd);
            for (auto d: direction4) {
                if (!valid(Point(gnl.x + d[0], gnl.y + d[1])))
                    continue;
                Cell &c = servMap.map[gnl.x + d[0]][gnl.y + d[1]];
                if (c.type == CellType::city && !c.belonging || c.type == CellType::mountain) {
                    c = Cell(0, servMap.map[gnl.x][gnl.y].belonging, CellType::city);
                    flagSuccess = true;
                    break;
                }
            }
            if (flagSuccess)
                continue;
            for (auto d: direction4) {
                if (!valid(Point(gnl.x + d[0], gnl.y + d[1])))
                    continue;
                Cell &c = servMap.map[gnl.x + d[0]][gnl.y + d[1]];
                if (c.type != CellType::general) {
                    c = Cell(0, servMap.map[gnl.x][gnl.y].belonging, CellType::city);
                    flagSuccess = true;
                    break;
                }
            }
            if (!flagSuccess)
                qDebug() << "[mapGenerator.cpp]setCity: no valid position to set extra city in City-State mod";
        }
    }
}

ServerMap RandomMapGenerator::randomMap(int cntPlayer, int cntTeam, const std::vector<int> &idTeam, int gameMode) {
    using std::vector;

    init(cntPlayer, cntTeam, idTeam, gameMode);
    setGeneral(cntPlayer, cntTeam, gameMode);
    setObstacle(cntPlayer, cntTeam, gameMode);
    setCity(cntPlayer, cntTeam, gameMode);

    servMap.calcStat();
    return std::move(servMap);
}

unsigned int RandomMapGenerator::lastSeed() {
    return seed;
}
