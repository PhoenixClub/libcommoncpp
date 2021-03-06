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

#include "commonc++/TempFile.h++"
#include "commonc++/DynamicArray.h++"
#include "commonc++/File.h++"
#include "commonc++/System.h++"

#include <cstdlib>
#include <iostream>
#include <cstring>

#ifdef CCXX_OS_POSIX
#include <unistd.h>
#endif

#ifdef CCXX_OS_WINDOWS
#include <io.h>
#endif

namespace ccxx {

/*
 */

TempFile::TempFile(const String &path /* = String::null */)
  : Stream(),
    _path(path)
{
}

/*
 */

TempFile::~TempFile() throw()
{
}

/*
 */

void TempFile::open() throw(IOException)
{
  if(isOpen())
    return;

  String path;

  if(_path.isNull())
    path = System::getTempDir();
  else
  {
    FileName fn(_path);
    path = fn.getDirectory();
  }

  path << File::separator << "ccxxtmp-XXXXXX";

  DynamicArray<char> buf(path.getLength() + 1);
  std::strcpy(buf.data(), path.c_str());

#ifdef CCXX_OS_WINDOWS

  if(! _mktemp(buf.data()))
    throw IOException(System::getErrorString("_mktemp"));

  FileHandle handle = ::CreateFile(buf, FILE_READ_DATA | FILE_WRITE_DATA, 0,
                                   NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,
                                   NULL);
  if(handle == INVALID_HANDLE_VALUE)
    throw IOException(System::getErrorString("CreateFile"));

#else

  int handle = ::mkstemp(buf.data());

  if(handle < 0)
    throw IOException(System::getErrorString("mkstemp"));

#endif

  _tempPath = buf.data();

  Stream::_init(handle, true, true, true);
}

/*
 */

void TempFile::close() throw()
{
  if(isOpen())
  {
    Stream::close();

    if(! _path.isNull())
      // Move the temp file onto the shadowed file.
      File::rename(_tempPath, _path);
    else
      File::remove(_tempPath);
  }
}


}; // namespace ccxx

/* end of source file */
