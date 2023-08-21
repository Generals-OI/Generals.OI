#include "serverMap.h"
#include "mapGenerator.h"
#include <algorithm>

int main() {
    using namespace std;

    vector<int> idTeam{1, 2, 3, 4, 5, 6, 7, 8};
    ServerMap servMap = RandomMapGenerator::randomMap(8, 8, idTeam, 1);
    servMap.print();
    cout << endl << endl;
}
