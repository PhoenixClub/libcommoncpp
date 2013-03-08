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

#include "commonc++/System.h++"
#include "commonc++/File.h++"
#include "commonc++/Log.h++"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <ctime>

#ifdef CCXX_OS_WINDOWS
#include <Lmcons.h>
#include <sys/timeb.h>
#include <Userenv.h>

#include "commonc++/Windows.h++"
#endif // CCXX_OS_WINDOWS

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#include <pwd.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>
#endif // CCXX_OS_POSIX

#include "stacktrace.h"

namespace ccxx {

/*
 */

bool System::_inited = false;
String System::_username;
String System::_osname;
String System::_osver;
String System::_hostname;
String System::_arch;
String System::_homedir;
String System::_tempdir;
Mutex System::_globalLock;

#ifndef CCXX_OS_WINDOWS

static bool __static_init(void)
{
  stacktrace_init();
  ::atexit(stacktrace_cleanup);
  return(true);
}

static bool __static_init_flag = __static_init();

#endif

/*
 */

ccxx::time_ms_t System::currentTimeMillis() throw()
{
#ifdef CCXX_OS_WINDOWS

  struct _timeb tb;

  ::_ftime(&tb);
  return((static_cast<time_ms_t>(tb.time) * 1000)
         + static_cast<time_ms_t>(tb.millitm));
#else

  struct timeval tv;
  struct timezone tz;
  ::gettimeofday(&tv, &tz);
  return((static_cast<time_ms_t>(tv.tv_sec) * 1000)
         + static_cast<time_ms_t>(tv.tv_usec / 1000));
#endif
}

/*
 */

time_s_t System::currentTime() throw()
{
#ifdef CCXX_OS_WINDOWS

  struct _timeb tb;

  ::_ftime(&tb);
  return(static_cast<time_s_t>(tb.time));

#else

  struct timeval tv;
  struct timezone tz;
  ::gettimeofday(&tv, &tz);
  return(static_cast<time_s_t>(tv.tv_sec));

#endif
}

/*
 */

void System::setSystemTime(const time_ms_t &time) throw(SystemException)
{
#ifdef CCXX_OS_WINDOWS

  SYSTEMTIME systime;

  DateTime dt(time);

  systime.wYear = dt.getYear();
  systime.wMonth = dt.getMonth();
  systime.wDayOfWeek = 0; // ignored
  systime.wDay = dt.getDay();
  systime.wHour = dt.getHour();
  systime.wMinute = dt.getMinute();
  systime.wSecond = dt.getSecond();
  systime.wMilliseconds = dt.getMillisecond();

  if(! ::SetLocalTime(&systime))
    throw SystemException(System::getErrorString("SetLocalTime"));

#else

  struct timeval tv;
  tv.tv_sec = time / 1000;
  tv.tv_usec = (time % 1000) * 1000;

  int r = ::settimeofday(&tv, NULL);
  if(r != 0)
    throw SystemException(System::getErrorString("settimeofday"));

#endif
}

/*
 */

String System::getEnvVariable(const String &name)
{
#ifdef CCXX_OS_WINDOWS

  DWORD len = ::GetEnvironmentVariable(name.c_str(), NULL, 0);
  if(len == 0)
    return(String::null);

  char *buf = new char[len];

  len = ::GetEnvironmentVariable(name.c_str(), buf, len);

  String val(buf, 0, len, false);
  return(val);

#else

  String val = ::getenv(name.c_str());
  return(val);

#endif
}

/*
 */

bool System::setEnvVariable(const String &name, const String &value) throw()
{
  if(name.contains('='))
    return(false);

#ifdef CCXX_OS_WINDOWS

  return(::SetEnvironmentVariable(name.c_str(), value.c_str()) == TRUE);

#else

  return(::setenv(name.c_str(), value.c_str(), 1) == 0);

#endif
}

/*
 */

bool System::delEnvVariable(const String &name) throw()
{
  if(name.contains('='))
    return(false);

#ifdef CCXX_OS_WINDOWS

  return(::SetEnvironmentVariable(name.c_str(), NULL) == TRUE);

#else

  ::unsetenv(name.c_str());

#endif

  return(true);
}


/*
 */

String System::getUserName()
{
  _getSystemInfo();

  return(_username);
}

/*
 */

String System::getOSName()
{
  _getSystemInfo();

  return(_osname);
}

/*
 */

String System::getArchitecture()
{
  _getSystemInfo();

  return(_arch);
}

/*
 */

String System::getOSVersion()
{
  _getSystemInfo();

  return(_osver);
}

/*
 */

String System::getHostName()
{
  _getSystemInfo();

  return(_hostname);
}

/*
 */

String System::getHomeDir()
{
  _getSystemInfo();

  return(_homedir);
}

/*
 */

String System::getTempDir()
{
  _getSystemInfo();

  return(_tempdir);
}

/*
 */

void System::_getSystemInfo() throw()
{
  if(! _inited)
  {
    lockGlobalMutex();

#ifdef CCXX_OS_WINDOWS

    char hbuf[MAX_COMPUTERNAME_LENGTH + 1];
    char ubuf[UNLEN + 1];
    ubuf[UNLEN] = 0;

    DWORD size = (DWORD)sizeof(ubuf);
    if(::GetUserName(ubuf, &size) == TRUE)
      _username = ubuf;

    HANDLE token;
    DWORD bufSize = 0;
    char *pbuf = NULL;
    BOOL ok = ::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token);
    if(::GetUserProfileDirectory(token, NULL, &bufSize))
    {
      pbuf = new char[bufSize];
      if(::GetUserProfileDirectory(token, pbuf, &bufSize))
        _homedir = String(pbuf, 0, 0, false);
      else
        delete[] pbuf;
    }
    ::CloseHandle(token);

    if(_homedir.isEmpty())
      _homedir = getEnvVariable("USERPROFILE");

    size = (DWORD)sizeof(hbuf);
    if(::GetComputerName(hbuf, &size) == TRUE)
      _hostname = hbuf;

    SYSTEM_INFO sys_info;
    OSVERSIONINFO os_info;

    ::GetSystemInfo(&sys_info);

    switch(sys_info.dwProcessorType)
    {
      case PROCESSOR_INTEL_386:
      case PROCESSOR_INTEL_486:
      case PROCESSOR_INTEL_PENTIUM:
        _arch = "x86";
        break;

      case PROCESSOR_INTEL_IA64:
        _arch = "ia64";
        break;

#ifdef PROCESSOR_AMD_X8664
      case PROCESSOR_AMD_X8664:
        _arch = "amd8664";
        break;
#endif

      default:
        _arch = "unknown";
        break;
    }

    _osname = "Windows";

    os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    ::GetVersionEx(&os_info);

    switch(os_info.dwMajorVersion)
    {
      case 4:
        switch(os_info.dwMinorVersion)
        {
          case 0:
            if(os_info.dwPlatformId == VER_PLATFORM_WIN32_NT)
              _osver = "NT 4.0";
            else
              _osver = "95";
            break;

          case 10:
            _osver = "98";
            break;

          case 90:
            _osver = "ME";
            break;

          default:
            _osver = "unknown";
            break;
        }
        break;

      case 5:
        switch(os_info.dwMinorVersion)
        {
          case 0:
            _osver = "2000";
            break;

          case 1:
            _osver = "XP";
            break;

          case 2:
            _osver = "Server 2003";
            break;

          default:
            _osver = "unknown";
        }
        break;

      case 6:
        _osver = "Vista";
        break;

      default:
        _osver = "unknown";
        break;
    }

    DWORD tmpLen = ::GetTempPath(0, NULL);
    char *buf = new char[tmpLen];
    ::GetTempPath(tmpLen, buf);
    _tempdir = File::removeTrailingSeparators(buf);
    _tempdir = Windows::decodePathName(_tempdir);

#else

    passwd *pw = getpwuid(geteuid());

    if(pw)
    {
      _username = pw->pw_name;
      _homedir = pw->pw_dir;
    }

    struct utsname uts;
    uname(&uts);

    _osname = uts.sysname;
    _osver = uts.release;
    _hostname = uts.nodename;
    _arch = uts.machine;

    const char *tmpEnv = getenv("TMPDIR");
    _tempdir = (tmpEnv ? tmpEnv : P_tmpdir);
    if(File::isSymLink(_tempdir))
      File::resolveSymLink(_tempdir);

#endif

    _inited = true;

    unlockGlobalMutex();
  }
}

/*
 */

String System::getErrorString(const String &text /* = String::null */)
{
  String s;

  if(! text.isNull())
    s << text << ": ";

#ifdef CCXX_OS_WINDOWS

  DWORD err = ::GetLastError();
  LPVOID msg = NULL;

  DWORD msglen = ::FormatMessage(
    (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM), NULL,
    err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&msg, 0, NULL);

  if(msg == NULL)
    s << "Error " << static_cast<int>(err);
  else
  {
    s += (LPTSTR)msg;
    ::LocalFree(msg);
  }

#else

  // strerror_r is broken on Linux, so we don't use it

  if(errno > 0 && errno <= sys_nerr)
  {
#ifdef CCXX_OS_MACOSX
    s += sys_errlist[errno];
#else
    s += _sys_errlist[errno];
#endif
  }
  else
    s << "Error " << errno;

#endif

  return(s);
}

/*
 */

size_t System::getPageSize() throw()
{
  static size_t size = 0;

  if(size == 0)
  {

#ifdef CCXX_OS_WINDOWS

    SYSTEM_INFO info;

    ::GetSystemInfo(&info);
    size = static_cast<size_t>(info.dwPageSize);

#else

#if defined _SC_PAGESIZE
    size = ::sysconf(_SC_PAGESIZE);
#elif defined HAVE_GETPAGESIZE
    size = ::getpagesize();
#else
#error "No means to determine system page size."
#endif

#endif
  }

  return(size);
}

/*
 */

void System::roundToPageSize(size_t& size) throw()
{
  size_t sz = getPageSize();

  size += (sz - (size % sz));
}

/*
 */

void System::printStackTrace(uint_t maxFrames /* = 20 */)
{
  stacktrace_print(stderr, maxFrames, 0);
}

/*
 */

bool System::isPOSIX() throw()
{
#ifdef CCXX_OS_POSIX
  return(true);
#else
  return(false);
#endif
}

/*
 */

bool System::isMacOSX() throw()
{
#ifdef CCXX_OS_MACOSX
  return(true);
#else
  return(false);
#endif
}

/*
 */

bool System::isWindows() throw()
{
#ifdef CCXX_OS_WINDOWS
  return(true);
#else
  return(false);
#endif
}


}; // namespace ccxx

/* end of source file */
