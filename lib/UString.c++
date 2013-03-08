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

#include "commonc++/UString.h++"
#include "commonc++/CharOps.h++"
#include "commonc++/UTF8Decoder.h++"
#include "commonc++/UTF8Encoder.h++"

#include <cstdio>
#include <cstring>
#include <ostream>

namespace ccxx {

/*
 */

const uint_t UString::END = ~0;

const UString UString::null = UString();

const UString UString::empty = "";

const UString UString::whitespaceChars = " \t\f\r\n\v";

const UStringVec UStringVec::emptyVec;

/*
 */

class UString::StringBuf
{
  friend class UString;

  protected:

  char16_t* _data;
  uint_t _capacity;
  uint_t _length;
  AtomicCounter _refs;

  StringBuf(const StringBuf& other, uint_t length);
  StringBuf(char16_t* str = NULL, uint_t offset = 0, uint_t length = 0,
            bool copy = true);
  StringBuf(const char16_t* str, uint_t offset = 0, uint_t length = 0);
  ~StringBuf() throw();

  void reserve(uint_t size);
  void compact();
  void makeNull();
  void makeCopy(const char16_t* str, uint_t offset, uint_t length);

  private:

  CCXX_COPY_DECLS(StringBuf);
};

/*
 */

UString::UString()
  : _buf(new StringBuf())
{
}

/*
 */

UString::UString(char16_t *str, uint_t offset /* = 0 */,
                 uint_t count /* = 0 */,
                 bool copy /* = true */)
  : _buf(new StringBuf(str, offset, count, copy))
{
}

UString::UString(const char16_t *str, uint_t offset /* = 0 */,
                 uint_t count /* = 0 */)
  : _buf(new StringBuf(str, offset, count))
{
}

UString::UString(const char *str, uint_t offset /* = 0 */,
                 uint_t count /* = 0 */)
  : _buf(new StringBuf())
{
  _fromUTF8(str, offset, count);
}

/*
 */

UString::UString(const UString& other)
  : _buf(NULL)
{
  if(&other == &UString::null)
    _buf = new StringBuf();
  else if(other._buf->_refs == -1)
    _buf = new StringBuf(*other._buf, other._buf->_length);
  else
  {
    ++(other._buf->_refs);
    _buf = other._buf;
  }
}

/*
 */

UString& UString::operator=(const char *other)
{
  _release();
  _buf = new StringBuf();

  if(other != NULL)
    _fromUTF8(other, 0, 0);

  return(*this);
}

/*
 */

UString& UString::operator=(const UString &other)
{
  if(&other != this)
  {
    _release();

    if(&other == &UString::null)
      _buf = new StringBuf();
    else
    {
      if(other._buf->_refs == -1) // unshareable
        _buf = new StringBuf(other._buf->_data, 0, other._buf->_length);
      else
      {
        ++(other._buf->_refs);
        _buf = other._buf;
      }
    }
  }

  return(*this);
}

/*
 */

UString::~UString() throw()
{
  _release();
}

/*
 */

bool UString::endsWith(const UString &str, bool ignoreCase /* = false */) const
  throw()
{
  uint_t len = str.getLength();

  if(_buf->_length < len)
    return(false);

  if(len == 0)
    return(true);

  int r = CharOps::compare(_buf->_data + _buf->_length - len, str._buf->_data,
                           ignoreCase);

  return(r == 0);
}

/*
 */

bool UString::endsWith(const UChar &c, bool ignoreCase /* = false */) const
  throw()
{
  if(isEmpty())
    return(false);

  if(ignoreCase)
    return(UChar::toLowerCase(*(_buf->_data + _buf->_length - 1))
           == c.toLowerCase());
  else
    return(*(_buf->_data + _buf->_length - 1) == c);
}

/*
 */

bool UString::endsWithAnyOf(const UString& chars,
                           bool ignoreCase /* = false */) const throw()
{
  if(isEmpty())
    return(false);

  char16_t c = *(_buf->_data + _buf->_length - 1);
  bool matched = false;

  for(const char16_t* p = chars.data(); *p; ++p)
  {
    matched = (CharOps::compare(*p, c, ignoreCase) == 0);
    if(matched)
      break;
  }

  return(matched);
}

/*
 */

bool UString::startsWith(const UString &str, bool ignoreCase /* = false */) const
  throw()
{
  uint_t len = str.getLength();

  if(_buf->_length < len)
    return(false);

  if(len == 0)
    return(true);

  return(CharOps::compare(_buf->_data, str._buf->_data, len, ignoreCase) == 0);
}

/*
 */

bool UString::startsWith(const UChar &c, bool ignoreCase /* = false */) const
  throw()
{
  if(isEmpty())
    return(false);

  return(CharOps::compare(*(_buf->_data), c, ignoreCase) == 0);
}

/*
 */

bool UString::startsWithAnyOf(const UString& chars,
                             bool ignoreCase /* = false */) const throw()
{
  if(isEmpty())
    return(false);

  char16_t c = *(_buf->_data);
  bool matched = false;

  for(const char16_t* p = chars.data(); *p; ++p)
  {
    matched = (CharOps::compare(*p, c, ignoreCase) == 0);
    if(matched)
      break;
  }

  return(matched);
}

/*
 */

uint_t UString::getLength() const throw()
{
  return(_buf == NULL ? 0 : _buf->_length);
}

/*
 */

void UString::setLength(uint_t length)
{
  if(length < _buf->_length)
  {
    _makeCopy(length + 1);

    _buf->_length = length;
    *(_buf->_data + _buf->_length) = UChar::nul;
  }
}

/*
 */

void UString::reserve(uint_t capacity)
{
  _buf->reserve(capacity + 1);
}

/*
 */

uint_t UString::getCapacity() const throw()
{
  return(_buf->_capacity);
}

/*
 */

void UString::compact()
{
  // TODO
}

/*
 */

bool UString::isEmpty() const throw()
{
  return((_buf->_data == NULL) || (_buf->_length == 0));
}

/*
 */

bool UString::isNull() const throw()
{
  return(_buf->_data == NULL);
}

/*
 */

bool UString::isInteger() const throw()
{
  if(isEmpty())
    return(false);

  bool first = true;
  for(const char16_t* p = _buf->_data; *p; p++)
  {
    if(! (UChar::isDigit(*p) || (first && UChar::isSign(*p))))
      return(false);

    first = false;
  }

  return(true);
}

/*
 */

int UString::compareTo(const UString& other, bool ignoreCase /* = false */) const
  throw()
{
  if(_buf == other._buf)
    return(0);

  // null is "less than" non-null

  if(isNull())
    return(other.isNull() ? 0 : -1);

  if(other.isNull())
    return(1);

  return(CharOps::compare(_buf->_data, other._buf->_data, ignoreCase));
}

/*
 */

UString& UString::trim(const UString &chars /* = whitespaceChars */,
                     bool begin /* = true */,
                     bool end /* = true */)
{
  if(! isEmpty())
  {
    char16_t* p = _buf->_data, *q = p + _buf->_length - 1;

    // trim from beginning

    if(begin)
      for(; *p && CharOps::find(chars.data(), *p); ++p) {}

    // trim from end

    if(end)
      for(; CharOps::find(chars.data(), *q) && (q >= p); --q) {}

    uint_t nl = (q - p + 1);

    if(nl != _buf->_length)
    {
      _makeCopy(nl + 1);

      CharOps::move(_buf->_data, p, nl);
      _buf->_length = nl;
      *(_buf->_data + _buf->_length) = UChar::nul;
    }
  }

  return(*this);
}

/*
 */

UString& UString::chop(const UChar &c)
{
  if((! isEmpty()) && (! c.isNul()))
  {
    char16_t* p = CharOps::find(_buf->_data, c);
    if(p != NULL)
    {
      uint_t nl = (p - _buf->_data);

      _makeCopy(nl + 1);

      _buf->_length = nl;
      *(_buf->_data + _buf->_length) = UChar::nul;
    }
  }

  return(*this);
}

/*
 */

UString& UString::reverseChop(const UChar &c)
{
  if(! isEmpty() && (! c.isNul()))
  {
    char16_t* p = CharOps::findLast(_buf->_data, c);
    if(p != NULL)
    {
      uint_t nl = (p - _buf->_data);

      _makeCopy(nl + 1);

      _buf->_length = nl;
      *(_buf->_data + _buf->_length) = UChar::nul;
    }
  }

  return(*this);
}

/*
 */

UString& UString::replace(const UChar &a, const UChar &b)
{
  if((! isEmpty()) && (! a.isNul()) && (! b.isNul()) && (a != b))
  {
    char16_t *p = CharOps::find(_buf->_data, a);
    if(p != NULL)
    {
      _makeCopy(_buf->_capacity);

      p = CharOps::find(_buf->_data, a);

      for(char16_t *q = p; *q; q++)
      {
        if(*q == a)
          *q = b;
      }
    }
  }

  return(*this);
}

/*
 */

UString& UString::replace(const UString &a, const UString &b)
{
  if(!isEmpty() && !a.isNull() && !b.isNull())
  {
    char16_t* p = CharOps::find(_buf->_data, a.data());

    if(p)
    {
      uint_t l = a.length();
      UString copy;
      char16_t *q = _buf->_data;

      do
      {
        if(p != q)
          copy.append(q, (p - q));
        q = p + l;
        copy.append(b);
      }
      while((p = CharOps::find(q, a.data())) != NULL);

      if(! UChar::isNul(*q))
        copy.append(q);

      *this = copy;
    }
  }

  return(*this);
}


/*
 */

CharRef UString::operator[](int index) throw(OutOfBoundsException)
{
  if(isNull() || (index < 0))
    throw OutOfBoundsException();

  uint_t ind = static_cast<uint_t>(index);

  bool extend = false;
  if(ind >= _buf->_length)
  {
    ind = _buf->_length;
    extend = true;
  }

  _makeCopy(std::max(_buf->_length, static_cast<uint_t>(ind + 1)), true);

  if(extend)
  {
    ++_buf->_length;
    *(_buf->_data  + _buf->_length) = UChar::nul;
  }

  return(CharRef(*this, ind));
}

/*
 */

const UChar UString::operator[](int index) const throw(OutOfBoundsException)
{
  if(isNull() || (index < 0))
    throw OutOfBoundsException();

  uint_t ind = static_cast<uint_t>(index);

  if(ind >= _buf->_length)
    throw OutOfBoundsException();

  return(UChar(*(_buf->_data + ind)));
}

/*
 */

char16_t* UString::_makeGap(uint_t index, uint_t gaplen)
{
  if((index > _buf->_length) || (index == END))
    index = static_cast<uint_t>(_buf->_length);

  _makeCopy(_buf->_length + gaplen + 1);

  uint_t movelen = _buf->_length - index;

  if(movelen > 0)
    CharOps::move(_buf->_data + index + gaplen, _buf->_data + index, movelen);

  _buf->_length += gaplen;
  *(_buf->_data + _buf->_length) = UChar::nul;

  return(_buf->_data + index);
}

/*
 */

UString& UString::insert(const UChar &c, uint_t index /* = 0 */)
{
  if(! c.isNul())
  {
    char16_t *gap = _makeGap(index, 1);
    *gap = c;
  }

  return(*this);
}

/*
 */

void UString::_insert(const char16_t *str, uint_t length, uint_t index,
                      uint_t count)
{
  if((count == 0) || (count > length))
    count = length;

  if(count > 0)
  {
    char16_t *gap = _makeGap(index, count);
    CharOps::copy(gap, str, count);
  }
}

/*
 */

UString& UString::insert(const UString &str, uint_t index /* = 0 */,
                         uint_t count /* = 0 */)
{
  if(!str.isNull())
    _insert(str.data(), str.length(), index, count);

  return(*this);
}

/*
 */

UString& UString::insert(const char16_t *str, uint_t index /* = 0 */,
                         uint_t count /* = 0 */)
{
  if(str != NULL)
    _insert(str, CharOps::length(str), index, count);

  return(*this);
}

/*
 */

UString& UString::remove(uint_t fromIndex, uint_t toIndex /* = END */)
{
  if(! isEmpty())
  {
    if(toIndex == END)
      toIndex = static_cast<uint_t>(_buf->_length) + 1;
    if(fromIndex == END)
      fromIndex = static_cast<uint_t>(_buf->_length) + 1;

    if(toIndex > fromIndex)
    {
      uint_t len = toIndex - fromIndex - 1;

      CharOps::move(_buf->_data + fromIndex, _buf->_data + toIndex,
                    (_buf->_length - toIndex + 1));

      _buf->_length -= len;
      *(_buf->_data + _buf->_length) = UChar::nul;
    }
  }

  return(*this);
}

/*
 */

UString& UString::setCharAt(uint_t index, const UChar &c)
  throw(OutOfBoundsException)
{
  if(isNull() || (index < 0))
    throw OutOfBoundsException();

  uint_t ind = static_cast<uint_t>(index);

  if(ind >= _buf->_length)
    throw OutOfBoundsException();

  *(_buf->_data) = c;

  return(*this);
}

/*
 */

uint_t UString::hash(uint_t modulo /* = 256 */) const throw()
{
  uint32_t hash = 0;

  if(! isEmpty())
    hash = Hash::SDBMHash(reinterpret_cast<const byte_t *>(_buf->_data),
                          _buf->_length * sizeof(char16_t));

  return(hash % modulo);
}

/*
 */

UString UString::substring(uint_t fromIndex, uint_t toIndex /* = END */) const
{
  if(isNull() || (fromIndex == toIndex))
    return(UString::empty);

  if(fromIndex > toIndex)
    fromIndex = toIndex;

  UString substr(_buf->_data, fromIndex, (toIndex - fromIndex));

  return(substr);
}

/*
 */

int UString::indexOf(const UChar &c, uint_t fromIndex /* = 0 */) const throw()
{
  if(isNull() || (fromIndex >= _buf->_length) || c.isNul())
    return(-1);

  int index = fromIndex;

  for(const char16_t* p = _buf->_data + fromIndex; *p; p++, index++)
    if(*p == c)
      return(index);

  return(-1);
}

/*
 */

int UString::indexOfAnyOf(const UString& chars, uint_t fromIndex /* = 0 */) const
  throw()
{
  if(isNull() || (fromIndex >= _buf->_length) || (chars == NULL))
    return(-1);

  int index = fromIndex;

  for(const char16_t* p = _buf->_data + fromIndex; *p; p++, index++)
    if(CharOps::find(chars.data(), *p) != NULL)
      return(index);

  return(-1);
}

/*
 */

int UString::indexOf(const UString& str, uint_t fromIndex /* = 0 */) const
  throw()
{
  if(isEmpty() || str.isNull() || (fromIndex >= _buf->_length))
    return(-1);

  const char16_t* p = CharOps::find(_buf->_data + fromIndex, str.data());

  return(p ? static_cast<int>(p - _buf->_data) : -1);
}

/*
 */

int UString::lastIndexOf(const UChar &c, uint_t fromIndex /* = END */) const
  throw()
{
  if(isEmpty() || c.isNul())
    return(-1);

  if((fromIndex >= _buf->_length) || (fromIndex == END))
    fromIndex = _buf->_length - 1;

  int index = fromIndex;

  for(const char16_t* p = _buf->_data + fromIndex; index >= 0; index--, p--)
    if(*p == c)
      return(index);

  return(-1);
}

/*
 */

int UString::lastIndexOfAnyOf(const UString& chars, uint_t fromIndex /* = END */)
  const throw()
{
  if(isEmpty() || (chars == NULL))
    return(-1);

  if((fromIndex >= _buf->_length) || (fromIndex == END))
    fromIndex = _buf->_length - 1;

  int index = fromIndex;

  for(const char16_t* p = _buf->_data + fromIndex; index >= 0; index--, p--)
    if(CharOps::find(chars.data(), *p) != NULL)
      return(index);

  return(-1);
}

/*
 */

int UString::lastIndexOf(const UString& str, uint_t fromIndex /* = END */) const
  throw()
{
  if(isEmpty() || (str == NULL))
    return(-1);

  if(_buf->_length < str.length())
    return(-1);

  if((fromIndex >= _buf->_length) || (fromIndex == END))
    fromIndex = _buf->_length - 1;

  char16_t* p = CharOps::findLast(_buf->_data, str.data(), fromIndex);

  return(p ? (p - _buf->_data) : -1);
}

/*
 */

UString UString::nextToken(uint_t &fromIndex,
                         const UString& delim /* = whitespaceChars */) const
{
  if(isNull() || (fromIndex >= _buf->_length) || delim.isNull())
    return(UString::null);

  char16_t *p, *q;

  for(p = _buf->_data + fromIndex; *p; p++)
  {
    if(! CharOps::find(delim.data(), *p))
      break;
  }

  if(UChar::isNul(*p))
    return(UString::null);

  for(q = p + 1; *q; q++)
  {
    if(CharOps::find(delim.data(), *q))
      break;
  }

  fromIndex = static_cast<uint_t>(q - _buf->_data);

  UString tok(p, 0, static_cast<uint_t>(q - p));

  return(tok);
}

/*
 */

uint_t UString::split(UStringVec& tokens,
                     const UString& delim /* = whitespaceChars */,
                     uint_t limit /* = 0 */) const
{
  uint_t index = 0, count = 0;

  for(;;)
  {
    UString s = nextToken(index, delim);
    if(s.isNull())
      break;

    tokens << s;
    ++count;

    if(limit && (count == limit))
      break;
  }

  return(count);
}

/*
 */

UString& UString::toLowerCase()
{
  if(! isEmpty())
  {
    _makeCopy(_buf->_capacity);

    for(char16_t* p = _buf->_data; *p; p++)
      *p = UChar::toLowerCase(*p);
  }

  return(*this);
}

/*
 */

UString& UString::toUpperCase()
{
  if(! isEmpty())
  {
    _makeCopy(_buf->_capacity);

    for(char16_t* p = _buf->_data; *p; p++)
      *p = UChar(*p).toUpperCase();
  }

  return(*this);
}

/*
 */

UString& UString::fill(const UChar &c, uint_t count)
{
  if((! c.isNul()) && (count > 0))
  {
    _makeCopy(count + 1);

    uint_t n = count;
    for(char16_t *p = _buf->_data; n; ++p, --n)
      *p = c;

    _buf->_length = count;
    *(_buf->_data + _buf->_length) = UChar::nul;
  }

  return(*this);
}

/*
 */

UString& UString::pad(const UChar &c, uint_t count)
{
  if((! c.isNul()) && (count > 0))
  {
    _makeCopy(_buf->_length + count + 1);

    uint_t n = count;
    for(char16_t* p = _buf->_data + _buf->_length; n; ++p, --n)
      *p = c;

    _buf->_length += count;
    *(_buf->_data + _buf->_length) = UChar::nul;
  }

  return(*this);
}

/*
 */

bool UString::toBool(uint_t index /* = 0 */) const throw()
{
  if(isNull() || (index >= _buf->_length))
    return(false);

  return(! CharOps::compare(_buf->_data + index, CharOps::TRUE_REP, true));
}

/*
 */

int UString::toInt(uint_t index /* = 0 */, uint_t base /* = 10 */) const throw()
{
  if((base < 2) || (base > 36) || isNull() || (index >= _buf->_length))
    return(0);

  return(CharOps::toLong(_buf->_data + index, base));
}

/*
 */

uint_t UString::toUInt(uint_t index /* = 0 */, uint_t base /* = 10 */) const
  throw()
{
  int v = toInt(index, base);
  return(static_cast<uint_t>(v < 0 ? 0 : v));
}

/*
 */

int64_t UString::toInt64(uint_t index /* = 0 */, uint_t base /* = 10 */) const
  throw()
{
  if((base < 2) || (base > 36) || isNull() || (index >= _buf->_length))
    return(0);

  return(CharOps::toLongLong(_buf->_data + index, base));
}

/*
 */

uint64_t UString::toUInt64(uint_t index /* = 0 */, uint_t base /* = 10 */) const
  throw()
{
  if((base < 2) || (base > 36) || isNull() || (index >= _buf->_length))
    return(0);

  return(CharOps::toULongLong(_buf->_data + index, base));
}

/*
 */

float UString::toFloat(uint_t index /* = 0 */) const throw()
{
  if(isNull() || (index >= _buf->_length))
    return(0.0f);

  return(CharOps::toFloat(_buf->_data + index));
}

/*
 */

double UString::toDouble(uint_t index /* = 0 */) const throw()
{
  if(isNull() || (index >= _buf->_length))
    return(0.0);

  return(CharOps::toDouble(_buf->_data + index));
}

/*
 */

UString::operator const void *() const throw()
{
  return(_buf == NULL ? NULL : _buf->_data);
}

/*
 */

const char16_t* UString::data() const throw()
{
  return(_buf == NULL ? NULL : _buf->_data);
}

/*
 */

CString UString::toUTF8() const
{
  char *str = NULL;

  if(!isNull())
  {
    UTF8Encoder encoder(_buf->_data, 0, 0);
    const size_t len = encoder.encodedLength();
    str = new char[len + 1];
    encoder.encode(str, len);
    *(str + len) = 0;
  }

  return(CString(str));
}

/*
 */

UString& UString::append(bool v)
{
  return(insert((v ? CharOps::TRUE_REP : CharOps::FALSE_REP), END));
}

/*
 */

UString& UString::append(int val)
{
  char buf[16];

  ::snprintf(buf, CCXX_LENGTHOF(buf), "%d", val);
  append(UString(buf));

  return(*this);
}

/*
 */

UString& UString::append(int val, uint_t base)
{
  if((base >= 2) && (base <= 36))
  {
    char buf[34];
    bool neg = false;
    int i = 33;
    buf[i] = UChar::nul;

    if(val < 0)
    {
      neg = true;
      val = -val;

      if(val < 0) // int overflow
      {
        buf[--i] = CharOps::DIGITS[-(val + base) % base];
        val = -(val / base);
      }
    }

    do
    {
      buf[--i] = CharOps::DIGITS[val % base];
      val /= base;
    }
    while(val > 0);

    if(neg)
      buf[--i] = '-';

    append(UString(buf));
  }

  return(*this);
}

/*
 */

UString& UString::append(uint_t val)
{
  char buf[16];

  ::snprintf(buf, CCXX_LENGTHOF(buf), "%u", val);
  append(UString(buf));

  return(*this);
}

/*
 */

UString& UString::append(uint_t val, uint_t base)
{
  if((base >= 2) && (base <= 36))
  {
    char buf[34];
    int i = 33;
    buf[i] = UChar::nul;

    do
    {
      buf[--i] = CharOps::DIGITS[val % base];
      val /= base;
    }
    while(val > 0);

    append(UString(buf));
  }

  return(*this);
}

/*
 */

UString& UString::append(const int64_t& val)
{
  char buf[32];

  ::snprintf(buf, CCXX_LENGTHOF(buf), "%lld", val);
  append(UString(buf));

  return(*this);
}

/*
 */

UString& UString::append(const uint64_t& val)
{
  char buf[32];

  ::snprintf(buf, CCXX_LENGTHOF(buf), "%llu", val);
  append(UString(buf));

  return(*this);
}

/*
 */

UString& UString::append(const double& val, uint_t precision /* = 6 */)
{
  char buf[64];

  ::snprintf(buf, CCXX_LENGTHOF(buf), "%.*f", precision, val);
  append(UString(buf));

  return(*this);
}

/*
 */

void UString::_release()
{
  if(--(_buf->_refs) <= 0)
    delete _buf;
}

/*
 */

void UString::_makeCopy(uint_t size, bool unshareable /* = false */)
{
  if(_buf->_refs > 1) // and implicitly != -1 (unshareable)
  {
    StringBuf* buf = new StringBuf(*_buf, size);

    if(--(_buf->_refs) < 1)
      delete buf; // in case two threads are doing this at the same time
    else
      _buf = buf;
  }
  else
    _buf->reserve(size);

  _buf->_refs = (unshareable ? -1 : 1);
}

/*
 */

UString::StringBuf::StringBuf(const StringBuf& other, uint_t length)
  : _data(NULL), _capacity(0), _length(0), _refs(1)
{
  reserve(std::max(other._length, length) + 1);

  if(other._data != NULL)
    CharOps::copyRaw(_data, other._data, other._length);

  _length = other._length;
  *(_data + _length) = UChar::nul;
}

/*
 */

UString::StringBuf::StringBuf(const char16_t* str,
                             uint_t offset /* = 0 */,
                             uint_t length /* = 0 */)
  : _data(NULL), _capacity(0), _length(0), _refs(1)
{
  makeCopy(str, offset, length);
}

/*
 */

UString::StringBuf::StringBuf(char16_t* str /* = NULL */,
                             uint_t offset /* = 0 */,
                             uint_t length /* = 0 */,
                             bool copy /* = true */)
  : _data(NULL), _capacity(0), _length(0), _refs(1)
{
  if(! copy || (str == NULL))
  {
    _data = str;
    _length = (str == NULL ? 0 : CharOps::length(str));
    _capacity = (str == NULL ? 0 : _length + 1);
  }
  else
    makeCopy(str, offset, length);
}

/*
 */

UString::StringBuf::~StringBuf() throw()
{
  delete[] _data;
}

/*
 */

void UString::StringBuf::makeCopy(const char16_t* str, uint_t offset,
                                 uint_t length)
{
  if(str != NULL)
  {
    uint_t len = CharOps::length(str);

    if(length == 0)
      length = len - offset;
    else
      length = std::min(length, len - offset);

    reserve(length + 1);
    CharOps::copyRaw(_data, str + offset, length);
    *(_data + length) = UChar::nul;
    _length = length;
  }
}

/*
 */

void UString::StringBuf::reserve(uint_t size)
{
  if(_capacity < size)
  {
    uint_t newsz = (_capacity << 1) - (_capacity >> 1); // _capacity * 1.5
    uint_t needed = std::max<uint_t>(newsz, size);
    uint_t newcap = (needed > 0 ? 4 * ((needed - 1) / 4 + 1) : 0);
    char16_t* newdata = (newcap > 0 ? new char16_t[newcap] : NULL);

    if(newdata != NULL)
    {
      if(_data != NULL)
        CharOps::copyRaw(newdata, _data, _length);
    }

    if(_data != NULL)
      delete[] _data;

    _data = newdata;
    _capacity = newcap;
  }
}

/*
 */

void UString::StringBuf::compact()
{
  if(_data)
  {
    uint_t needed = _length + 1;
    if(_capacity > needed)
    {
      char16_t *newdata = new char16_t[needed];
      CharOps::copyRaw(newdata, _data, _length);
      newdata[_length] = UChar::nul;

      delete[] _data;
      _data = newdata;

      _capacity = needed;
    }
  }
}

/*
 */

void UString::StringBuf::makeNull()
{
  if(_data)
  {
    delete[] _data;
    _data = NULL;
  }

  _length = 0;
  _capacity = 0;
}

/*
 */

void UString::write(std::ostream& stream) const
{
  if(!isNull())
  {
    CString cs = toUTF8();
    stream.write(cs.data(), cs.length());
  }
  else
    stream << "(null)";
}

/*
 */

UString UString::operator+(const UString& str)
{
  UString s = *this;
  s += str;

  return(s);
}

/*
 */

UString UString::operator+(const char *str)
{
  UString s = *this;
  s += str;

  return(s);
}

/*
 */

UString UString::operator+(const UChar &c)
{
  UString s = *this;
  s += c;

  return(s);
}

/*
 */

void UString::_fromUTF8(const char *str, uint_t offset, uint_t count)
{
  UTF8Decoder decoder(str, offset, count);
  uint_t enclen = decoder.decodedLength();
  reserve(enclen);
  _buf->_length = decoder.decode(_buf->_data, enclen);
  *(_buf->_data + _buf->_length) = UChar::nul;
}


} // namespace blip
