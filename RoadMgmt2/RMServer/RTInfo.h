#pragma once
#include <string>
#include "City.h"

using namespace std;

class RTInfo {
public:
	City FromCity, ToCity;
	int DriveDistance, DirectDistance, TrafficCondition, WeatherCondition, AvgSpeedLim;
	string Time;
	bool blocked;

	RTInfo() {}

	RTInfo(City from, City to, int DrvDist, int DirDist)
	{ 
		FromCity = from;
		ToCity = to;
		DriveDistance = DrvDist;
		DirectDistance = DirDist;
		blocked = true; 
	}

	int DriveTime() 
	{
		if (blocked) 
			return -1;

		int dTime = DriveDistance;
		dTime /= AvgSpeedLim;
		dTime *= 1 + ((float)TrafficCondition / 10);
		dTime *= 1 + ((float)WeatherCondition / 10);

		string tm = Time;
		int hr = stoi(tm.substr(0, tm.find(":")));
		tm = tm.substr(tm.find(":") + 1);
		int min = stoi(tm.substr(0, tm.find(":")));
		tm = tm.substr(tm.find(":") + 1);
		int sec = stoi(Time.substr(0, Time.find(":")));
		
		dTime *= 1+(min(abs(hr - 9), abs(hr - 17))) / 8;

		return dTime;
	}
};