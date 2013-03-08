/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2012  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
*/

#include "commonc++/SQLDatabase.h++"
#include "commonc++/Log.h++"
#include "commonc++/SQLQuery.h++"

#include "SQLPrivate.h++"

namespace ccxx {

/*
 */

static const int OPCODES_PER_PROGRESS_CALLBACK = 1000;

/*
 */

class SQLDatabase::Private
{
  public:

  Private()
    : db(NULL), stmtBegin(NULL), stmtCommit(NULL), stmtRollback(NULL)
  {
  }

  void checkValid() throw(SQLException)
  {
    if(! db)
      throw SQLException("database not opened");
  }

  sqlite3 *db;
  sqlite3_stmt *stmtBegin;
  sqlite3_stmt *stmtCommit;
  sqlite3_stmt *stmtRollback;
};

/*
 */

SQLDatabase::SQLDatabase()
  : _private(new Private())
{
  ::sqlite3_initialize();
}

/*
 */

SQLDatabase::~SQLDatabase() throw()
{
  close();

  delete _private;

  ::sqlite3_shutdown();
}

/*
 */

uint_t SQLDatabase::executeSQL(const String &sql) throw(SQLException,
                                                        InterruptedException)
{
  _private->checkValid();

  Log_debug("executeSQL: %s", sql.c_str());

  uint_t rows = ::sqlite3_total_changes(_private->db);
  uint_t selRows = 0;

  char *errmsg;
  int r = ::sqlite3_exec(_private->db, sql.c_str(),
                         &SQLDatabase::_execCallback, &selRows, &errmsg);
  if(r == SQLITE_INTERRUPT)
    throw InterruptedException();
  else if(r != SQLITE_OK)
  {
    String msg;
    msg << "SQL error: " << errmsg;
    ::sqlite3_free(errmsg);
    throw SQLException(msg, sql);
  }

  uint_t rowsf = ::sqlite3_total_changes(_private->db);

  return((rowsf - rows) + selRows);
}

/*
 */

String SQLDatabase::getVersion() throw()
{
  return(::sqlite3_libversion());
}

/*
 */

void SQLDatabase::open(const String &path, bool create /* = true */,
                       bool readOnly /* = false */) throw(IOException)
{
  if(_private->db)
    throw SQLException("database already open");

  int flags = SQLITE_OPEN_FULLMUTEX;

  if(readOnly)
    flags |= SQLITE_OPEN_READONLY;
  else
    flags |= SQLITE_OPEN_READWRITE;

  if(create)
    flags |= SQLITE_OPEN_CREATE;

  int r = ::sqlite3_open_v2(path.c_str(), &_private->db, flags, NULL);

  if((r == SQLITE_CANTOPEN) &&
#ifdef CCXX_OS_WINDOWS
     ((::GetLastError() == ERROR_FILE_NOT_FOUND)
      || (::GetLastError() == ERROR_PATH_NOT_FOUND))
#else
     (errno == ENOENT)
#endif
    )
    throw PathNotFoundException(path);
  else if(r != SQLITE_OK)
    throw IOException(String() << "Failed to open database: "
                      << ::sqlite3_errmsg(_private->db));

  ::sqlite3_progress_handler(_private->db, OPCODES_PER_PROGRESS_CALLBACK,
                             &SQLDatabase::_progressCallback, this);
  ::sqlite3_commit_hook(_private->db, &SQLDatabase::_commitCallback, this);
  ::sqlite3_rollback_hook(_private->db, &SQLDatabase::_rollbackCallback, this);

  r = ::sqlite3_prepare_v2(_private->db, "BEGIN IMMEDIATE", -1,
                           &_private->stmtBegin, NULL);

  r = ::sqlite3_prepare_v2(_private->db, "COMMIT", -1,
                           &_private->stmtCommit, NULL);

  r = ::sqlite3_prepare_v2(_private->db, "ROLLBACK", -1,
                           &_private->stmtRollback, NULL);
}

/*
 */

void SQLDatabase::close() throw()
{
  Log_debug("SQLDatabase::close()");

  if(_private->db)
  {
    _private->checkValid();

    sqlite3_stmt *stmt;
    while((stmt = ::sqlite3_next_stmt(_private->db, NULL)) != NULL)
      ::sqlite3_finalize(stmt);

    ::sqlite3_close(_private->db);
    _private->db = NULL;
  }
}

/*
 */

void SQLDatabase::interrupt() throw(SQLException)
{
  _private->checkValid();

  ::sqlite3_interrupt(_private->db);
}

/*
 */

bool SQLDatabase::isAutoCommit() throw(SQLException)
{
  _private->checkValid();

  return(::sqlite3_get_autocommit(_private->db) != 0);
}

/*
 */

SQLQuery *SQLDatabase::prepareQuery(const String &statement)
  throw(SQLException)
{
  _private->checkValid();

  sqlite3_stmt *stmt = NULL;

  int r = ::sqlite3_prepare_v2(_private->db, statement.c_str(), -1, &stmt,
                               NULL);

  if(r != SQLITE_OK)
    throw SQLException(String() << "Failed to prepare statement: "
                       << ::sqlite3_errmsg(_private->db), statement);

  SQLPrivate priv(_private->db, stmt);
  return(new SQLQuery(priv));
}

/*
 */

int64_t SQLDatabase::getGeneratedKey() throw(SQLException)
{
  _private->checkValid();

  return(::sqlite3_last_insert_rowid(_private->db));
}

/*
 */

uint_t SQLDatabase::getModifiedRowCount() throw(SQLException)
{
  _private->checkValid();

  return(static_cast<uint_t>(::sqlite3_changes(_private->db)));
}

/*
 */

void SQLDatabase::begin() throw(SQLException)
{
  _private->checkValid();

  ::sqlite3_reset(_private->stmtBegin);
  if(::sqlite3_step(_private->stmtBegin) != SQLITE_DONE)
    throw SQLException(String() << "begin transaction failed: "
                       << ::sqlite3_errmsg(_private->db));
}

/*
 */

void SQLDatabase::commit() throw(SQLException, TimeoutException)
{
  _private->checkValid();

  ::sqlite3_reset(_private->stmtCommit);
  int r = ::sqlite3_step(_private->stmtCommit);

  if(r == SQLITE_BUSY)
    throw TimeoutException();
  else if(r != SQLITE_DONE)
    throw SQLException(String() << "commit failed: "
                       << ::sqlite3_errmsg(_private->db));
}

/*
 */

void SQLDatabase::rollback() throw(SQLException, TimeoutException)
{
  _private->checkValid();

  ::sqlite3_reset(_private->stmtRollback);
  int r = ::sqlite3_step(_private->stmtRollback);

  if(r == SQLITE_BUSY)
    throw TimeoutException();
  else if(r != SQLITE_DONE)
    throw SQLException(String() << "rollback failed: "
                       << ::sqlite3_errmsg(_private->db));
}

/*
 */

bool SQLDatabase::onProgress()
{
  return(true);
}

/*
 */

void SQLDatabase::onCommit()
{
  Log_debug("commit");
}

/*
 */

void SQLDatabase::onRollback()
{
  Log_debug("rollback");
}

/*
 */

void SQLDatabase::_checkOpened() throw(SQLException)
{
  if(! _private->db)
    throw SQLException("database not opened");
}

/*
 */

int SQLDatabase::_progressCallback(void *self)
{
  SQLDatabase *database = reinterpret_cast<SQLDatabase *>(self);

  return(database->onProgress() ? 0 : 1);
}

/*
 */

int SQLDatabase::_commitCallback(void *self)
{
  SQLDatabase *database = reinterpret_cast<SQLDatabase *>(self);

  database->onCommit();
  return(0);
}

/*
 */

void SQLDatabase::_rollbackCallback(void *self)
{
  SQLDatabase *database = reinterpret_cast<SQLDatabase *>(self);

  database->onRollback();
}

/*
 */

int SQLDatabase::_execCallback(void *self, int cols, char **values,
                               char **colnames)
{
  uint_t *rows = reinterpret_cast<uint_t *>(self);
  ++(*rows);

  return(0);
}

/*
 */

ScopedTransaction::ScopedTransaction(SQLDatabase &database)
  throw(SQLException)
  : _database(database),
    _committed(false)
{
  _database.begin();
}

/*
 */

ScopedTransaction::~ScopedTransaction() throw()
{
  if(! _committed)
  {
    try
    {
      _database.rollback();
    }
    catch(const Exception &ex)
    {
      // ignore
    }
  }
}

/*
 */

void ScopedTransaction::commit() throw(SQLException, TimeoutException)
{
  _database.commit();
  _committed = true;
}


}; // namespace ccxx

/* end of source file */
