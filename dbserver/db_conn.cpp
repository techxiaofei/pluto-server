#include "db_conn.h"

CResultSet::CResultSet(MYSQL_RES* res)
{
	res_ = res;

	int num_fields = mysql_num_fields(res_);
	MYSQL_FIELD* fields = mysql_fetch_fields(res_);
	for (int i = 0; i < num_fields; ++i)
	{
		int a = fields[i].type;  //TODO, 类型
		//LogDebug("CResultSet:%d,%s,%d\n", i, fields[i].name,a);
		key_map_.insert(make_pair(fields[i].name, i));
	}
}

CResultSet::~CResultSet()
{
	if (res_)
	{
		mysql_free_result(res_);
		res_ = NULL;
	}
}

bool CResultSet::Next()
{
	row_ = mysql_fetch_row(res_);
	if (row_)
	{
		return true;
	}
	else
	{
		return false;
	}

	//return row_?true:false;
}

int CResultSet::_GetIndex(const char* key)
{
	KeyMap::iterator iter = key_map_.find(key);
	if (iter == key_map_.end())
	{
		return -1;
	}
	else
	{
		return iter->second;
	}
}

int CResultSet::GetInt(const char* key)
{
	int idx = _GetIndex(key);
	if (idx == -1)
	{
		return 0;
	}
	else
	{
		return atoi(row_[idx]);
	}
}

char* CResultSet::GetString(const char* key)
{
	int idx = _GetIndex(key);
	if (idx == -1)
	{
		return NULL;
	}
	else
	{
		return row_[idx];
	}
}

CDBConn::CDBConn()
{
	mysql_ = NULL;
}

CDBConn::~CDBConn()
{

}

int CDBConn::Init(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
{
	mysql_ = mysql_init(NULL);
	if (!mysql_)
	{
		LogError("mysql init error\n");
		return 1;
	}

	bool reconnect = true;
	mysql_options(mysql_, MYSQL_OPT_RECONNECT, &reconnect);
	mysql_options(mysql_, MYSQL_SET_CHARSET_NAME, "utf8mb4");

	if (!mysql_real_connect(mysql_, host, user, passwd, db, port, NULL, 0))
	{
		LogError("mysql_real_connect fail:%s\n",mysql_error(mysql_));
		return 2;
	}
	LogDebug("db_conn_init success\n");
	return 0;
}

CResultSet* CDBConn::ExecuteQuery(const char* sql_query)
{
	LogDebug("CDBConn::ExecuteQuery:%s\n", sql_query);

	mysql_ping(mysql_);

	if (mysql_real_query(mysql_, sql_query, strlen(sql_query)))
	{
		LogError("mysql_real_query error:%s\n", mysql_error(mysql_));
		return NULL;
	}

	MYSQL_RES* res = mysql_store_result(mysql_);
	if (!res)
	{
		LogError("mysql_store_result error: %s\n", mysql_error(mysql_));
		return NULL;
	}

	CResultSet* result_set = new CResultSet(res);
	return result_set;
}

bool CDBConn::ExecuteUpdate(const char* sql_query)
{
	mysql_ping(mysql_);

	if (mysql_real_query(mysql_, sql_query, strlen(sql_query)))
	{
		LogError("mysql_real_query error:%s\n", mysql_error(mysql_));
		return false;
	}

	if (mysql_affected_rows(mysql_) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t CDBConn::GetInsertId()
{
	return (uint32_t)mysql_insert_id(mysql_);
}


