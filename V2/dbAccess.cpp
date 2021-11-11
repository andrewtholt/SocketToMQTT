#include "dbAccess.h"
#include <cerrno>


/***********************************************************************
 *  Method: dbAccess::dbAccess
 *  Params: sqlite3 *d
 * Effects: sets ec true on failure.
 ***********************************************************************/
dbAccess::dbAccess(std::string dbName, bool &ec) {
    std::cout << "dbAccess" << std::endl;

    int con = sqlite3_open_v2(dbName.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL);

    if (con != SQLITE_OK) {
        ec = true;;
    } else {
        ec = false;
    }
}


/***********************************************************************
 *  Method: dbAccess::setClientName
 *  Params: string clientName
 * Returns: void
 * Effects:
 ***********************************************************************/
void dbAccess::setClientName(std::string clientName) {
    if( this->clientName.empty() ) {
        this->clientName = clientName;
    }
}


/***********************************************************************
 *  Method: dbAccess::dbGet
 *  Params: std::string name
 * Returns: std::string
 * Effects:
 ***********************************************************************/
std::string dbAccess::dbGet(std::string name) {
    std::string result="";

    if( this->clientName.empty() ) {
        result = "-NONAME";
    } else {
    }

    return result;
}


/***********************************************************************
 *  Method: dbAccess::dbSet
 *  Params: std::string name, std::string value
 * Returns: bool
 * Effects:
 ***********************************************************************/
bool dbAccess::dbSet(std::string name, std::string value) {
    bool failFlag = true;

    return failFlag;
}


/***********************************************************************
 *  Method: dbAccess::doMQTT
 *  Params:
 * Returns: void
 * Effects:
 ***********************************************************************/
void dbAccess::doMQTT() {
}


