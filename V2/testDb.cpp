#include "dbAccess.h"

using namespace std;

int main() {

    bool flag= false;
    dbAccess *tst = new dbAccess("../map.db", flag);

    cout << flag << endl;

    tst->setClientName("TEST");

}

