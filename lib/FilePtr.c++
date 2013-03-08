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

#include "commonc++/FilePtr.h++"

#include <cstdio>

namespace ccxx {

/*
 */

FilePtr::FilePtr(const String& name, const String& attrs) throw()
  : _fp(NULL)
{
  _fp = std::fopen(name.c_str(), attrs.c_str());
}

/*
 */

FilePtr::FilePtr(const char *name, const char *attrs) throw()
  : _fp(NULL)
{
  _fp = std::fopen(name, attrs);
}

/*
 */

FilePtr::FilePtr(FILE *fp) throw()
  : _fp(fp)
{
}

/*
 */

FilePtr::~FilePtr() throw()
{
  if(_fp)
    std::fclose(_fp);
}


}; // namespace ccxx

/* end of source file */
