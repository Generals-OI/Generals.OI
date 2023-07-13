#include "serverMap.h"
#include "geometry.h"

#include <queue>
#include <iostream>
#include <random>

ServerMap generate(int playerCnt, int teamCnt, const std::vector<int> &team) {
    using std::pair;
    using std::make_pair;
    using std::max;
    using std::min;
    using std::sqrt;
    using std::log2;
    using std::swap;
    using std::cout;
    using std::endl;
    using std::deque;
    using std::queue;
    using std::vector;
    using std::mt19937;
    using std::uniform_int_distribution;

    const int direction4[4][2] = {{-1, 0},
                                  {1,  0},
                                  {0,  -1},
                                  {0,  1}};
    const int direction8[8][2] = {{1,  1},
                                  {1,  0},
                                  {1,  -1},
                                  {0,  1},
                                  {0,  -1},
                                  {-1, 1},
                                  {-1, 0},
                                  {-1, -1}};

    if (playerCnt < 2 || playerCnt > maxPlayerNum)
        cout << "In function generate: Wrong Player Cnt" << endl;

    const auto seed = time(nullptr);
    cout << "In function generate: Random seed=" << seed << endl;
    mt19937 rnd(seed);
    auto randInt = [&rnd](int rangeL, int rangeR) -> int {
        uniform_int_distribution<> range(rangeL, rangeR);
        return range(rnd);
    };

    int lBound, rBound;
    if (playerCnt <= 8) {
        lBound = int(1.2857 * playerCnt + 11.7143);
        rBound = 2 * playerCnt + 15;
    } else {
        lBound = int(3.7738 * playerCnt - 10.2976);
        rBound = int(3.6309 * playerCnt + 1.7381);
    }
    ServerMap servMap{GlobalMap(randInt(lBound, rBound), randInt(lBound, rBound), teamCnt, playerCnt, team)};

    auto valid = [servMap](Point p) -> bool {
        return p.x > 0 && p.x <= servMap.width && p.y > 0 && p.y <= servMap.length;
    };

    auto manhattanDistance = [](Point p, Point q) -> int {
        return abs(p.x - q.x) + abs(p.y - q.y);
    };

    // Initialize some arrays

    vector<Point> posList;
    vector<vector<int>> teamMbr(teamCnt);
    vector<int> teamList(teamCnt);
    vector<Point> pntCenter(teamCnt), pntCrown(playerCnt);

    for (int i = 1; i <= servMap.width; i++)
        for (int j = 1; j <= servMap.length; j++)
            posList.emplace_back(i, j);
    shuffle(posList.begin(), posList.end(), rnd);

    for (int i = 0; i < playerCnt; i++)
        teamMbr[team[i] - 1].push_back(i);
    for (int i = 0; i < teamCnt; i++)
        teamList.push_back(i);
    shuffle(teamList.begin(), teamList.end(), rnd);
    for (int i = 0; i < teamCnt; i++)
        shuffle(teamMbr[i].begin(), teamMbr[i].end(), rnd);
    shuffle(posList.begin(), posList.end(), rnd);

    // Set crown position randomly

    int minDist = randInt(min(servMap.length, 38), max(servMap.width + servMap.length - 2, 57));
    for (auto t: teamList)
        while (true) {
            bool flagBreak = false;
            for (auto cur: posList) {
                bool flag = true;
                for (auto k: teamList)
                    if (t == k)
                        break;
                    else if (manhattanDistance(pntCenter[k], cur) < minDist) {
                        flag = false;
                        break;
                    }
                if (flag && rnd() % minDist <= 10) {
                    pntCenter[t] = cur;
                    flagBreak = true;
                    break;
                }
            }
            if (flagBreak)
                break;

            if (minDist == 1) {
                cout << "In function generate: Unable to set _team center" << endl;
                break;
            }

            if (minDist > 11)
                minDist = max(minDist - randInt(10, 13), 11);
            else
                minDist -= 2;
        }

    cout << "In function generate: pntCenter has been set" << endl;

    for (auto t: teamList) {
        int minEnemyDist = randInt(min(18, servMap.length), max(35, servMap.width + servMap.length));
        int maxCenterDist = randInt(3, minDist);
        int minCenterDist = randInt(1, maxCenterDist);
        const int tSize = (int) teamMbr[t].size();
        auto pPlayer = teamMbr[t].begin();

        if (tSize == 1) {
            pntCrown[teamMbr[t][0]] = pntCenter[t];
            servMap.info[pntCenter[t].x][pntCenter[t].y] = Cell(0, teamMbr[t][0] + 1, CellType::crown);
            continue;
        }

        while (!pntCrown[teamMbr[t][tSize - 1]].x) {
            queue<pair<Point, int>> q;
            vector<vector<bool>> visited(servMap.width + 1, vector<bool>(servMap.length + 1));
            q.emplace(pntCenter[t], 0);
            visited[pntCenter[t].x][pntCenter[t].y] = true;
            while (!q.empty()) {
                Point cur = q.front().first, nxt;
                int dist = q.front().second;
                q.pop();

                if (dist > maxCenterDist)
                    break;

                if (dist >= minCenterDist) {
                    bool flag = true;
                    for (auto j = teamMbr[t].begin(); j != pPlayer; j++)
                        for (auto d: direction8)
                            if (valid(Point(cur.x + d[0], cur.y + d[1])) &&
                                pntCrown[*j] == Point(cur.x + d[0], cur.y + d[1])) {
                                flag = false;
                                break;
                            }
                    if (flag) {
                        for (auto enemyTeam: teamList) {
                            if (enemyTeam == t)
                                break;
                            for (auto enemy: teamMbr[enemyTeam])
                                if (manhattanDistance(pntCrown[enemy], cur) < minEnemyDist) {
                                    flag = false;
                                    break;
                                }
                            if (!flag)
                                break;
                        }
                    }
                    if (flag && rnd() % minEnemyDist <= 15) {
                        pntCrown[*pPlayer] = cur;
                        pPlayer++;
                        if (pPlayer == teamMbr[t].end())
                            break;
                        if (tSize >= 10)
                            minCenterDist += 4;
                        else if (tSize >= 6)
                            minCenterDist += 3;
                        else if (tSize >= 3)
                            minCenterDist += 2;
                        else
                            minCenterDist++;
                    }
                }

                for (auto d: direction4) {
                    nxt = Point(cur.x + d[0], cur.y + d[1]);
                    if (valid(nxt) && !visited[nxt.x][nxt.y]) {
                        q.emplace(nxt, dist + 1);
                        visited[nxt.x][nxt.y] = true;
                    }
                }
            }

            if (minEnemyDist == 5 && maxCenterDist == USHRT_MAX && !pntCrown[teamMbr[t][tSize - 1]].x) {
                cout << "In function generate: Unable to set crown pos" << std::endl;
                break;
            }

            if (minEnemyDist <= 15) {
                minEnemyDist--;
                minCenterDist = 0;
                maxCenterDist = USHRT_MAX;
            } else {
                minEnemyDist = max(minEnemyDist - randInt(4, 9), 15);
                minCenterDist = tSize >= 6 ? maxCenterDist : int(maxCenterDist * (0.4 + tSize * 0.1));
                maxCenterDist += randInt(3, 10);
            }
        }

        for (auto i: teamMbr[t])
            servMap.info[pntCrown[i].x][pntCrown[i].y] = Cell{0, i + 1, CellType::crown};
    }

    cout << "In function generate: crowns have been set" << endl;

    // Set mountain positions while ensuring that crowns can reach each other

    vector<RealPoint> pntGeoCenter(teamCnt);
    vector<vector<int>> idxTeamChunk(servMap.width + 1, vector<int>(servMap.length + 1));
    vector<vector<Point>> teamBorder(teamCnt);
    vector<vector<bool>> isCrucialPath(servMap.width + 1, vector<bool>(servMap.length + 1));
    auto mountainToBe = isCrucialPath;

    for (auto p: pntCrown)
        isCrucialPath[p.x][p.y] = true;
    for (auto t: teamList) {
        if (teamMbr[t].size() == 1) {
            Point crown = pntCrown[teamMbr[t][0]];
            pntGeoCenter[t] = crown;
            idxTeamChunk[crown.x][crown.y] = t + 1;
            for (auto d: direction8) {
                Point p(crown.x + d[0], crown.y + d[1]);
                if (valid(p)) {
                    isCrucialPath[p.x][p.y] = true;
                    teamBorder[t].push_back(p);
                }
            }
            continue;
        }

        for (auto p: teamMbr[t]) {
            pntGeoCenter[t].x += pntCrown[p].x;
            pntGeoCenter[t].y += pntCrown[p].y;
            isCrucialPath[pntCrown[p].x][pntCrown[p].y] = true;
        }
        pntGeoCenter[t].x /= (int) teamMbr[t].size();
        pntGeoCenter[t].y /= (int) teamMbr[t].size();

        for (int i = 1; i <= servMap.width; i++)
            for (int j = 1; j <= servMap.length; j++) {
                bool flag = false;
                LineSegment l1(RealPoint(i - 0.5, j - 0.5), RealPoint(i + 0.5, j - 0.5)),
                        l2(RealPoint(i - 0.5, j - 0.5), RealPoint(i - 0.5, j + 0.5)),
                        l3(RealPoint(i - 0.5, j + 0.5), RealPoint(i + 0.5, j + 0.5)),
                        l4(RealPoint(i + 0.5, j - 0.5), RealPoint(i + 0.5, j + 0.5));
                for (auto p1 = teamMbr[t].begin(); p1 != teamMbr[t].end() && !flag; p1++)
                    for (auto p2 = p1 + 1; p2 != teamMbr[t].end(); p2++) {
                        LineSegment a(pntCrown[*p1], pntCrown[*p2]),
                                b(pntCrown[*p1], pntGeoCenter[t]),
                                c(pntCrown[*p2], pntGeoCenter[t]);
                        if (inTriangle(pntCrown[*p1], pntCrown[*p2], pntGeoCenter[t], RealPoint(i - 0.5, j - 0.5)) ||
                            intersect(l1, a) || intersect(l1, b) || intersect(l1, c) ||
                            intersect(l2, a) || intersect(l2, b) || intersect(l2, c) ||
                            intersect(l3, a) || intersect(l3, b) || intersect(l3, c) ||
                            intersect(l4, a) || intersect(l4, b) || intersect(l4, c)) {
                            idxTeamChunk[i][j] = t + 1;
                            flag = true;
                            break;
                        }
                    }
            }

        for (int i = 1; i <= servMap.width; i++)
            for (int j = 1; j <= servMap.length; j++)
                if (idxTeamChunk[i][j] != t + 1)
                    for (auto d: direction8) {
                        Point point(i + d[0], j + d[1]);
                        if (valid(point) && idxTeamChunk[point.x][point.y] == t + 1) {
                            isCrucialPath[i][j] = true;
                            teamBorder[t].emplace_back(i, j);
                        }
                    }
    }

    cout << "In function generate: finished calculating idxTeamChunk" << endl;

    struct Edge {
        int u{}, v{};
        int value{};

        Edge() = default;

        Edge(int u, int v, int value)
                : u(u), v(v), value(value) {}
    };

    vector<Edge> edges;
    vector<int> parent(teamCnt + 1);

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

    auto rndPath = [servMap, manhattanDistance, randInt](const Point pntStart, const Point pntEnd) -> vector<Point> {
        const int k = manhattanDistance(pntStart, pntEnd) / 8;
        const int uBound = max(1, min(pntStart.x, pntEnd.x) - k);
        const int dBound = min(servMap.width, max(pntStart.x, pntEnd.x) + k);
        const int lBound = max(1, min(pntStart.y, pntEnd.y) - k);
        const int rBound = min(servMap.length, max(pntStart.y, pntEnd.y) + k);

        vector <Point> path;
        Point cur = pntStart;

        for (int i = 1; i <= k + 1; i++) {
            Point target;
            if (i == k + 1)
                target = pntEnd;
            else
                target = Point(randInt(uBound, dBound), randInt(lBound, rBound));
            while (cur != target) {
                path.push_back(cur);
                if ((cur.y == target.y || randInt(0, 1)) && cur.x != target.x)
                    cur.x += (target.x - cur.x) / abs(target.x - cur.x);
                else
                    cur.y += (target.y - cur.y) / abs(target.y - cur.y);
            }
        }
        path.push_back(cur);

        return path;
    };

    for (int i = 0; i < teamCnt; i++) {
        parent[i + 1] = i + 1;
        for (int j = i + 1; j < teamCnt; j++)
            edges.emplace_back(i, j, ::manhattanDistance(pntGeoCenter[i], pntGeoCenter[j]));
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
            Point pntStart = teamBorder[e.u][randInt(0, (int) teamBorder[e.u].size() - 1)],
                    pntEnd = teamBorder[e.v][randInt(0, (int) teamBorder[e.v].size() - 1)];
            vector<Point> path = rndPath(pntStart, pntEnd);
            for (auto p: path)
                isCrucialPath[p.x][p.y] = true;
        }
    }
    for (int i = 1; i <= servMap.width; i++)
        for (int j = 1; j <= servMap.length; j++)
            if (!isCrucialPath[i][j] && rnd() % 7 <= 3)
                mountainToBe[i][j] = true;

    cout << "In function generate: mountainToBe has been set" << endl;

    vector<vector<int>> root(servMap.width + 2, vector<int>(servMap.length + 2));
    auto depth = root;
    vector<vector<bool>> considered(servMap.width + 2, vector<bool>(servMap.length + 2));
    vector<vector<vector<Point>>> ancestor(servMap.width + 2, vector<vector<Point>>(servMap.length + 2));
    queue<Point> q;
    int cntRoot = 0;

    // Apply algorithm "Binary Lifting LCA"
    auto circleLength = [&ancestor, &root, &depth](Point p, Point q) -> int {
        if (root[p.x][p.y] != root[q.x][q.y])
            return USHRT_MAX;
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

    auto expandMountains = [&]() -> void {
        while (!q.empty()) {
            Point cur = q.front(), nxt;
            q.pop();
            for (auto d: direction8) {
                nxt = Point(cur.x + d[0], cur.y + d[1]);
                if (!valid(nxt) || !mountainToBe[nxt.x][nxt.y] || considered[nxt.x][nxt.y])
                    continue;

                considered[nxt.x][nxt.y] = true;
                bool flag = false;
                if (depth[nxt.x][nxt.y] <= 3) {
                    flag = rnd() % 2 == 1;
                } else {
                    flag = true;
                    for (int i = 0; i < 8 && flag; i++)
                        for (int j = i + 1; j < 8; j++) {
                            Point a(nxt.x + direction8[i][0], nxt.y + direction8[i][1]);
                            Point b(nxt.x + direction8[j][0], nxt.y + direction8[j][1]);
                            if (valid(a) && valid(b) &&
                                servMap.info[a.x][a.y].type == CellType::mountain &&
                                servMap.info[b.x][b.y].type == CellType::mountain) {
                                int dist = circleLength(a, b);
                                if (!idxTeamChunk[nxt.x][nxt.y] && dist > 8 ||
                                    idxTeamChunk[nxt.x][nxt.y] && dist > 5) {
                                    flag = false;
                                    break;
                                }
                            }
                        }

                    flag = flag || rnd() % 7 <= 4;
                }

                if (flag) {
                    for (auto i: direction4) {
                        Point crown(nxt.x + i[0], nxt.y + i[1]);
                        if (valid(crown) && servMap.info[crown.x][crown.y].type == CellType::crown) {
                            bool surrounded = true;
                            for (auto j: direction4)
                                if (valid(Point(crown.x + j[0], crown.y + j[1])))
                                    surrounded = surrounded &&
                                                 servMap.info[crown.x + j[0]][crown.y + j[1]].type != CellType::land;
                            if (surrounded) {
                                flag = false;
                                break;
                            }
                        }
                    }
                }

                if (flag) {
                    q.push(nxt);
                    depth[nxt.x][nxt.y] = depth[cur.x][cur.y] + 1;
                    root[nxt.x][nxt.y] = root[cur.x][cur.y];
                    servMap.info[nxt.x][nxt.y].type = CellType::mountain;
                    int cntAncestor = (int) log2(depth[nxt.x][nxt.y]) + 1;
                    ancestor[nxt.x][nxt.y].resize(cntAncestor);
                    ancestor[nxt.x][nxt.y][0] = cur;
                    for (int i = 1; i < cntAncestor; i++)
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
            mountainToBe[p.x][p.y] && !considered[p.x][p.y]) {
            considered[p.x][p.y] = true;
            bool flagLegal = true;
            for (int i = 0; i < 8 && flagLegal; i++)
                for (int j = i + 1; j < 8; j++) {
                    Point a(p.x + direction8[i][0], p.y + direction8[i][1]);
                    Point b(p.x + direction8[j][0], p.y + direction8[j][1]);
                    if (valid(a) && valid(b) &&
                        servMap.info[a.x][a.y].type == CellType::mountain &&
                        servMap.info[b.x][b.y].type == CellType::mountain) {
                        int dist = circleLength(a, b);
                        if (!idxTeamChunk[p.x][p.y] && dist > 10 || idxTeamChunk[p.x][p.y] && dist > 5) {
                            flagLegal = false;
                            break;
                        }
                    }
                }

            if (flagLegal) {
                q.emplace(p.x, p.y);
                depth[p.x][p.y] = 1;
                root[p.x][p.y] = ++cntRoot;
                servMap.info[p.x][p.y].type = CellType::mountain;
                ancestor[p.x][p.y].emplace_back(p.x, p.y);

                expandMountains();
            }
        }

    cout << "In function generate: obstacles have been placed" << endl;


    // Change some mountains into castles

    for (int i = 1; i <= servMap.width; i++)
        for (int j = 1; j <= servMap.length; j++)
            if (servMap.info[i][j].type == CellType::mountain && rnd() % 13 <= 1) {
                servMap.info[i][j].type = CellType::castle;
                servMap.info[i][j].number = randInt(40, 50);
            }

    servMap.calcStat();

    return servMap;
}
