#include "serverMap.h"
#include "mapGenerator.h"
#include <algorithm>

int main() {
    using namespace std;

    vector<int> idTeam{1, 1, 1, 1, 2, 2, 2, 2};
    ServerMap servMap = RandomMapGenerator::randomMap(8, 2, idTeam, 4097);
    servMap.print();
    cout << endl << endl;
}
