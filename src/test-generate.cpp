#include "serverMap.h"
#include "mapGenerator.h"
#include <algorithm>

int main() {
    using namespace std;

//    ofstream fOut("test000.txt");
    vector<int> idTeam{1, 2,3,4,5,6,7,8};
    ServerMap servMap = MapGenerator::randomMap(8, 8, idTeam);
    servMap.print();
    cout << endl << endl;
}
