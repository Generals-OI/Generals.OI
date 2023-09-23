#include "serverMap.h"
#include "mapGenerator.h"
#include <algorithm>
#include <windows.h>

int main() {
    using namespace std;

    vector<int> idTeam{1, 2, 2, 3, 3, 4, 4};
    ServerMap map1 = RandomMapGenerator::randomMap(7, 4, idTeam, 1);
    QByteArray byteArray = map1.toByteArray();
    qDebug() << byteArray;
    ServerMap map2;
    map2.loadByteArray(byteArray);

    Sleep(1000);

    map1.print();
    puts("===");
    map2.print();
    puts("===");
}
