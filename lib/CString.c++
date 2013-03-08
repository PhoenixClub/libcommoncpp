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

#include "commonc++/CString.h++"

#include <cstring>

#include "commonc++/AtomicCounter.h++"

// TODO: Fix this to deal with NULL strings

namespace ccxx {

/*
 */

CString::CString(const char *str)
  : _str(str),
    _refs(new AtomicCounter(1))
{
}

/*
 */

CString::CString(const CString& other)
  : _str(other._str),
    _refs(other._refs)
{
  ++(*_refs);
}

/*
 */

CString::~CString() throw()
{
  _release();
}

/*
 */

CString& CString::operator=(const CString &other)
{
  if(other._str != _str)
  {
    _release();
    _str = other._str;
    _refs = other._refs;
    ++(*_refs);
  }

  return(*this);
}

/*
 */

size_t CString::length() const throw()
{
  return(_str ? std::strlen(_str) : 0);
}

/*
 */

void CString::_release() throw()
{
  if(--(*_refs) == 0)
  {
    delete[] _str;
    delete _refs;
  }
}


} // namespace ccxx
