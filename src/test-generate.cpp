#include "serverMap.h"
#include <algorithm>

int main() {
    using namespace std;

//    ofstream fOut("test000.txt");
    vector<int> teamInfo{1, 2,2,3,3};
    ServerMap servMap = generate(5, 3, teamInfo);
    servMap.print();
    cout << endl << endl;

    string str=servMap.export2Str(true);

    cout<<std::count(str.begin(), str.end(),'_')<<endl;
    GlobalMap globMap;
    globMap.import(str);
    globMap.print();
    cout<<endl<<endl;

    servMap.addRound();
    servMap.addRound();
    str=servMap.export2Str(false);
    cout<<std::count(str.begin(), str.end(),'_')<<endl;
    globMap.import(str);
    globMap.print();
    cout<<endl<<endl;

//    Point crown;
//    for (int i = 1; i <= servMap.length && !crown.x; i++)
//        for (int j = 1; j <= servMap.width; j++)
//            if (servMap.info[i][j].type == CellType::crown) {
//                crown = Point{i, j};
//                break;
//            }
//    for (int i = 1; i <= 101; i++) {
//        servMap.addRound();
//        servMap.move(servMap.info[crown.x][crown.y].belonging,
//                     crown, 1, 0, false);
//    }
//    servMap.print();

    cout << servMap.stat[0].first.army;
}
