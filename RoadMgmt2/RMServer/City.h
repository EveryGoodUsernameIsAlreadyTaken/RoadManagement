#pragma once
#include <string>

using namespace std;

class City {
public:
	int CId;
	string CityName;

	City() {}

	City(int cid, string name)
	{
		CId = cid;
		CityName = name;
	}
};