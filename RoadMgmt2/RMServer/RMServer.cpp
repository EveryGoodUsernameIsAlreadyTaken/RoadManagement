// RMServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "NetworkMgmt.h"

using namespace std;

int main(int argc, char* argv[])
{

    int32_t i = 0;
    printf("%" PRId32 "\n", i);

    mssqldb::ConnectDB();

    // if (argc < 3) return 0;
    string ipaddress = "192.168.1.4";
    int portnum = 10200;

    //char ipaddress[20] = "192.168.1.4";
    //int portnum = 10200;

    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);

    NetworkMgmt ntwMgmt = NetworkMgmt();
    ntwMgmt.Setup((char*)ipaddress.c_str(), portnum);
    ntwMgmt.RoadLoop();


    ntwMgmt.CloseSocket();
    WSACleanup();

    mssqldb::DisconnectDB();
    return 0;
}
