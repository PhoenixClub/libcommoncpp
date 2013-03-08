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

#include "commonc++/UTF8Decoder.h++"

#include <cstring>

namespace ccxx {

/*
 */

UTF8Decoder::UTF8Decoder(const char *str, size_t offset /* = 0 */,
                         size_t length /* = 0 */) throw()
  : _str(str)
  , _offset(offset)
  , _length((length ? length : std::strlen(str)))
  , _inp(str + offset)
  , _left(str == NULL ? 0 : _length)
  , _extra(0)
  , _needed(0)
{
}

/*
 */

UTF8Decoder::~UTF8Decoder() throw()
{
}

/*
 */

size_t UTF8Decoder::decode(char16_t *buf, size_t len) throw()
{
  char16_t *outp = buf;
  uint32_t uchar = 0;

  if((_left == 0) || (buf == NULL) || (len == 0))
    return(0);

  if(_extra != 0)
  {
    *outp = _extra;
    ++outp;
    --len;
    _extra = 0;
  }

  for(; (_left > 0) && (len > 0); --_left, ++_inp)
  {
    if(_needed == 0)
    {
      if((*_inp & 0x80) == 0)
      {
        uchar = *_inp;
        if(!outputChar(&outp, &len, uchar))
          break;
      }
      else if((*_inp & 0xE0) == 0xC0)
      {
        _needed = 1;
        uchar = (*_inp & 0x1F);
      }
      else if((*_inp & 0xF0) == 0xE0)
      {
        _needed = 2;
        uchar = (*_inp & 0x0F);
      }
      else if((*_inp & 0xF8) == 0xF0)
      {
        _needed = 3;
        uchar = (*_inp & 0x07);
      }
      else
      {
        // TODO: ERROR!
      }
    }
    else
    {
      if((*_inp & 0xC0) == 0x80)
      {
        uchar <<= 6;
        uchar |= (*_inp & 0x3F);
        if(--_needed == 0)
        {
          if(!outputChar(&outp, &len, uchar))
            break;
        }
      }
      else
      {
        // TODO: ERROR!
      }
    }
  }

  return(outp - buf);
}

/*
 */

bool UTF8Decoder::outputChar(char16_t **buf, size_t *len, uint32_t uchar)
  throw()
{
  if(uchar < 0x10000)
  {
    **buf = static_cast<char16_t>(uchar & 0xFFFF);
    ++*buf;
    --*len;
  }
  else if((uchar >= 0x10000) && (uchar < 0x110000))
  {
    // surrogate pair
    uchar -= 0x10000;
    char16_t lo = static_cast<char16_t>(0xDC00 + (uchar & 0x3FF));
    uchar >>= 10;
    **buf = static_cast<char16_t>(0xD800 + (uchar & 0x3FF));
    ++*buf;
    if(--*len == 0)
    {
      _extra = lo;
      return(false);
    }
    else
    {
      **buf = lo;
      ++*buf;
      --*len;
    }
  }
  else
  {
    // invalid character; ignore
  }

  return(true);
}

/*
 */

size_t UTF8Decoder::decodedLength() const throw()
{
  size_t count = 0;
  size_t len = _length;

  for(const char *p = _str + _offset; *p; ++p)
  {
    if((*p & 0xC0) != 0x80)
    {
      ++count;

      if((*p & 0xF0) == 0xF0)
        ++count; // code point > 0xFFFF
    }

    if((len != 0) && (--len == 0))
      break;
  }

  return(count);
}


} // namespace ccxx
