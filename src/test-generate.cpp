#include "serverMap.h"
#include <algorithm>

int main() {
    using namespace std;

//    ofstream fOut("test000.txt");
    vector<int> idTeam{1, 2,3,4,5,6,7,8};
    ServerMap servMap = generateMap(8, 8, idTeam);
    servMap.print();
    cout << endl << endl;
    // 1689573054
//    string str= servMap.exportMap(true);
//
//    cout<<std::count(str.begin(), str.end(),'_')<<endl;
//    GlobalMap globMap;
//    globMap.import(str);
//    globMap.print();
//    cout<<endl<<endl;
//
//    servMap.addRound();
//    servMap.addRound();
//    str= servMap.exportMap(false);
//    cout<<std::count(str.begin(), str.end(),'_')<<endl;
//    globMap.import(str);
//    globMap.print();
//    cout<<endl<<endl;
//
////    Point general;
////    for (int i = 1; i <= servMap.length && !general.x; i++)
////        for (int j = 1; j <= servMap.width; j++)
////            if (servMap.map[i][j].type == CellType::general) {
////                general = Point{i, j};
////                break;
////            }
////    for (int i = 1; i <= 101; i++) {
////        servMap.addRound();
////        servMap.move(servMap.map[general.x][general.y].belonging,
////                     general, 1, 0, false);
////    }
////    servMap.print();
//
//    cout << servMap.stat[0].first.army;
}
