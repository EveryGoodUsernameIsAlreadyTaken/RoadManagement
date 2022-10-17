#pragma once
#include <string>
#include <iostream>
#include <process.h>
#include <Windows.h>
#include <vector>
#include <stack>
#include <thread>
#include <time.h>
#include "RTInfo.h"
#include "mssqldb.h"
#include "MyUtils.h"

class NetworkMgmt
{
private:
	SOCKET _sockServer;
	SOCKET _sockClient;
	static vector<RTInfo> roads [11];
	static City cities[11];

public:
	NetworkMgmt();
	void Setup(char*, int portnum);
	void RoadLoop();
	void CloseSocket();
	static void readDB();
	static string GetDate();
	static void FindFastestRoute(int city, int taketime, int totTime[11], int prevCity[11]);
	static void DoIt(void* param);
	static RTInfo InputRoad(string info);

	/*

	void AcceptProc(int index);

	void ReadProc(int index);

	void CloseProc(int index);

	HANDLE AddNetworkEvent(SOCKET sock, long net_event);

	void EventLoop(SOCKET sock);

	void AcceptLoop(SOCKET sock);

	*/
};

