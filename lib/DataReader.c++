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

#include "commonc++/DataReader.h++"
#include "commonc++/ByteOrder.h++"

namespace ccxx {

/*
 */

DataReader::DataReader() throw()
{
}

/*
 */

DataReader::~DataReader() throw()
{
}

/*
 */

DataReader & DataReader::operator>>(bool &v) throw(IOException)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(byte_t &v) throw(IOException)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(byte_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
    read(v, ctx.length);

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(char &v) throw(IOException)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(char *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
    read(reinterpret_cast<byte_t *>(v), ctx.length);

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(int16_t &v) throw(IOException)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(int16_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(int16_t));

    if(! isSameEndianness())
    {
      int16_t *end = v + ctx.length;
      for(int16_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(uint16_t &v) throw(IOException)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(uint16_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(uint16_t));

    if(! isSameEndianness())
    {
      uint16_t *end = v + ctx.length;
      for(uint16_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(int32_t &v) throw(IOException)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(int32_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(int32_t));

    if(! isSameEndianness())
    {
      int32_t *end = v + ctx.length;
      for(int32_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(uint32_t &v) throw(IOException)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(uint32_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(uint32_t));

    if(! isSameEndianness())
    {
      uint32_t *end = v + ctx.length;
      for(uint32_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(int64_t &v) throw(IOException)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(int64_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(int64_t));

    if(! isSameEndianness())
    {
      int64_t *end = v + ctx.length;
      for(int64_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(uint64_t &v) throw(IOException)
{
  read(reinterpret_cast<byte_t *>(&v), sizeof(v));
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(uint64_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    read(reinterpret_cast<byte_t *>(v), ctx.length * sizeof(uint64_t));

    if(! isSameEndianness())
    {
      uint64_t *end = v + ctx.length;
      for(uint64_t *p = v; p < end; ++p)
        ByteOrder::reverseBytes(*p);
    }
  }

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(float &v) throw(IOException)
{
  // assume IEEE-754 floating point representation
  union
  {
    float f;
    uint32_t i;
  } un;

  operator>>(un.i);
  v = un.f;

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(float *v) throw(IOException)
{
  // assume IEEE-754 floating point representation
  return(operator>>(reinterpret_cast<uint32_t *>(v)));
}

/*
 */

DataReader & DataReader::operator>>(double &v) throw(IOException)
{
  // assume IEEE-754 floating point representation
  union
  {
    double d;
    uint64_t i;
  } un;

  operator>>(un.i);
  v = un.d;

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(double *v) throw(IOException)
{
  // assume IEEE-754 floating point representation
  return(operator>>(reinterpret_cast<uint64_t *>(v)));
}

/*
 */

DataReader & DataReader::operator>>(Blob &v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length == 0)
    v = Blob::null;
  else
  {
    v.setLength(ctx.length);
    read(v.getData(), ctx.length);
  }

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(String &v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length == 0)
    v = String::null;
  else
  {
    if((ctx.limit - ctx.offset) < static_cast<int64_t>(ctx.length))
      throw IOException("not enough bytes remaining before limit");

    v = "";
    byte_t buf[256];
    size_t count = ctx.length;

    while(count > 0)
    {
      size_t n = std::min(count, sizeof(buf));
      read(buf, n);
      v.append(reinterpret_cast<const char *>(buf), n);
      count -= n;
    }

    if(ctx.stringPadding)
      v.trim();
  }

  return(*this);
}

/*
 */

DataReader & DataReader::operator>>(const Manipulator &manip)
  throw(IOException)
{
  manip.apply(*this);

  return(*this);
}


} // namespace ccxx

/* end of source file */
