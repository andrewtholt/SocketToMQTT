#include <iostream>
#include <sqlite3.h>
#include <mutex>

class dbAccess {
    private:
        sqlite3 *db=nullptr;
        std::mutex classMutex;
        std::string clientName;

        void doMQTT();

//        int con = 0;
    public:
        dbAccess(std::string dbName, bool &ec);
        void setClientName(std::string clientName);

        std::string dbGet(std::string name);
        bool dbSet(std::string name, std::string value);
};

