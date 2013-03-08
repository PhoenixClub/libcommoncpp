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

#include "commonc++/CharsetConverter.h++"

#include <iconv.h>

namespace ccxx {

/*
 */

CharsetConverter::CharsetConverter(const char *fromCode, const char *toCode)
  : _iconv(NULL)
{
  iconv_t cd = ::iconv_open(toCode, fromCode);
  if(cd != (iconv_t)-1)
    _iconv = cd;
}

/*
 */

CharsetConverter::~CharsetConverter()
{
  if(_iconv)
    ::iconv_close(_iconv);
}

/*
 */

size_t CharsetConverter::convert(const byte_t **inBuf, size_t *inBytesLeft,
                                 byte_t **outBuf, size_t *outBytesLeft) throw()
{
  if(! _iconv)
    return(0);

  return(::iconv(_iconv, (char **)(inBuf), inBytesLeft,
                 (char **)(outBuf), outBytesLeft));
}

/*
 */

void CharsetConverter::reset() throw()
{
  if(_iconv)
    ::iconv(_iconv, NULL, NULL, NULL, NULL);
}


}; // namespace ccxx

/* end of source file */
