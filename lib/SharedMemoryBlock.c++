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

#include "commonc++/SharedMemoryBlock.h++"
#include "commonc++/File.h++"
#include "commonc++/System.h++"

#include <cerrno>

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#endif

#ifdef CCXX_OS_WINDOWS
#include "commonc++/Windows.h++"
#endif

namespace ccxx {

/*
 */

SharedMemoryBlock::SharedMemoryBlock(const String &name, size_t size,
                                     const Permissions &perm
                                     /* = Permissions::USER_READ_WRITE */)
  : MemoryBlock(NULL, size > 0 ? size : 1),
    _name(name),
    _perm(perm)
{
  System::roundToPageSize(_size);
}

/*
 */

SharedMemoryBlock::~SharedMemoryBlock() throw()
{
  detach();
}

/*
 */

void SharedMemoryBlock::attach(bool readOnly /* = false */)
  throw(SystemException)
{
  if(_base != NULL)
    throw SystemException("already attached");

#ifdef CCXX_OS_WINDOWS

  SECURITY_ATTRIBUTES sa;
  WinPerms wperm;

  Windows::encodePermissions(_perm, wperm);

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = wperm.pdesc;
  sa.bInheritHandle = FALSE;

  DWORD rangeLo = _size;
  DWORD rangeHi = 0;

  _handle = ::CreateFileMapping(INVALID_HANDLE_VALUE, &sa,
                                (readOnly ? PAGE_READONLY : PAGE_READWRITE),
                                rangeHi, rangeLo, _name.c_str());
  if(_handle == NULL)
    throw SystemException(System::getErrorString("CreateFileMapping"));

  _base = reinterpret_cast<byte_t *>
    (::MapViewOfFile(_handle,
                     (readOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS),
                     0, 0, 0));

#else

  if(! POSIX::verifyIPCName(_name))
    throw SystemException("Invalid IPC name");

  _name.prepend(File::separator);

  mode_t mode;

  POSIX::encodePermissions(_perm, mode);

  bool init = true;

  if((_handle = ::shm_open(_name.c_str(),
                           (O_CREAT | (readOnly ? O_RDONLY : O_RDWR) | O_EXCL),
                           mode)) < 0)
  {
    if(errno == EEXIST)
    {
      init = false;
      int flags = O_CREAT | (readOnly ? O_RDONLY : O_RDWR);

      if((_handle = ::shm_open(_name.c_str(), flags, mode)) < 0)
        throw SystemException(System::getErrorString("shm_open"));
    }
    else
      throw SystemException(System::getErrorString("shm_open"));
  }

  if(init)
  {
    if(::ftruncate(_handle, static_cast<off_t>(_size)) != 0)
    {
      String error = System::getErrorString("ftruncate");
      ::shm_unlink(_name.c_str());
      throw SystemException(error);
    }
  }

  int flags = PROT_READ;
  if(! readOnly)
    flags |= PROT_WRITE;

  _base = static_cast<byte_t *>(::mmap(NULL, _size, flags, MAP_SHARED, _handle,
                                       static_cast<off_t>(0)));

  if(_base == MAP_FAILED)
    throw SystemException(System::getErrorString("mmap"));

  if(init && !readOnly)
    zero();

#endif
}

/*
 */

void SharedMemoryBlock::detach() throw()
{
  if(_base != NULL)
  {
#ifdef CCXX_OS_WINDOWS

    ::UnmapViewOfFile(_base);

    ::CloseHandle(_handle);

#else

    ::munmap(_base, _size);
    ::shm_unlink(_name.c_str());

#endif

    _base = NULL;
  }
}

/*
 */

void SharedMemoryBlock::setBase(byte_t *base) throw()
{
  // Overridden to be a no-op.
}

/*
 */

void SharedMemoryBlock::setSize(size_t size) throw()
{
  // Overridden to be a no-op.
}


}; // namespace ccxx

/* end of source file */
