// Copyright © 2017-2018 Dmitriy Khaustov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: Dmitriy Khaustov aka xDimon
// Contacts: khaustov.dm@gmail.com
// File created on: 2017.05.08

// MysqlConnection.cpp


#include <cstring>
#include "MysqlConnection.hpp"
#include "MysqlConnectionPool.hpp"
#include "MysqlResult.hpp"
#include "../../thread/Thread.hpp"

#ifndef ASYNC_MYSQL
#define ASYNC_MYSQL 0
 #if MARIADB_BASE_VERSION // https://mariadb.com/kb/en/library/using-the-non-blocking-library/
  #define ASYNC_MYSQL 1
 #endif
#endif

void MysqlConnection::implWait(std::chrono::steady_clock::duration duration)
{
	auto pool = _pool.lock();
	if (pool)
	{
		auto iam = pool->detachDbConnection();
		Thread::self()->postpone(duration);
		pool->attachDbConnection(iam);
	}
}

MYSQL* MysqlConnection::implConnect(
	const char* host,
	const char* user,
	const char* passwd,
	const char* db,
	unsigned int port,
	const char* unix_socket,
	unsigned long clientflag
)
{
	my_bool on = true;
	mysql_options(_mysql, MYSQL_OPT_RECONNECT, &on);
#if ASYNC_MYSQL
	mysql_options(_mysql, MYSQL_OPT_NONBLOCK, nullptr);
#endif // ASYNC_MYSQL
	mysql_options(_mysql, MYSQL_SET_CHARSET_NAME, "utf8"); // utf8mb4
	mysql_options(_mysql, MYSQL_INIT_COMMAND, "SET time_zone='+00:00';\n");

	MYSQL *ret = nullptr;
#if ASYNC_MYSQL
	auto status = mysql_real_connect_start(
		&ret,
		_mysql,
		host,
		user,
		passwd,
		db,
		port,
		unix_socket,
		clientflag
	);

	while (status != 0)
	{
		if (auto pool = _pool.lock())
		{
			pool->log().warn("Postpone CONNECT %p", this);
		}
		Thread::self()->postpone(std::chrono::milliseconds(10));
		status = mysql_real_connect_cont(&ret, _mysql, status);
	}
	if (auto pool = _pool.lock())
	{
		pool->log().warn("Return   CONNECT %p", this);
	}

#else // ASYNC_MYSQL
	ret = mysql_real_connect(
		_mysql,
		host,
		user,
		passwd,
		db,
		port,
		nullptr,
		CLIENT_REMEMBER_OPTIONS
	);
#endif // ASYNC_MYSQL
	return ret;
}

void MysqlConnection::implClose()
{
#if ASYNC_MYSQL
	auto status = mysql_close_start(_mysql);
	while (status != 0)
	{
		if (auto pool = _pool.lock())
		{
			pool->log().warn("Postpone CLOSE %p", this);
		}
		implWait(std::chrono::milliseconds(10));
		status = mysql_close_cont(_mysql, status);
	}
	if (auto pool = _pool.lock())
	{
		pool->log().warn("Return   CLOSE %p", this);
	}
#else // ASYNC_MYSQL
	mysql_close(_mysql);
#endif // ASYNC_MYSQL
}

bool MysqlConnection::implQuery(const std::string& sql)
{
	// Выполнение запроса
#if ASYNC_MYSQL
	int ret = 0;
	auto status = mysql_real_query_start(&ret, _mysql, sql.c_str(), sql.length());
	while (status != 0)
	{
		if (auto pool = _pool.lock())
		{
			pool->log().warn("Postpone QUERY %p", this);
		}
		implWait(std::chrono::milliseconds(10));
		status = mysql_real_query_cont(&ret, _mysql, status);
	}
	if (auto pool = _pool.lock())
	{
		pool->log().warn("Return   QUERY %p", this);
	}
	return ret == 0;
#else // ASYNC_MYSQL
	return mysql_real_query(_mysql, sql.c_str(), sql.length()) == 0;
#endif // ASYNC_MYSQL
}

MYSQL_RES* MysqlConnection::implStoreResult()
{
#if ASYNC_MYSQL
	MYSQL_RES* ret;
	auto status = mysql_store_result_start(&ret, _mysql);
	while (status != 0)
	{
		if (auto pool = _pool.lock())
		{
			pool->log().warn("Postpone STORE %p", this);
		}
		implWait(std::chrono::milliseconds(10));
		status = mysql_store_result_cont(&ret, _mysql, status);
	}
	if (auto pool = _pool.lock())
	{
		pool->log().warn("Return   STORE %p", this);
	}
	return ret;
#else // ASYNC_MYSQL
	return mysql_store_result(_mysql);
#endif // ASYNC_MYSQL
}

int MysqlConnection::implNextResult()
{
#if ASYNC_MYSQL
	int ret;
	auto status = mysql_next_result_start(&ret, _mysql);
	while (status != 0)
	{
		if (auto pool = _pool.lock())
		{
			pool->log().warn("Postpone NEXT %p", this);
		}
		implWait(std::chrono::milliseconds(10));
		status = mysql_next_result_cont(&ret, _mysql, status);
	}
	if (auto pool = _pool.lock())
	{
		pool->log().warn("Return   NEXT %p", this);
	}
	return ret;
#else // ASYNC_MYSQL
	return mysql_next_result(_mysql);
#endif // ASYNC_MYSQL
}

void MysqlConnection::implFreeResult(MYSQL_RES* result)
{
#if ASYNC_MYSQL
	auto status = mysql_free_result_start(result);
	while (status != 0)
	{
		if (auto pool = _pool.lock())
		{
			pool->log().warn("Postpone FREE %p", this);
		}
		implWait(std::chrono::milliseconds(10));
		status = mysql_free_result_cont(result, status);
	}
	if (auto pool = _pool.lock())
	{
		pool->log().warn("Return   FREE %p", this);
	}
#else // ASYNC_MYSQL
	mysql_free_result(result);
#endif // ASYNC_MYSQL
}

MysqlConnection::MysqlConnection(
	const std::shared_ptr<DbConnectionPool>& pool,
	const std::string& dbname,
	const std::string& dbuser,
	const std::string& dbpass,
	const std::string& dbserver,
	unsigned int dbport
)
: DbConnection(std::dynamic_pointer_cast<MysqlConnectionPool>(pool))
, _mysql(nullptr)
, _transaction(0)
{
	_mysql = mysql_init(_mysql);
	if (_mysql == nullptr)
	{
		throw std::runtime_error("Can't init mysql connection");
	}

	if (implConnect(
		dbserver.c_str(),
		dbuser.c_str(),
		dbpass.c_str(),
		dbname.c_str(),
		dbport,
		nullptr,
		CLIENT_REMEMBER_OPTIONS
	) == nullptr)
	{
		throw std::runtime_error(std::string("Can't connect to database ← ") + mysql_error(_mysql));
	}

	unsigned int timeout = 900;
	mysql_options(_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
}

MysqlConnection::MysqlConnection(
	const std::shared_ptr<DbConnectionPool>& pool,
	const std::string& dbname,
	const std::string& dbuser,
	const std::string& dbpass,
	const std::string& dbsocket
)
: DbConnection(pool)
, _mysql(nullptr)
, _transaction(0)
{
	_mysql = mysql_init(_mysql);
	if (_mysql == nullptr)
	{
		throw std::runtime_error("Can't init mysql connection");
	}

	if (implConnect(
		nullptr,
		dbuser.c_str(),
		dbpass.c_str(),
		dbname.c_str(),
		0,
		dbsocket.c_str(),
		CLIENT_REMEMBER_OPTIONS
	) == nullptr)
	{
		throw std::runtime_error(std::string("Can't connect to database ← ") + mysql_error(_mysql));
	}

	unsigned int timeout = 900;
	mysql_options(_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
}

MysqlConnection::~MysqlConnection()
{
	implClose();
}

std::string MysqlConnection::escape(const std::string& str)
{
	size_t size = str.length() * 4 + 1;
	if (size > 1024)
	{
		auto buff = new char[size];
		mysql_escape_string(buff, str.c_str(), str.length());
		std::string result(buff);
		free(buff);
		return std::move(result);
	}
	else
	{
		char buff[size];
		mysql_escape_string(buff, str.c_str(), str.length());
		return std::forward<std::string>(buff);
	}
}

bool MysqlConnection::startTransaction()
{
	if (_transaction > 0)
	{
		_transaction++;
		return true;
	}

	if (DbConnection::query("START TRANSACTION;"))
	{
		_transaction = 1;
		return true;
	}

	return false;
}

bool MysqlConnection::deadlockDetected()
{
	return mysql_errno(_mysql) == 1213;
}

bool MysqlConnection::commit()
{
	if (_transaction > 1)
	{
		_transaction--;
		return true;
	}
	if (_transaction == 0)
	{
		if (auto pool = _pool.lock())
		{
			pool->log().warn("Internal error: commit when counter of transaction is zero");
		}
		return false;
	}
	if (DbConnection::query("COMMIT;"))
	{
		_transaction = 0;
		return true;
	}

	return false;
}

bool MysqlConnection::rollback()
{
	if (_transaction > 1)
	{
		_transaction--;
		return true;
	}
	if (_transaction == 0)
	{
		if (auto pool = _pool.lock())
		{
			pool->log().warn("Internal error: rollback when counter of transaction is zero");
		}
		return false;
	}
	if (DbConnection::query("ROLLBACK;"))
	{
		_transaction = 0;
		return true;
	}

	return false;
}

bool MysqlConnection::query(const std::string& sql, DbResult* res, size_t* affected, size_t* insertId)
{
	auto pool = _pool.lock();

	auto result = dynamic_cast<MysqlResult *>(res);

	auto beginTime = std::chrono::steady_clock::now();
	if (pool) pool->metricAvgQueryPerSec->addValue();

	if (pool) pool->log().debug("MySQL query: %s", sql.c_str());

	bool success = implQuery(sql);

	auto now = std::chrono::steady_clock::now();
	auto timeSpent =
		static_cast<double>(std::chrono::steady_clock::duration(now - beginTime).count()) /
		static_cast<double>(std::chrono::steady_clock::duration(std::chrono::seconds(1)).count());
	if (timeSpent > 0)
	{
		if (pool) pool->metricAvgExecutionTime->addValue(timeSpent, now);
	}

	if (timeSpent >= 5)
	{
		if (pool) pool->log().warn("MySQL query too long: %0.03f sec\n\t\tFor query:\n\t\t%s", timeSpent, sql.c_str());
	}

	if (!success)
	{
		if (pool) pool->metricFailQueryCount->addValue();
		if (!deadlockDetected())
		{
			if (pool) pool->log().warn("MySQL query error: [%u] %s\n\t\tFor query:\n\t\t%s", mysql_errno(_mysql), mysql_error(_mysql), sql.c_str());
		}
		return false;
	}

	if (affected != nullptr)
	{
		*affected = mysql_affected_rows(_mysql);
	}

	if (insertId != nullptr)
	{
		*insertId = mysql_insert_id(_mysql);
	}

	if (result != nullptr)
	{
		result->set(implStoreResult());

		if (result->get() == nullptr && mysql_errno(_mysql) != 0)
		{
			if (pool) pool->metricFailQueryCount->addValue();
			if (pool) pool->log().error("MySQL store result error: [%u] %s\n\t\tFor query:\n\t\t%s", mysql_errno(_mysql), mysql_error(_mysql), sql.c_str());
			return false;
		}
	}

	if (pool) pool->metricSuccessQueryCount->addValue();
	return true;
}

bool MysqlConnection::multiQuery(const std::string& sql)
{
	auto pool = _pool.lock();

	mysql_set_server_option(_mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON);

	auto beginTime = std::chrono::steady_clock::now();
	if (pool) pool->metricAvgQueryPerSec->addValue();

	bool success = implQuery(sql);

	auto now = std::chrono::steady_clock::now();
	auto timeSpent =
		static_cast<double>(std::chrono::steady_clock::duration(now - beginTime).count()) /
		static_cast<double>(std::chrono::steady_clock::duration(std::chrono::seconds(1)).count());
	if (timeSpent > 0)
	{
		if (pool) pool->metricAvgExecutionTime->addValue(timeSpent, now);
	}

	if (!success)
	{
		if (pool) pool->log().error("MySQL query error: [%u] %s\n\t\tFor query:\n\t\t%s", mysql_errno(_mysql), mysql_error(_mysql), sql.c_str());

		mysql_set_server_option(_mysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
		if (pool) pool->metricFailQueryCount->addValue();
		return false;
	}

	for(;;)
	{
		MYSQL_RES* result = implStoreResult();
		if (result != nullptr)
		{
			implFreeResult(result);
		}
		if (implNextResult() == 0)
		{
			break;
		}
	}

	mysql_set_server_option(_mysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
	if (pool) pool->metricSuccessQueryCount->addValue();

	return true;
}
