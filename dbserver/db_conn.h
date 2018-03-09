#ifndef __DB_CONN_H__
#define __DB_CONN_H__

#include <mysql.h>
#include <map>
#include <string>
#include "util.h"
#include "logger.h"

using namespace pluto;

class CResultSet
{
public:
	typedef map<string, int> KeyMap;

	CResultSet(MYSQL_RES* res);
	virtual ~CResultSet();

	bool Next();
	int GetInt(const char* key);
	char* GetString(const char* key);

	KeyMap& GetKeyMap(){return key_map_;}

private:
	int _GetIndex(const char* key);

	MYSQL_RES* res_;
	MYSQL_ROW row_;
	KeyMap key_map_;

};

class CDBConn
{
public:
	CDBConn();
	virtual ~CDBConn();
	int Init(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);

	CResultSet* ExecuteQuery(const char* sql_query);
	bool ExecuteUpdate(const char* sql_query);

	uint32_t GetInsertId();
	MYSQL* GetMysql(){return mysql_;}

private:
	MYSQL* mysql_;



};

#endif
