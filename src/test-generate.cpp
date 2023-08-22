#include "serverMap.h"
#include "mapGenerator.h"
#include <algorithm>

int main() {
    using namespace std;

    vector<int> idTeam{1, 2, 2, 3, 3, 4, 4};
    ServerMap servMap = RandomMapGenerator::randomMap(7, 4, idTeam, 1);
    servMap.print();
    cout << endl << endl;
}
