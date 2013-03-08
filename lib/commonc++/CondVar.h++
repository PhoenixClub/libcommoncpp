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

#ifndef __ccxx_CondVar_hxx
#define __ccxx_CondVar_hxx

#include <commonc++/Common.h++>

#ifdef CCXX_OS_POSIX
#include <pthread.h>
#endif

namespace ccxx {

class Mutex; // fwd decl

/** A condition variable -- a synchronization mechanism used to
 * coordinate the actions of multiple threads. A condition variable
 * is <i>signalled</i> to indicate that the state of some shared
 * data has changed; one or more threads can <i>wait</i> on the
 * condition variable for this change to occur.
 *
 * @author Mark Lindner
 */

class COMMONCPP_API CondVar
{
  public:

  /** Construct a new <b>CondVar</b>. */
  CondVar() throw();

  /** Destructor. */
  ~CondVar() throw();

  /** Wait for the condition variable to be signalled. The associated mutex
   * should be held by the thread at the time that it calls this method.
   *
   * @param mutex The mutex that is protecting the shared data; the
   * mutex will be automatically released during the wait and reacquired
   * when the wait completes.
   * @param msec The amount of time to wait, or <b>FOREVER</b> to wait
   * indefinitely.
   * @return <b>true</b> if the condition variable was signalled,
   * <b>false</b> otherwise.
   */
  bool wait(Mutex& mutex, uint_t msec = FOREVER) throw();

  /** Signal one of the threads that are waiting on this condition
   * variable. If predictable scheduling behavior is required, the
   * associated mutex should be held by the thread at the time that it
   * calls this method.
   */
  void notify() throw();

  /** Signal all threads that are waiting on this condition
   * variable.  If predictable scheduling behavior is required, the
   * associated mutex should be held by the thread at the time that
   * it calls this method.
   */
  void notifyAll() throw();

  /** A constant representing an infinite wait. */
  static const uint_t FOREVER;

  private:

#ifdef CCXX_OS_WINDOWS
  HANDLE _sem;
  int _waitersCount;
  CRITICAL_SECTION _waitersLock;
  HANDLE _waitersDone;
  bool _broadcast;
#else
  pthread_cond_t _cond;
#endif

  CCXX_COPY_DECLS(CondVar);
};

}; // namespace ccxx

#endif // __ccxx_CondVar_hxx

/* end of header file */
