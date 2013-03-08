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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/Semaphore.h++"
#include "commonc++/File.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"
#endif
#ifdef CCXX_OS_WINDOWS
#include "commonc++/Windows.h++"
#endif

namespace ccxx {

/*
 */

Semaphore::Semaphore(const String &name, uint_t value /* = 1 */,
                     const Permissions& perm
                     /* = Permissions::USER_READ_WRITE */)
  : _name(name),
    _value(value),
    _perm(perm),
    _initialized(false)
{
}

/*
 */

Semaphore::~Semaphore() throw()
{
  if(_initialized)
  {
#ifdef CCXX_OS_WINDOWS

    if(_sem != NULL)
      ::CloseHandle(_sem);

#else

    if(_sem != reinterpret_cast<sem_t *>(SEM_FAILED))
    {
      ::sem_close(_sem);
      ::sem_unlink(_name.c_str());
    }

#endif
  }
}

/*
 */

void Semaphore::init() throw(SystemException)
{
  if(_initialized)
    return;

  if(_value == 0)
    throw SystemException("Invalid semaphore value");

#ifdef CCXX_OS_WINDOWS

  SECURITY_ATTRIBUTES sa;
  WinPerms wperm;

  Windows::encodePermissions(_perm, wperm);

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = wperm.pdesc;
  sa.bInheritHandle = FALSE;

  _sem = ::CreateSemaphore(&sa, 0, _value, _name.c_str());
  if(_sem == NULL)
    throw SystemException(System::getErrorString("CreateSemaphore"));

  // make all instances immediately available
  ::ReleaseSemaphore(_sem, _value, NULL);

#else

  if(! POSIX::verifyIPCName(_name))
    throw SystemException("Invalid IPC name");

  _name.prepend(File::separator);

  mode_t mode;
  POSIX::encodePermissions(_perm, mode);
  _sem = ::sem_open(_name.c_str(), (O_CREAT | O_RDWR), mode, _value);
  if(_sem == reinterpret_cast<sem_t *>(SEM_FAILED))
    throw SystemException(System::getErrorString("sem_open"));

#endif

  _initialized = true;
}

/*
 */

bool Semaphore::wait() throw()
{
  if(! _initialized)
    return(false);

#ifdef CCXX_OS_WINDOWS

  return(::WaitForSingleObjectEx(_sem, INFINITE, TRUE) == WAIT_OBJECT_0);

#else

  return(::sem_wait(_sem) == 0);

#endif
}

/*
 */

bool Semaphore::tryWait() throw()
{
  if(! _initialized)
    return(false);

#ifdef CCXX_OS_WINDOWS

  return(::WaitForSingleObjectEx(_sem, 0, TRUE) == WAIT_OBJECT_0);

#else

  return(::sem_trywait(_sem) == 0);

#endif
}

/*
 */

bool Semaphore::signal() throw()
{
  if(! _initialized)
    return(false);

#ifdef CCXX_OS_WINDOWS

  return(::ReleaseSemaphore(_sem, 1, NULL) == TRUE);

#else

  return(::sem_post(_sem) == 0);

#endif
}

/*
 */

int Semaphore::getValue() const throw()
{
  if(! _initialized)
    return(-1);

#ifdef CCXX_OS_WINDOWS

  return(-1); // not supported

#else

  int val = 0;
  int e = ::sem_getvalue(_sem, &val);

  return((e == 0) ? val : -1);

#endif
}

}; // namespace ccxx

/* end of source file */
