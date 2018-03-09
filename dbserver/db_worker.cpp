#include <string>
#include <map>
#include "util.h"
#include "message_queue.h"
#include "package.h"
#include "cjson_helper.h"
#include "db_worker.h"
#include "logger.h"


DBWorker::DBWorker(int worker_id):ThreadWorker(worker_id),conn_success_(false)
{

}

DBWorker::~DBWorker()
{

}

void DBWorker::Start()
{	
	struct message_queue* q = NULL;

	if (!conn_success_)
	{
		LogError("DBWorker::Start: conn_fail");
		return;
	}

	while(1)
	{
		//LogDebug("DBWorker:Start before\n");
		q = MessageDispatch();
		
		switch(q->type)
		{
			case MQ_TYPE_PACK:
			{
				Package* pack = q->pack;
				string body_data = (char*)pack->GetBodyData();

				JsonHelper jsonhp(body_data);

				string sql;

				if(!jsonhp.GetJsonItem("sql", sql))
				{
					LogError("DBWorker::sql parse error");
				}

				LogDebug("DBWorker::sql %s",sql.c_str());

				if (pack->GetCommandId() == SQL_QUERY)
				{
					char* result = DealQuerySqlOne(sql.c_str());
					//LogDebug("MQ_TYPE_PACK:%d,%d,%s\n", pack->GetCommandId(), pack->GetSubId(), result);
					MessageQueue::getSingleton().MQ2C_Push(q->uniqid, pack->GetCommandId(), pack->GetSubId(), result);
				}
				else if (pack->GetCommandId() == SQL_UPDATE)
				{
					DealUpdateSql(sql.c_str());
				}
				
				//处理完delete掉
				delete pack;
				pack = NULL;
				break;
			}
			case MQ_TYPE_TICK:
				break;
			case MQ_TYPE_CLOSE:
				LogDebug("DBWorker::MQ_TYPE_CLOSE");
				break;
			default:
				LogError("DBWorker:MQ_TYPE_ERROR");	
				break;
		}
		
		delete q;
		q = NULL;

		//LogDebug("DBWorker:Start after\n");
	}
}

void DBWorker::ConnectDB(const char* host, const char* user, const char* passwd, const char* db, unsigned int port)
{
	int ret = db_conn_.Init(host, user, passwd, db, port);
	if (ret < 0)
		conn_success_ = false;
	else
		conn_success_ = true;
}

int DBWorker::DealQuerySql(const char* sql_query)
{
	CResultSet* result = db_conn_.ExecuteQuery(sql_query);

	while (result->Next())
	{
		int idx = result->GetInt("id");
		char* name = result->GetString("hash");

		LogDebug("Deal_Query_Sql:%d,%s\n", idx, name);
	}

	delete result;
	return 0;
}

char* DBWorker::DealQuerySqlOne(const char* sql_query)
{
	JsonHelper jsonhp;
	CResultSet* result = db_conn_.ExecuteQuery(sql_query);
	if (result->Next())
	{

		map<string, int> key_map = result->GetKeyMap();
		map<string, int>::iterator iter = key_map.begin();
		for (; iter != key_map.end(); ++iter)
		{	
			const char* key = iter->first.c_str();
			char* value = result->GetString(key);
			jsonhp.SetJsonItem(key, value);
		}
		return jsonhp.Json2Char();
	}
	return "{}";
}

void DBWorker::DealUpdateSql(const char* sql_update)
{
	bool flag = db_conn_.ExecuteUpdate(sql_update);
	if (flag)
	{
		LogDebug("DBWorker::DealUpdateSql success\n");
	}
	else
	{
		LogDebug("DBWorker::DealUpdateSql fail\n");
	}
}
