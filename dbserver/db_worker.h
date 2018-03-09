#ifndef __DB_WORKER_H__
#define __DB_WORKER_H__

#include "thread_worker.h"
#include "db_conn.h"

using namespace pluto;

/*
worker thread to deal sql operation
 */


class DBWorker : ThreadWorker
{
public:
	DBWorker(int worker_id);
	~DBWorker();

	void ConnectDB(const char* host, const char* user, const char* passwd, const char* db, unsigned int port);

	void Start();

	int DealQuerySql(const char* sql_query);
	char* DealQuerySqlOne(const char* sql_query);
	void DealUpdateSql(const char* sql_query);


private:
	CDBConn db_conn_;
	bool conn_success_;


};
#endif