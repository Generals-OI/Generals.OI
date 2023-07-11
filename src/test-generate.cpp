#include <fstream>
#include "serverMap.h"

int main() {
    using namespace std;

//    ofstream fOut("test000.txt");
    vector<int> teamInfo = {0, 1,2};
    ServerMap servMap;
    servMap.gMap = generate(2, 2, teamInfo);
    servMap.gMap.print();
    cout<<endl<<endl;

    servMap.gMap.import(servMap.gMap.export2Str(false));
    servMap.gMap.print();
    cout<<endl<<endl;

    servMap.gMap.import(servMap.gMap.export2Str(false));
    servMap.gMap.print();
    cout<<endl<<endl;
}
