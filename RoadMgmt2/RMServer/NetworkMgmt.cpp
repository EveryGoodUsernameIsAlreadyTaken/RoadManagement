
#include "NetworkMgmt.h"
#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)

#define BLOG_SIZE   10
#define MAX_MSG_LEN 256


using namespace std;

vector<RTInfo> NetworkMgmt::roads[11];
City NetworkMgmt::cities[11];

NetworkMgmt::NetworkMgmt()
{
    cities[1] = City(1, "LA");
    cities[2] = City(2, "Oklahoma City");
    cities[3] = City(3, "Dallas");
    cities[4] = City(4, "Houston");
    cities[5] = City(5, "St. Louis");
    cities[6] = City(6, "Memphis");
    cities[7] = City(7, "Chicago");
    cities[8] = City(8, "Nashville");
    cities[9] = City(9, "Atlanta");
    cities[10] = City(10, "NY");
    //mssqldb::SPInsertCity(mssqldb::GetWString("{CALL PQ_INSERT_CITY(?,?)}"), cities[1]);
    //mssqldb::SPInsertCity(mssqldb::GetWString("{CALL PQ_INSERT_CITY(?,?)}"), cities[2]);
    //mssqldb::SPInsertCity(mssqldb::GetWString("{CALL PQ_INSERT_CITY(?,?)}"), cities[3]);
    //mssqldb::SPInsertCity(mssqldb::GetWString("{CALL PQ_INSERT_CITY(?,?)}"), cities[4]);
    //mssqldb::SPInsertCity(mssqldb::GetWString("{CALL PQ_INSERT_CITY(?,?)}"), cities[5]);
    //mssqldb::SPInsertCity(mssqldb::GetWString("{CALL PQ_INSERT_CITY(?,?)}"), cities[6]);
    //mssqldb::SPInsertCity(mssqldb::GetWString("{CALL PQ_INSERT_CITY(?,?)}"), cities[7]);
    //mssqldb::SPInsertCity(mssqldb::GetWString("{CALL PQ_INSERT_CITY(?,?)}"), cities[8]);
    //mssqldb::SPInsertCity(mssqldb::GetWString("{CALL PQ_INSERT_CITY(?,?)}"), cities[9]);
    //mssqldb::SPInsertCity(mssqldb::GetWString("{CALL PQ_INSERT_CITY(?,?)}"), cities[10]);


    roads[1] = { RTInfo(cities[1], cities[2], 200, 175), RTInfo(cities[1], cities[3], 150, 130), RTInfo(cities[1], cities[4], 180, 120) };
    roads[2] = { RTInfo(cities[2], cities[5], 275, 130), RTInfo(cities[2], cities[6], 215, 200) };
    roads[3] = { RTInfo(cities[3], cities[5], 250, 200), RTInfo(cities[3], cities[6], 175, 170) };
    roads[4] = { RTInfo(cities[4], cities[5], 190, 160), RTInfo(cities[4], cities[6], 110, 90) };
    roads[5] = { RTInfo(cities[5], cities[7], 260, 215), RTInfo(cities[5], cities[8], 275, 230), RTInfo(cities[5], cities[9], 315, 275) };
    roads[6] = { RTInfo(cities[6], cities[7], 320, 225), RTInfo(cities[6], cities[8], 255, 240), RTInfo(cities[6], cities[9], 235, 200) };
    roads[7] = { RTInfo(cities[7], cities[10], 530, 390) };
    roads[8] = { RTInfo(cities[8], cities[10], 440, 400) };
    roads[9] = { RTInfo(cities[9], cities[10], 485, 455) };
}

void NetworkMgmt::Setup(char* ipaddress, int portnum)
{
    _sockClient = NULL;
    _sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN servaddr = { 0 };
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ipaddress);
    //servaddr.sin_addr = com.GetDefaultMyIP();
    servaddr.sin_port = htons(portnum);

    int re = 0;
    re = bind(_sockServer, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (re == -1) {
        perror("Error occured binding");
    }

    re = listen(_sockServer, BLOG_SIZE);
    if (re == -1) {
        perror("Error occured listening");
    }
}

void NetworkMgmt::RoadLoop()
{
    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);

    cout << "Waiting for client connection..." << endl;

    while (true) {
        _sockClient = accept(_sockServer, (SOCKADDR*)&cliaddr, &len);
        cout << "Connected to client." << endl;
        if (_sockClient == -1)
        {
            perror("Failed to connect to client");
            return;
        }
        printf("Client: %s:%d has connected.\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

        _beginthread(DoIt, 0, (void*)_sockClient);
    }

    /*  while (1)
      {
          struct tm newtime;
          __time64_t long_time;
          errno_t err;
          _time64(&long_time);
          err = _localtime64_s(&newtime, &long_time);

          int iCol = newtime.tm_min * newtime.tm_sec;
          int ASLimit = (rand() * iCol) % 20 + 55;
          int WCondition = (rand() * iCol) % 10;
          int TCondition = (rand() * iCol) % 10;
          int Blocked = (rand() * iCol) % 2;
          string CurTime = to_string(newtime.tm_hour) + ":" + to_string(newtime.tm_min) + ":" + to_string(newtime.tm_sec);

          sprintf_s(msg, "%d,%d,%d,%d, %s", ASLimit, WCondition, TCondition, Blocked, CurTime.c_str());

          int iRet = send(_sockClient, msg, sizeof(msg), 0);
          if (iRet == -1) perror("Failed to send.");
          cout << "Message Sent: " << msg << endl;

          Sleep(5 * 1000);
      }*/
}

void NetworkMgmt::readDB() 
{
    for (string line; getline(cin, line);) 
    {
        if (line.compare("read DB")) 
        {
            auto checkempty = [&](string input, string info) {
                if (input.empty()) {
                    Message::Out(info + " cannot be empty!", 2, true);
                    return true;
                }
                cout << endl;
                return false;
            };
            auto isnumber = [&](string input, int from, int to) {
                if (input.find_first_not_of("0123456789") == string::npos) {
                    int inum = stoi(input);
                    if(inum >= from && inum <= to)
                    return false;
                }
                Message::Out("Please enter a number!", 2, true);
                return true;
            };

            string input, time = "%";

            do {
                cout << "Type the time of the roads: " << endl;
                cout << "Hours: ";
                getline(cin, input);
            } while (checkempty(input, "Hours") || isnumber(input, 1, 24));
            time += input + ":";
            do {
                cout << "Type the time of the roads: " << endl;
                cout << "Minutes: ";
                getline(cin, input);
            } while (checkempty(input, "Minutes") || isnumber(input, 0, 60));
            time += input + "%";

            mssqldb::SPGetRouteWithTime(mssqldb::GetWString("{CALL PQ_SELECT_ROWS_FROM_RTINFO(?)}"), time);
            break;
        }
    }
}

string NetworkMgmt::GetDate() {
    struct tm newtime;
    __time64_t long_time;
    errno_t err;
    _time64(&long_time);
    err = _localtime64_s(&newtime, &long_time);

    return to_string(newtime.tm_mon + 1) + "/" + to_string(newtime.tm_mday) + "/" + to_string(newtime.tm_year + 1900) + " ";
}

void NetworkMgmt::DoIt(void* param)
{
    SOCKET dosock = (SOCKET)param;
    char msg[MAX_MSG_LEN] = "";
    recv(dosock, msg, sizeof(msg), 0);
    printf("Received: %s\n", msg);

    thread wait_for_command(readDB);

    while (recv(dosock, msg, sizeof(msg), 0) > 0)
    {
        printf("navi recv: %s\n", msg);
        RTInfo rtinfo = InputRoad((string)msg);
        
        int totTime[11], prevCity[11];
        for (int i = 0; i <= 10; i++)
            totTime[i] = prevCity[i] = -1;
        FindFastestRoute(1, 0, totTime, prevCity);

        stack<int> fastestpath;
        vector<string> route = { "LA"};
        vector<int> timepath = { 0 };
        int inum = 10;

        if (prevCity[10] != -1)
        {
            fastestpath.push(10);
            while (prevCity[inum] != 1)
            {
                inum = prevCity[inum];
                fastestpath.push(inum);
            }

            cout << cities[1].CityName << " 0 hrs";
            while (!fastestpath.empty())
            {
                inum = fastestpath.top();
                route.push_back(cities[inum].CityName);
                timepath.push_back(totTime[inum]);

                cout << " -> " << cities[inum].CityName << " " << totTime[inum] << " hrs";

                fastestpath.pop();
            }
            cout << endl << endl;
            
            mssqldb::SPInsertIntoRouteInfo(mssqldb::GetWString("{CALL PQ_INSERT_FAST_ROUTE(?,?,?,?,?,?,?,?)}"), route, timepath, rtinfo.Time);
        }
        else
        {
            cout << "Not enough info" << endl << endl;
        }

        send(dosock, msg, sizeof(msg), 0);
    }
    closesocket(dosock);
}

RTInfo NetworkMgmt::InputRoad(string info)
{
    int cityFrom = stoi(info.substr(0, info.find(",")));
    info = info.substr(info.find(",") + 1);

    int cityTo = stoi(info.substr(0, info.find(",")));
    info = info.substr(info.find(",") + 1);

    for (int i = 0; i < NetworkMgmt::roads[cityFrom].size(); i++)
    {
        if (roads[cityFrom][i].ToCity.CId == cityTo) 
        {
            roads[cityFrom][i].AvgSpeedLim = stoi(info.substr(0, info.find(",")));
            info = info.substr(info.find(",") + 1);

            roads[cityFrom][i].WeatherCondition = stoi(info.substr(0, info.find(",")));
            info = info.substr(info.find(",") + 1);

            roads[cityFrom][i].TrafficCondition = stoi(info.substr(0, info.find(",")));
            info = info.substr(info.find(",") + 1);

            roads[cityFrom][i].blocked = info.substr(0, info.find(",")) == "1";
            info = info.substr(info.find(",") + 1);
            
            roads[cityFrom][i].Time = GetDate() + " " + info.substr(0, info.find(","));

            mssqldb::SPInsertRoadTrafficInfo(mssqldb::GetWString("{CALL PQ_INSERT_ROAD_TRAFFIC(?,?,?,?,?,?,?,?,?)}"), roads[cityFrom][i]);
            return roads[cityFrom][i];
        }
    }
}

void NetworkMgmt::FindFastestRoute(int city, int taketime, int totTime[11], int prevCity[11])
{
    for (RTInfo cti : roads[city]) 
    {
        if (!cti.blocked)
        {
            int tocity = cti.ToCity.CId;
            if (totTime[tocity] == -1)
            {
                totTime[tocity] = taketime + cti.DriveTime();
                prevCity[tocity] = city;

            }
            else if (taketime + cti.DriveTime() < totTime[tocity])
            {
                totTime[tocity] = taketime + cti.DriveTime();
                prevCity[tocity] = city;
            }

            FindFastestRoute(tocity, totTime[tocity], totTime, prevCity);
        }
    }
}

void NetworkMgmt::CloseSocket()
{
    closesocket(_sockClient);
    closesocket(_sockServer);
    _sockClient = NULL;
}

/*

void NetworkMgmt::AcceptProc(int index) {
    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);

    SOCKET dosock = accept(sock_base[0], (SOCKADDR*)&cliaddr, &len);
    if (cnt == FD_SETSIZE) {
        printf("Chat room is filled! %s:%d failed to join\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        closesocket(dosock);
        return;
    }

    AddNetworkEvent(dosock, FD_READ | FD_CLOSE);
    printf("%s:%d joined!\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

}

HANDLE NetworkMgmt::AddNetworkEvent(SOCKET sock, long net_event) {
    HANDLE Hev = WSACreateEvent();
    sock_base[cnt] = sock;
    hev_base[cnt] = Hev;
    cnt++;

    WSAEventSelect(sock, Hev, net_event);
    return Hev;
}

void NetworkMgmt::ReadProc(int index) {
    char msg[MAX_MSG_LEN];
    recv(sock_base[index], msg, MAX_MSG_LEN, 0);

    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);

    getpeername(sock_base[index], (SOCKADDR*)&cliaddr, &len);

    char smsg[MAX_MSG_LEN];
    sprintf(smsg, "[%s:%d]:%s", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), msg);
    for (int i = 1; i < cnt; i++) {
        send(sock_base[i], smsg, MAX_MSG_LEN, 0);
    }
}
void NetworkMgmt::CloseProc(int index) {
    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);

    getpeername(sock_base[index], (SOCKADDR*)&cliaddr, &len);

    printf("[%s:%d] has left\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    closesocket(sock_base[index]);
    WSACloseEvent(hev_base[index]);
    cnt--;
    sock_base[index] = sock_base[cnt];
    hev_base[index] = hev_base[cnt];

    char smsg[MAX_MSG_LEN];
    sprintf(smsg, "[%s:%d]: has left", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    for (int i = 1; i < cnt; i++) {
        send(sock_base[i], smsg, MAX_MSG_LEN, 0);
    }
}

void NetworkMgmt::EventLoop(SOCKET sock) {
    AddNetworkEvent(sock, FD_ACCEPT);

    while (true) {
        int index = WSAWaitForMultipleEvents(cnt, hev_base, false, INFINITE, false);
        WSANETWORKEVENTS net_events;
        WSAEnumNetworkEvents(sock_base[index], hev_base[index], &net_events);
        switch (net_events.lNetworkEvents) {
        case FD_ACCEPT:
            AcceptProc(index);
            break;
        case FD_READ:
            ReadProc(index);
            break;
        case FD_CLOSE:
            CloseProc(index);
            break;
        }
    }
    closesocket(sock);
}

void NetworkMgmt::AcceptLoop(SOCKET sock) {
    SOCKET dosock;
    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);
    while (true)
    {
        dosock = accept(sock, (SOCKADDR*)&cliaddr, &len);
        if (dosock == -1)
        {
            perror("Failed to accept");
            break;
        }
        printf("%s:%d - Connection Request\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        DoIt(dosock);
    }
    closesocket(sock);
}
void NetworkMgmt::DoIt(SOCKET dosock) {
    char msg[MAX_MSG_LEN] = "";
    while (recv(dosock, msg, sizeof(msg), 0) > 0)
    {
        printf("recv:%s\n", msg);
        send(dosock, msg, sizeof(msg), 0);
    }
    closesocket(dosock);
}

*/