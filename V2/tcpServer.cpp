/*
   C socket server example, handles multiple clients using threads
   Compile
   gcc tcpServer.c -lpthread -o tcpServer
   */

#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <mutex>
#include <iostream>
#include <sqlite3.h>
#include <mqttHelper.h>

using namespace std;

using namespace std;

void mine(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    cout << "Hello from msg callback " << endl;

    cout << "Topic:" << msg->topic << endl;
    cout << "Msg  :" << (char *)msg->payload << endl;
    cout << "=====" << endl;
}
//
// The thread function
//
void *connection_handler(void *);

std::mutex mtx;

class threadCounter {
    private:
        int threadCount=0;
    public:

        void incCount() {
            std::lock_guard<std::mutex> lck (mtx);
            threadCount++;
        }

        void decCount() {
            std::lock_guard<std::mutex> lck (mtx);
            threadCount--;

            if( threadCount < 0) {
                cout << "Thread count sub-zero" << endl;
                threadCount=0;
            }
        }

        int getCount() {
            std::lock_guard<std::mutex> lck (mtx);
            return(threadCount);
        }
};

threadCounter C;

string dbName;

void usage() {

    cout << "\nUsage: tcpServer -d <database> -m <MQTT Broker> -p <port> -v | -h" << endl;
    cout << "\t-d <db>\tSqlite3 database file." << endl;
    cout << "\t-m <mqtt>\tName or IP of MQTT broker." << endl;
    cout << "\t-p <mqtt>\tPort used by MQTT broker." << endl;
    cout << "\t-h\tHelp." << endl;
    cout << "\t-v\tVerbose." << endl;
}

int main(int argc , char *argv[]) {
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;

    //    char *mqttServer = NULL;
    string mqttServer ;
    int mqttPort = 1833;
    bool verbose=false;

    dbName = "../map.db";
    mqttServer = "192.168.10.124";

    int f;
    while(( f=getopt(argc,argv,"d:hm:p:v")) != -1) {
        switch(f) {
            case 'd':
                dbName = optarg;
                if (verbose) {
                    cout << "DB file  :" << dbName << endl;
                }
                break;
            case 'h':
                usage();
                return 0;
                break;
            case 'm':
                mqttServer = optarg;
                if (verbose) {
                    cout << "MQTT     :" << mqttServer << endl;
                }
                break;

            case 'p':
                mqttPort = stoi(optarg);
                if (verbose) {
                    cout << "MQTT Port:" << mqttPort << endl;
                }
                break;

            case 'v':
                verbose=true;
                cout << "Verbose" << endl;
                break;
        }
    }

    bool fail = true;
    if(dbName.empty()) {
        cerr << "Fatal error, no database specified" << endl;
    } else {
        fail = false;
    }

    if(mqttServer.empty() ) {
        cerr << "Fatal error, no MQTT server specified" << endl;
    } else {
        fail = false;
    }

    if (fail) {
        return 1;
    }

    //
    // Create socket
    //
    socket_desc = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);

    if (socket_desc == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    //    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // Bind to localhost
    server.sin_port = htons( 8888 );
    //
    // Bind
    //
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
    //
    // Listen
    //
    listen(socket_desc , 3);
    //
    // Accept and incoming connection
    //
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    //
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    char buffer[1024];
    int len;
    char *tmp=NULL;

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
        puts("Connection accepted");
        bzero(buffer,sizeof(buffer));

        //        len = recv(client_sock, buffer, sizeof(buffer), 0);

        //        tmp = strtok(buffer,"\r\n");

        //        cout << len << endl;
        //        cout << ">" << tmp << "<" << endl;


        /*
           if( tmp != NULL) {
           if(!strcmp(tmp, (char *)"INPUT")) {
           */

        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0) {
            perror("could not create thread");
            return 1;
        }
        /*
        //
        // Now join the thread , so that we dont terminate before the thread
        // pthread_join( thread_id , NULL);
        //
        puts("Handler assigned");
        } else {
        close(client_sock);
        }
        }
        */
    }

    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }

    return 0;
}
void displayLineHex(uint8_t *a) {
    int i;

    for(i=0;i<16;i++) {
        printf(" %02x",*(a++));
    }
}


void displayLineAscii(uint8_t *a) {
    int i;

    printf(":");

    for(i=0;i<16;i++) {
        if( (*a < 0x20 ) || (*a > 0x80 )) {
            printf(".");
            a++;
        } else {
            printf("%c",*(a++));
        }
    }
    printf("\r\n");
}

void dump(void *ptr, int len ) {
    uint8_t *address = (uint8_t *) ptr;
    int lines=len/16;

    if ( lines == 0) {
        lines=1;
    }

    int i=0;
    for( i = 0; i<len;i+=16) {
        printf("%08x:", (uintptr_t)address);
        displayLineHex( address );
        displayLineAscii( address );
        address +=16;
    }
}
/*
 * This will handle connection for each client
 */
void *connection_handler(void *socket_desc) {
    //
    // Get the socket descriptor
    //
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];

    string clientName;

    char data[32] ;
    memset(data,0,sizeof data);
    mqttHelper *h = mqttHelper::Instance();

    h->setClientID("DB_TEST");
    h->setHost("192.168.10.124");
    h->setUserDataPointer((void *)data);

    bool mqttFail=h->connect2MQTT();

    h->setMsgCallback((void*)mine);

    char buffer[80];
    int count=0;
    int len=80;

    C.incCount();

    sqlite3 *db=nullptr;
    int con=0; // sqlite3 db connection

    bool fail = true;

    con = sqlite3_open_v2(dbName.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL);

    if ( con != SQLITE_OK ) {
        cerr << "failed to open database "<< dbName << endl;
    } else {
        fail = false;
    }

    if ( fail == false) {
        char sql[255];
        char out[255];
        sqlite3_stmt *res = NULL;

        while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 ) {
            //
            // End of string marker
            //
            memset(sql,0,sizeof(sql));
            memset(out,0,sizeof(out));

            printf("Thread count : %d\n", C.getCount());
            printf("Read         : %d\n", read_size);
            dump( client_message, read_size) ;

            char *cmd = (char *)strtok(client_message," \n\r");

            if( !strcmp(cmd, "GET")) {

                if(!clientName.empty()) {
                    // CHANGE:
                    // Get from MySQL comman database.
                    //
                    char *name = (char *)strtok(NULL, " \n\r");
                    printf("%s\n",name);

                    sprintf(sql,"SELECT value from map where name='%s';",name);

                    printf("sql is >%s<\n", sql);

                    int rc = sqlite3_prepare_v2( db, sql, -1, &res, NULL);
                    rc = sqlite3_step(res);

                    char *tmp=(char *)sqlite3_column_text(res, 0);

                    sprintf(out,"%s\n",tmp);
                    printf("result=%s\n",tmp);
                    write(sock , out , strlen(out));
                    sqlite3_finalize(res);
                } else {
                    sprintf(out,"-NONAME\n");
                    write(sock , out , strlen(out));
                }

            } else if(!strcmp(cmd,"SUB")) {
                // PUB <name>
                // Get topic(s) from comman database
                //
                if(!clientName.empty()) {
                }
            } else if(!strcmp(cmd,"SET")) {
                char *name = (char *)strtok(NULL, " \n\r");
                char *value = (char *)strtok(NULL, " \n\r");

                if( name != NULL && value != NULL) {

                    if(!clientName.empty()) {
                        memset(sql,0,sizeof(sql));
                        sprintf(sql,"SELECT cmdTopic from map where name='%s' and client_name='%s';",name,clientName.c_str());
                        printf("sql is:%s\n",sql);
                        int rc = sqlite3_prepare_v2( db, sql, -1, &res, NULL);
                        rc = sqlite3_step(res);

                        char *cmdTopic=(char *)sqlite3_column_text(res, 0);

                        printf("Command topic %s %s\n", cmdTopic, value);
                        sqlite3_finalize(res);
                        memset(sql,0,sizeof(sql));
                        sprintf(sql,  "update map set value='%s' where name='%s';",value,name);
                        printf("sql is >%s<\n", sql);

                        char *err_msg = 0;

                        sqlite3_exec(db,sql, 0,0, &err_msg);

                        sqlite3_free(err_msg);
                    } else {
                        sprintf(out,"-NONAME\n");
                        write(sock , out , strlen(out));
                    }
                }
            } else if(!strcmp(cmd,"ID")) {

                if( clientName.empty()) {
                    char *tmp = (char *)strtok(NULL, " \n\r");
                    clientName = tmp;
                }

            }
        }

        // client_message[read_size] = '\0';
        //        fgets(buffer, 80,stdin);
        //
        // Send the message back to client
        //
        //        write(sock , buffer , strlen(buffer));
        //
        // clear the message buffer
        //
        memset(client_message, 0, 2000);
    }

    if(read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if(read_size == -1) {
        perror("recv failed");
    }

    if ( fail ) {
        cerr << "Error in thread" << endl;
    }

    C.decCount();
    sqlite3_close(db);
    close(sock);

    return 0;
}

