#include "mssqldb.h"

// define handles and variables
//SQLHANDLE sqlConnHandle;
SQLHENV henv;
SQLHDBC hdbc1;
SQLHSTMT hstmt1;

SQLWCHAR retconstring[SQL_RETURN_CODE_LEN];
RETCODE retcode;

void mssqldb::ConnectDB()
{
	hstmt1 = NULL;
	hdbc1 = SQL_NULL_HDBC;

	cout << "Attempting connection to SQL Server..." << endl;

	// Allocate the ODBC environment and save handle.  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);
	if ((retcode != SQL_SUCCESS_WITH_INFO) && (retcode != SQL_SUCCESS)) {
		printf("SQLAllocHandle(Env) Failed\n\n");
		getchar();
		DisconnectDB();
	}

	// Notify ODBC that this is an ODBC 3.0 app.  
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if ((retcode != SQL_SUCCESS_WITH_INFO) && (retcode != SQL_SUCCESS)) {
		printf("SQLSetEnvAttr(ODBC version) Failed\n\n");
		getchar();
		DisconnectDB();
	}

	// Allocate ODBC connection handle and connect.  
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc1);
	if ((retcode != SQL_SUCCESS_WITH_INFO) && (retcode != SQL_SUCCESS)) {
		printf("SQLAllocHandle(hdbc1) Failed\n\n");
		getchar();
		DisconnectDB();
	}

	SQLWCHAR retconstring[SQL_RETURN_CODE_LEN];
	// This sample use Integrated Security. Create the SQL Server DSN by using the Windows NT authentication.   
	//retcode = SQLConnect(hdbc1, L"DRIVER={SQL Server};SERVER=noesys, 1433; DATABASE=CPPDB; UID=cpptest; PWD=cpptest;", SQL_NTS, (SQLWCHAR*)"", SQL_NTS, (SQLWCHAR*)"", SQL_NTS);

	retcode = SQLDriverConnect(hdbc1,
		NULL,
		GetWString("DRIVER={SQL Server};SERVER=noesys, 1433; DATABASE=CPPDB; UID=cpptest; PWD=cpptest;"),
		SQL_NTS,
		retconstring, 1024, NULL, SQL_DRIVER_NOPROMPT);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLConnect() Failed\n\n");
		getchar();
		DisconnectDB();
	}

	//ConnectStetement();
}

void mssqldb::ConnectStatement()
{
	// Allocate statement handle.  
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLAllocHandle(hstmt1) Failed\n\n");
		getchar();
		DisconnectDB();
	}
}

void mssqldb::DisconnectDB()
{
	if (hstmt1 != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	if (hdbc1 != SQL_NULL_HDBC) {
		SQLDisconnect(hdbc1);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc1);
	}

	if (henv != SQL_NULL_HENV)
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
}

void mssqldb::DisconnectStatement()
{
	if (hstmt1 != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
}

SQLWCHAR* mssqldb::GetWString(string szStr)
{
	int iLen = strlen(szStr.c_str()) + 1;
	SQLWCHAR* wStr = new SQLWCHAR[iLen];

	for (int i = 0; i < iLen; i++)
	{
		wStr[i] = szStr[i];
	}
	return wStr;
}

void mssqldb::ShowErrorMsg(char* fn, SQLHANDLE handle, SQLSMALLINT type)
{
	SQLSMALLINT i = 0;
	SQLINTEGER NativeError;
	SQLWCHAR SQLState[7];
	SQLWCHAR MessageText[256];
	SQLSMALLINT TextLength;
	SQLRETURN ret;

	fprintf(stderr, "\nThe driver reported the following error [%s]\n", fn);
	do
	{
		ret = SQLGetDiagRec(type, handle, ++i, SQLState, &NativeError,
			MessageText, sizeof(MessageText), &TextLength);
		if (SQL_SUCCEEDED(ret)) {
			printf_s("%s:%ld:%ld:%s\n\n", SQLState, (long)i, (long)NativeError, MessageText);
		}
	} while (ret == SQL_SUCCESS);
}

// -------------------------------------------------------

bool mssqldb::CheckUser(string user, string table)
{
	bool bRet = false;

	try {
		string szSQL = "SELECT * FROM " + table + " WHERE USERNAME = '" + user + "'";
		ConnectStatement();
		SQLWCHAR* szWSQL = GetWString(szSQL);
		if (SQL_SUCCESS == SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
			if (SQLFetch(hstmt1) == SQL_SUCCESS) bRet = true;
	}
	catch (...)
	{
		cout << "cannot find account" << endl;
	}
	DisconnectStatement();

	return bRet;
}

bool mssqldb::CheckPass(string pass, string table)
{
	bool bRet = false;

	try {
		string szSQL = "SELECT * FROM " + table + " WHERE PASSWORD = '" + pass + "'";
		ConnectStatement();
		SQLWCHAR* szWSQL = GetWString(szSQL);
		if (SQL_SUCCESS == SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
			if (SQLFetch(hstmt1) == SQL_SUCCESS) bRet = true;
	}
	catch (...)
	{
		cout << "cannot find account" << endl;
	}
	DisconnectStatement();

	return bRet;
}

int mssqldb::GetNewIDFromTable(string idCol, string table)
{
	SQLLEN ptrSqlVersion;
	int sqlNewID = 1;

	try {
		string szSQL = "SELECT IF(COUNT(idCol) > 0, MAX(" + idCol + ") + 1 FROM " + table + ", 1";
		SQLWCHAR* szWSQL = GetWString(szSQL);
		ConnectStatement();
		if (SQL_SUCCESS == SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
			if (SQLFetch(hstmt1) == SQL_SUCCESS)
			{
				SQLGetData(hstmt1, 1, SQL_INTEGER, &sqlNewID, SQL_RESULT_LEN, &ptrSqlVersion);
			}
			else cout << "No data" << endl;
	}
	catch (...)
	{
		cout << "No data" << endl;
	}

	DisconnectStatement();
	return sqlNewID;
}

void mssqldb::ExecDBData(string szSQL, DExecType DEType)
{
	string arAlert[]{ "Creating table...", "Inserting data...", "Updating Data...", "Deleting Data..." };
	cout << arAlert[(int)DEType] << endl;

	//if there is a problem execuing the query then exit application
	//else display query result
	SQLWCHAR* szWSQL = GetWString(szSQL);
	ConnectStatement();
	RETCODE rc = SQLExecDirect(hstmt1, szWSQL, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		ShowErrorMsg((char*)(arAlert[(int)DEType]).c_str(), hstmt1, SQL_HANDLE_STMT);
		//Disconnection();
	}
	else {
		string arAlert[]{ "Created table", "Inserted data", "Updated data", "Deleted data" };
		std::cout << arAlert[(int)DEType] << " successfully" << endl << endl;
	}
	DisconnectStatement();
}

void mssqldb::SQLInputStr(int ipar, int coldef, string* sInput)
{
	retcode = SQLBindParameter(hstmt1, ipar, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, coldef, 0, (SQLPOINTER)(*sInput).c_str(), 0, NULL);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLBindParameter(sParm%d) Failed\n\n", ipar);
		getchar();
	}
}

void mssqldb::SQLInputInt(int ipar, int* iInput)
{
	retcode = SQLBindParameter(hstmt1, ipar, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, iInput, 0, NULL);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLBindParameter(sParm%d) Failed\n\n", ipar);
		getchar();
	}
}

void mssqldb::SQLInputFloat(int ipar, float* dInput)
{
	retcode = SQLBindParameter(hstmt1, ipar, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT, 0, 0, dInput, 0, NULL);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLBindParameter(sParm%d) Failed\n\n", ipar);
		getchar();
	}
}

void mssqldb::SQLInputBit(int ipar, bool* bInput)
{
	retcode = SQLBindParameter(hstmt1, ipar, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, 0, 0, bInput, 0, NULL);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLBindParameter(sParm%d) Failed\n\n", ipar);
		getchar();
	}
}

//------------------------------ Stored Procedure
void mssqldb::SPInsertCity(SQLWCHAR* szSPName, City info)
{
	ConnectStatement();

	mssqldb::SQLInputInt(1, &info.CId);
	mssqldb::SQLInputStr(2, 30, &info.CityName);

	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szSPName, SQL_NTS))
	{
		printf("SQLExecDirect Failed\n\n");
	}

	DisconnectStatement();
}

void mssqldb::SPInsertRoadTrafficInfo(SQLWCHAR* szSPName, RTInfo info)
{
	ConnectStatement();

	mssqldb::SQLInputInt(1, &info.FromCity.CId);
	mssqldb::SQLInputInt(2, &info.ToCity.CId);
	mssqldb::SQLInputInt(3, &info.DriveDistance);
	mssqldb::SQLInputInt(4, &info.DirectDistance);
	mssqldb::SQLInputInt(5, &info.AvgSpeedLim);
	mssqldb::SQLInputInt(6, &info.WeatherCondition);
	mssqldb::SQLInputInt(7, &info.TrafficCondition);
	mssqldb::SQLInputBit(8, &info.blocked);
	mssqldb::SQLInputStr(9, 30, &info.Time);

	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szSPName, SQL_NTS))
	{
		printf("SQLExecDirect Failed\n\n");
	}

	DisconnectStatement();
}

void mssqldb::SPInsertIntoRouteInfo(SQLWCHAR* szSPName, vector<string> route, vector<int> timepath, string curTime)
{
	ConnectStatement();

	mssqldb::SQLInputStr(1, 30, &curTime);
	mssqldb::SQLInputStr(2, 30, &route[1]);
	mssqldb::SQLInputInt(3, &timepath[1]);
	mssqldb::SQLInputStr(4, 30, &route[2]);
	mssqldb::SQLInputInt(5, &timepath[2]);
	mssqldb::SQLInputStr(6, 30, &route[3]);
	mssqldb::SQLInputInt(7, &timepath[3]);
	mssqldb::SQLInputInt(8, &timepath[4]);

	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szSPName, SQL_NTS))
	{
		printf("SQLExecDirect Failed\n\n");
	}

	DisconnectStatement();
}

void mssqldb::SPGetRouteWithTime(SQLWCHAR* szSPName, string time)
{
	ConnectStatement();

	mssqldb::SQLInputStr(1, 30, &time);
	
	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szSPName, SQL_NTS))
	{
		printf("SQLExecDirect Failed\n\n");
	}
	else 
	{
		SQLLEN ptrSqlVersion;

		SQLCHAR sqlTime[30];
		SQLCHAR sqlCity1[30];
		SQLINTEGER sqlHours1;
		SQLCHAR sqlCity2[30];
		SQLINTEGER sqlHours2;
		SQLCHAR sqlCity3[30];
		SQLINTEGER sqlHours3;
		SQLCHAR sqlCity4[30];
		SQLINTEGER sqlHours4;
		SQLCHAR sqlCity5[30];
		SQLINTEGER sqlHours5;
		
		char buffer[1000];

		sprintf_s(buffer, "%-20s %-50s", "TIME", "ROUTES");
		cout << buffer << endl;
		sprintf_s(buffer, "%-20s %-50s", "----", "------------------------------------------------------------------------");
		cout << buffer << endl;

		while (SQLFetch(hstmt1) == SQL_SUCCESS) {
			SQLGetData(hstmt1, 1, SQL_CHAR, sqlTime, sizeof(sqlTime), &ptrSqlVersion);
			SQLGetData(hstmt1, 2, SQL_CHAR, sqlCity1, sizeof(sqlCity1), &ptrSqlVersion);
			SQLGetData(hstmt1, 3, SQL_INTEGER, &sqlHours1, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 4, SQL_CHAR, sqlCity2, sizeof(sqlCity2), &ptrSqlVersion);
			SQLGetData(hstmt1, 5, SQL_INTEGER, &sqlHours2, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 6, SQL_CHAR, sqlCity3, sizeof(sqlCity4), &ptrSqlVersion);
			SQLGetData(hstmt1, 7, SQL_INTEGER, &sqlHours3, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 8, SQL_CHAR, sqlCity4, sizeof(sqlTime), &ptrSqlVersion);
			SQLGetData(hstmt1, 9, SQL_INTEGER, &sqlHours4, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 10, SQL_CHAR, sqlCity5, sizeof(sqlCity5), &ptrSqlVersion);
			SQLGetData(hstmt1, 11, SQL_INTEGER, &sqlHours5, SQL_RESULT_LEN, &ptrSqlVersion);

			sprintf_s(buffer, "%-20s %-15s %-2d hrs -> %-15s %-2d hrs -> %-15s %-2d hrs -> %-15s %-2d hrs -> %-15s %-2d hrs", (char*)sqlTime, (char*)sqlCity1, sqlHours1, (char*)sqlCity2, sqlHours2, (char*)sqlCity3, sqlHours3, (char*)sqlCity4, sqlHours4, (char*)sqlCity5, sqlHours5);
			cout << buffer << endl;
		}
		cout << endl;
	}

	DisconnectStatement();
}

//-- REFERENCE
void mssqldb::CreateTable(string szSQL)
{
	std::cout << endl << "Creating Table to DB..." << endl;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	ConnectStatement();
	RETCODE rc = SQLExecDirect(hstmt1, szWSQL, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		ShowErrorMsg((char*)"[Create Table]", hstmt1, SQL_HANDLE_STMT);
	}
	else {
		std::cout << "Created table successfully" << endl;
	}
	DisconnectStatement();

}


