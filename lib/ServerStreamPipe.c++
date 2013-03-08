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

#include "commonc++/ServerStreamPipe.h++"
#include "commonc++/File.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#endif

#ifdef CCXX_OS_WINDOWS
#include "commonc++/Windows.h++"
#include "cbits.h"
#endif

#include <sys/stat.h>
#include <cerrno>

namespace ccxx {

#ifdef CCXX_OS_POSIX

#ifndef SUN_LEN
#define SUN_LEN (A) \
    (std::strlen((A)->sun_path) + sizeof((A)->sun_len) \
     + sizeof((A)->sun_family))
#endif

#endif

/*
 */

ServerStreamPipe::ServerStreamPipe(const String &path,
                                   uint_t backlog /* = 3 */)
  : _backlog(backlog),
    _listening(false)
{
#ifdef CCXX_OS_WINDOWS

  _path = path;
  _path.prepend("\\\\.\\pipe\\");

#else

  _path = path;

#endif
}

/*
 */

ServerStreamPipe::~ServerStreamPipe() throw()
{
  shutdown();
}

/*
 */

void ServerStreamPipe::listen() throw(IOException)
{
  if(_listening)
    return;

#ifdef CCXX_OS_WINDOWS

  SECURITY_ATTRIBUTES sa;
  WinPerms wperm;

  Permissions perm = Permissions::ALL_READ_WRITE;
  Windows::encodePermissions(perm, wperm);

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = wperm.pdesc;
  sa.bInheritHandle = FALSE;

  _handle = ::CreateNamedPipe(_path.c_str(),
                              PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                              PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
                              PIPE_UNLIMITED_INSTANCES,
                              PIPE_BUF,
                              PIPE_BUF,
                              INFINITE,
                              &sa);

  if(_handle == NULL)
    throw IOException(System::getErrorString("CreateNamedPipe"));

#else

  File::remove(_path); // just in case

  _handle = ::socket(AF_LOCAL, SOCK_STREAM, 0);
  if(_handle < 0)
    throw IOException(System::getErrorString("socket"));

  int v = 1;
  if(::setsockopt(_handle, SOL_SOCKET, SO_REUSEADDR, (char *)&v, sizeof(v))
     != 0)
    throw IOException(System::getErrorString("setsockopt"));

  struct sockaddr_un saddr;
  saddr.sun_family = AF_LOCAL;
  std::strcpy(saddr.sun_path, _path.c_str());
  socklen_t len = SUN_LEN(&saddr);

  if(::bind(_handle, (struct sockaddr *)&saddr, len) != 0)
  {
    String err = System::getErrorString("bind");
    shutdown();
    throw IOException(err);
  }

  if(::listen(_handle, _backlog) != 0)
  {
    String err = System::getErrorString("listen");
    shutdown();
    throw IOException(err);
  }

#endif

  _listening = true;
}

/*
 */

void ServerStreamPipe::accept(StreamPipe& pipe) throw(IOException)
{
  if(pipe.isConnected())
    throw IOException("pipe already connected");

#ifdef CCXX_OS_WINDOWS

  if(! ::ConnectNamedPipe(_handle, NULL))
  {
    if(::GetLastError() != ERROR_PIPE_CONNECTED)
      throw IOException(System::getErrorString("ConnectNamedPipe"));
  }

  pipe.setPipeHandle(_handle);

#else

  struct sockaddr_un raddr;
  socklen_t sz = sizeof(struct sockaddr_un);
  int sd;

  while((sd = ::accept(_handle, (sockaddr *)&raddr, &sz)) < 0)
  {
    if(errno != EINTR)
      throw IOException(System::getErrorString("accept"));
  }

  pipe.setPipeHandle(sd);

#endif
}

/*
 */

void ServerStreamPipe::shutdown() throw()
{
#ifdef CCXX_OS_WINDOWS

  if(_handle != NULL)
  {
    ::CloseHandle(_handle);
    _handle = NULL;
  }

#else

  if(_handle != -1)
  {
    ::shutdown(_handle, SHUT_RDWR);
    ::close(_handle);
    _handle = -1;
  }

  File::remove(_path);

#endif

  _listening = false;
}


}; // namespace ccxx

/* end of source file */
