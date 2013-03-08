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

#ifndef __ccxx_SQLDatabase_hxx
#define __ccxx_SQLDatabase_hxx

#include <commonc++/Common.h++>
#include <commonc++/IOException.h++>
#include <commonc++/InterruptedException.h++>
#include <commonc++/String.h++>
#include <commonc++/SQLException.h++>

namespace ccxx {

class SQLQuery; // fdw decl

/** A class representing a session with an embedded SQL
 * database. The class can be subclassed to provide custom
 * implementations for the three callback methods.
 *
 * @author Mark Lindner
 */

class COMMONCPPDB_API SQLDatabase
{
  public:

  /** Construct a new SQLDatabase object. */
  SQLDatabase();

  /** Destructor. Closes the database, if it is open. */
  virtual ~SQLDatabase() throw();

  /** Get the version of the underlying SQLite database software. */
  String getVersion() throw();

  /** Open the database.
   *
   * @param path The pathname of the database file to open.
   * @param create A flag indicating whether the database should be
   * created if it does not exist.
   * @param readOnly A flag indicating whether the database should be
   * opened read-only.
   * @throw IOException If the database file could not be opened.
   */
  void open(const String &path, bool create = true, bool readOnly = false)
    throw(IOException);

  /** Close the database. Any pending uncommitted transactions will be
   * rolled back.
   */
  void close() throw();

  /** Execute an SQL statement.
   *
   * @param sql The statement to execute.
   * @return The number of rows that were affected by the statement.
   * @throw SQLException If a database error occurs.
   * @throw InterruptedException If the statement was interrupted via a
   * call to interrupt().
   */
  uint_t executeSQL(const String &sql)
    throw(SQLException, InterruptedException);

  /** Interrupt any currently-executing SQL statement. The corresponding
   * method will throw an InterruptedException.
   *
   * @throw SQLException If a database error occurs.
   */
  void interrupt() throw(SQLException);

  /** Test if the database is currently in auto-commit mode. Auto-commit
   * mode is on by default if a transaction is not currently in progress.
   *
   * @throw SQLException If a database error occurs.
   */
  bool isAutoCommit() throw(SQLException);

  /** Create a query for the given SQL statement.
   *
   * @param sql The statement.
   * @return The new SQLQuery. The caller is responsible for
   * deleting this object before closing the database.
   * @throw SQLException If a database error occurs.
   */
  SQLQuery *prepareQuery(const String &sql) throw(SQLException);

  /** Get the generated key created by the last SQL "INSERT" statement.
   *
   * @return The generated key.
   * @throw SQLException If a database error occurs.
   */
  int64_t getGeneratedKey() throw(SQLException);

  /** Get the number of rows modified by the last SQL statement. */
  uint_t getModifiedRowCount() throw(SQLException);

  /** Begin a transaction. Transactions cannot be nested.
   *
   * @throw SQLException If a database error occurs.
   */
  void begin() throw(SQLException);

  /** Commit a pending transaction.
   *
   * @throw SQLException If a database error occurs.
   * @throw TimeoutException If a timeout occurs while waiting for
   * database locks. The call should be retried in this case.
   */
  void commit() throw(SQLException, TimeoutException);

  /** Roll back a pending transaction.
   *
   * @throw SQLException If a database error occurs.
   * @throw TimeoutException If a timeout occurs while waiting for
   * database locks. The call should be retried in this case.
   */
  void rollback() throw(SQLException, TimeoutException);

  protected:

  /** A callback that's invoked periodically while a SQL statement is
   * being executed. The default implementation does nothing and returns
   * <b>true</b>.
   *
   * @return <b>true</b> if the in-progress operation should continue, or
   * <b>false</b> if it should be interrupted.
   */
  virtual bool onProgress();

  /** A callback that's invoked each time a transaction is committed. The
   * default implementation does nothing.
   */
  virtual void onCommit();

  /** A callback that's invoked each time a transaction is rolled
   * back. The default implementation does nothing.
   */
  virtual void onRollback();

  private:

  void _checkOpened() throw(SQLException);

  static int _progressCallback(void *self);
  static int _commitCallback(void *self);
  static void _rollbackCallback(void *self);
  static int _execCallback(void *self, int cols, char **values,
                           char **colnames);

  class Private; // fwd decl
  Private *_private;

  CCXX_COPY_DECLS(SQLDatabase);
};

/** A scoped database transaction. ScopedTransaction begins a
 * transaction in its constructor, and automatically rolls back the
 * transaction when it goes out of scope, if the transaction had not
 * previously been committed via a call to commit().
 *
 * @author Mark Lindner
 */

class COMMONCPPDB_API ScopedTransaction
{
  public:

  /** Construct a new ScopedTransaction for the given SQLDatabase.
   *
   * @param database The database.
   * @throw SQLException If a database error occurs.
   */
  ScopedTransaction(SQLDatabase &database) throw(SQLException);

  /** Destructor. Rolls back the transaction if it has not been committed via
   * a call commit().
   */
  ~ScopedTransaction() throw();

  /** Commit the transaction.
   *
   * @throw SQLException If a database error occurs.
   * @throw TimeoutException If a timeout occurs while waiting for
   * database locks. The call should be retried in this case.
   */
  void commit() throw(SQLException, TimeoutException);

  private:

  SQLDatabase &_database;
  bool _committed;

  CCXX_COPY_DECLS(ScopedTransaction);
};

}; // namespace ccxx

#endif // __ccxx_SQLDatabase_hxx

/* end of header file */
