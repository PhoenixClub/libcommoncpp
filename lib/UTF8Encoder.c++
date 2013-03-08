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

#include "commonc++/UTF8Encoder.h++"

#include <cstring>

#include "commonc++/CharOps.h++"

namespace ccxx {

UTF8Encoder::UTF8Encoder(const char16_t *str, size_t offset /* = 0 */,
                         size_t length /* = 0 */) throw()
  : _str(str)
  , _offset(offset)
  , _length(str == NULL ? 0 : length ? length : CharOps::length(str))
  , _inp(str + offset)
  , _left(_length)
  , _numExtra(0)
{
}

/*
 */

UTF8Encoder::~UTF8Encoder() throw()
{
}

/*
 */

size_t UTF8Encoder::encode(char *buf, size_t len) throw()
{
  char *outp = buf;
  char16_t surrogate = 0;

  while((_numExtra > 0) && (len > 0))
  {
    *outp = _extra[2 - _numExtra];
    ++outp;
    --len;
    --_numExtra;
  }

  for(; (_left > 0) && (len > 0); --_left, ++_inp)
  {
    uint32_t uchar = 0;

    // UTF16 -> UTF32
    if((*_inp & 0xFC00) == 0xD800)
    {
      surrogate = *_inp;
      continue;
    }
    else
    {
      if(surrogate != 0)
      {
        uint32_t hi = surrogate - 0xD800; // high 10 bits
        uint32_t lo = *_inp - 0xDC00; // low 10 bits
        uchar = ((hi << 10) | lo) + 0x10000;
        surrogate = 0;
      }
      else
        uchar = *_inp;
    }

    outputChar(&buf, &len, uchar);
  }

  return(outp - buf);
}

/*
 */

void UTF8Encoder::outputChar(char **buf, size_t* len, uint32_t uchar) throw()
{
  int bits = 0;

  // UTF32 -> UTF8
  if(uchar < 0x80)
  {
    // 0xxxxxxx => 0xxxxxxx
    **buf = (static_cast<char>(uchar & 0x7F));
    ++*buf;
    --*len;
    return;
  }
  else if(uchar < 0x800)
  {
    // 00000yyy yyxxxxxx => 110yyyyy 10xxxxxx
    **buf = static_cast<char>(0xC0 & ((uchar >> 6) & 0x1F));
    bits = 6;
  }
  else if(uchar < 0x10000)
  {
    // zzzzyyyy yyxxxxxx => 1110zzzz 10yyyyyy 10xxxxxx
    **buf = static_cast<char>(0xE0 & ((uchar >> 12) & 0x0F));
    bits = 12;
  }
  else if(uchar < 0x110000)
  {
    // 000wwwzz zzzzyyyy yyxxxxxx => 11110www 10zzzzzz 10yyyyyy 10xxxxxx
    **buf = static_cast<char>(0xF0 & ((uchar >> 18) & 0x07));
    bits = 18;
  }
  else
  {
    // invalid character; ignore
    return;
  }

  ++*buf;
  --*len;

  size_t tlen = 3;
  char *tbuf = _extra;

  do
  {
    bits -= 6;
    if(*len == 0)
    {
      // Switch to overflow buffer.
      buf = &tbuf;
      len = &tlen;
    }

    **buf = static_cast<char>(0x80 & ((uchar >> bits) & 0x3F));
    ++*buf;
    --*len;
  }
  while(bits > 0);

  _numExtra = 3 - tlen;
}

/*
 */

size_t UTF8Encoder::encodedLength() const throw()
{
  size_t count = 0;
  size_t len = _length;
  char16_t mask;

  for(const char16_t *p = _str + _offset; *p; ++p)
  {
    if(*p < 0x80)
      ++count;
    else if(*p < 0x800)
      count += 2;
    else
    {
      mask = (*p & 0xFC00);
      if(mask == 0xD800) // high surrogate introduces code point >= 0xFFFF
        count += 4;
      else if(mask != 0xDC00) // Don't count low surrogates
        count += 3;
    }

    if((len != 0) && (--len == 0))
      break;
  }

  return(count);
}

} // namespace ccxx
