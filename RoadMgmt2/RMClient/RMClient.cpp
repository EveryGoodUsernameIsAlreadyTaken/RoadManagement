// RMClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <WinSock2.h>
#include <string>
#include <iostream>
#include <vector>
#include <time.h>
#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)

#define MAX_MSG_LEN     256  
#define MAX_SVR_LEN     10
using namespace std;

int main(int argc, char* argv[])
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);

    vector<int> roads[11];
    roads[1] = { 2, 3, 4 };
    roads[2] = { 5, 6 };
    roads[3] = { 5, 6 };
    roads[4] = { 5, 6 };
    roads[5] = { 7, 8, 9 };
    roads[6] = { 7, 8, 9 };
    roads[7] = { 10 };
    roads[8] = { 10 };
    roads[9] = { 10 };

    // Configration File
    string ipaddress = "192.168.1.4";
    int portnum = 10200;

    cout << "Please type your city id: ";
    int CityId;
    cin >> CityId;

    // Set Max Server Num
    SOCKET sockServer;
    SOCKADDR_IN servaddr;

    sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    servaddr = { 0 };
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ipaddress.c_str());
    servaddr.sin_port = htons(portnum++);

    int re = 0;
    re = connect(sockServer, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (re == -1)
    {
        cout << "No server online..." << endl;
    }

    char msg[MAX_MSG_LEN] = "Request connection...\n";
    cout << msg << endl;
    send(sockServer, msg, sizeof(msg), 0);

    while (1) 
    {
        if (sockServer != NULL)
        {
            for (int road: roads[CityId])
            {
                struct tm newtime;
                __time64_t long_time;
                errno_t err;
                _time64(&long_time);
                err = _localtime64_s(&newtime, &long_time);

                srand(time(0) + newtime.tm_sec);
                int ASLimit = rand() % 20 + 55;
                int WCondition = rand() % 10;
                int TCondition = rand() % 10;
                int Blocked = (rand() % 10 == 0) ? 1 : 0;
                string CurTime = to_string(newtime.tm_hour) + ":" + to_string(newtime.tm_min) + ":" + to_string(newtime.tm_sec);

                sprintf_s(msg, "%d,%d,%d,%d,%d,%d,%s", CityId, road, ASLimit, WCondition, TCondition, Blocked, CurTime.c_str());
                send(sockServer, msg, sizeof(msg), 0);
                Sleep(20 * 1000);
            }
            recv(sockServer, msg, sizeof(msg), 0);
            cout << msg << endl;
        }
    }

    closesocket(sockServer);
    WSACleanup();
    return 0;
}

