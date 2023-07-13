#include <fstream>
#include "serverMap.h"

int main() {
    using namespace std;

//    ofstream fOut("test000.txt");
    vector<int> teamInfo{1, 2};
    ServerMap servMap = generate(2, 2, teamInfo);
    servMap.print();
    cout << endl << endl;

    Point crown;
    for (int i = 1; i <= servMap.length && !crown.x; i++)
        for (int j = 1; j <= servMap.width; j++)
            if (servMap.info[i][j].type == CellType::crown) {
                crown = Point{i, j};
                break;
            }
    for (int i = 1; i <= 101; i++) {
        servMap.addRound();
        servMap.move(servMap.info[crown.x][crown.y].belonging,
                     crown, 1, 0, false);
    }
    servMap.print();

    cout << servMap.stat[0].first.army;
}
