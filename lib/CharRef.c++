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

#include "commonc++/CharRef.h++"
#include "commonc++/UChar.h++"
#include "commonc++/UString.h++"

namespace ccxx {

/*
 */

CharRef::CharRef(UString &str, uint_t index) throw()
  : _str(str),
    _index(index)
{
}

/*
 */

CharRef::~CharRef() throw()
{
}

/*
 */

CharRef& CharRef::operator=(const UChar &c) throw(OutOfBoundsException)
{
  _str.setCharAt(_index, c);

  return(*this);
}

/*
 */

bool CharRef::operator==(const UChar &other) const throw()
{
  return(_str[_index] == other);
}



} // namespace ccxx
