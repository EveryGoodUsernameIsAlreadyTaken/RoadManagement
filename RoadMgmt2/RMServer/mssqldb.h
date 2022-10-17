#pragma once
#include <string>
#include <iostream>
#include <Windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <vector>
#include "RTInfo.h"
#include "City.h"

enum DExecType { TABLE = 0, INS = 1, UPD = 2, DEL = 3 };

#define SQL_RESULT_LEN 50
#define SQL_RETURN_CODE_LEN 1000

using namespace std;

class mssqldb
{
public:
	static void ConnectDB();
	static void DisconnectDB();
	static void ConnectStatement();
	static void DisconnectStatement();

	static SQLWCHAR* GetWString(string szStr);
	static void ShowErrorMsg(char* fn, SQLHANDLE handle, SQLSMALLINT type);

	static bool CheckUser(string user, string table);
	static bool CheckPass(string pass, string table);
	static int GetNewIDFromTable(string idCol, string table);
	static void ExecDBData(string szSQL, DExecType DEType);
	//static Customer QueryCustomerInfo(string szSQL);
	//static vector<Customer> QueryCustomerList(string szSQL);

	// For Orders
	//static bool CheckOrderId(string orderNo, string custNo);
	//static int NextOrderNo();
	//static Order QueryOrderInfo(string szSQL);
	//static vector<Order> QueryOrderList(string szSQL);

	static void SQLInputStr(int ipar, int coldef, string* sInput);
	static void SQLInputInt(int ipar, int* iInput);
	static void SQLInputFloat(int ipar, float* dInput);
	static void SQLInputBit(int ipar, bool* bInput);

	// stored procedure
	static void SPInsertCity(SQLWCHAR* szSPName, City info);
	static void SPInsertRoadTrafficInfo(SQLWCHAR* szSPName, RTInfo info);
	static void SPInsertIntoRouteInfo(SQLWCHAR* szSPName, vector<string> route, vector<int> timepath, string curTime);
	static void SPGetRouteWithTime(SQLWCHAR* szSPName, string time);

	//--REFERENCE
	static void CreateTable(string szSQL);
};

