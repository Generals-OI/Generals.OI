#include <fstream>
#include "serverMap.h"

int main() {
    using namespace std;

//    ofstream fOut("test000.txt");
    vector<int> teamInfo = {0, 1,2};
    ServerMap servMap;
    servMap = generate(2, 2, teamInfo);
    servMap.print();
    cout<<endl<<endl;

    servMap.import(servMap.export2Str(false));
    servMap.print();
    cout<<endl<<endl;

    servMap.import(servMap.export2Str(false));
    servMap.print();
    cout<<endl<<endl;
}
