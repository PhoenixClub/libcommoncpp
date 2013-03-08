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

#include "commonc++/DataWriter.h++"
#include "commonc++/ByteOrder.h++"

namespace ccxx {

/*
 */

DataWriter::DataWriter() throw()
{
}

/*
 */

DataWriter::~DataWriter() throw()
{
}

/*
 */

DataWriter & DataWriter::operator<<(bool v) throw(IOException)
{
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(byte_t v) throw(IOException)
{
  write(&v, sizeof(v));

  return(*this);
}


/*
 */

DataWriter & DataWriter::operator<<(const byte_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
    write(v, ctx.length);

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(char v) throw(IOException)
{
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const char *v) throw(IOException)
{
  const Context &ctx = currentContext();

  size_t vlen = CharTraits::length(v);
  size_t wlen = (ctx.length > 0 ? ctx.length : vlen);

  write(reinterpret_cast<const byte_t *>(v), vlen);

  if((ctx.length > 0) && (vlen < wlen))
    skip(wlen - vlen, ctx.stringPadding ? ' ' : 0);

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(int16_t v) throw(IOException)
{
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const int16_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if( isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v), ctx.length * sizeof(int16_t));
    else
    {
      const int16_t *end = v + (ctx.length * sizeof(int16_t));
      for(const int16_t *p = v; v < end; ++v)
      {
        int16_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(t), sizeof(int16_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(uint16_t v) throw(IOException)
{
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const uint16_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if( isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v),
            ctx.length * sizeof(uint16_t));
    else
    {
      const uint16_t *end = v + (ctx.length * sizeof(uint16_t));
      for(const uint16_t *p = v; v < end; ++v)
      {
        uint16_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(t), sizeof(uint16_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(int32_t v) throw(IOException)
{
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const int32_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if( isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v), ctx.length * sizeof(int32_t));
    else
    {
      const int32_t *end = v + (ctx.length * sizeof(int32_t));
      for(const int32_t *p = v; v < end; ++v)
      {
        int32_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(&t), sizeof(int32_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(uint32_t v) throw(IOException)
{
  if(! isSameEndianness())
    ByteOrder::reverseBytes(v);
  write(reinterpret_cast<const byte_t *>(&v), sizeof(v));

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const uint32_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if( isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v),
            ctx.length * sizeof(uint32_t));
    else
    {
      const uint32_t *end = v + (ctx.length * sizeof(uint32_t));
      for(const uint32_t *p = v; v < end; ++v)
      {
        uint32_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(&t), sizeof(uint32_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const int64_t &v) throw(IOException)
{
  int64_t tmp = v;

  if(! isSameEndianness())
    ByteOrder::reverseBytes(tmp);
  write(reinterpret_cast<const byte_t *>(&tmp), sizeof(tmp));

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const int64_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if( isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v),
            ctx.length * sizeof(int64_t));
    else
    {
      const int64_t *end = v + (ctx.length * sizeof(int64_t));
      for(const int64_t *p = v; v < end; ++v)
      {
        int64_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(t), sizeof(int64_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const uint64_t &v) throw(IOException)
{
  uint64_t tmp = v;

  if(! isSameEndianness())
    ByteOrder::reverseBytes(tmp);
  write(reinterpret_cast<const byte_t *>(&tmp), sizeof(tmp));

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const uint64_t *v) throw(IOException)
{
  const Context &ctx = currentContext();

  if(ctx.length > 0)
  {
    if(isSameEndianness())
      write(reinterpret_cast<const byte_t *>(v),
            ctx.length * sizeof(uint64_t));
    else
    {
      const uint64_t *end = v + (ctx.length * sizeof(uint64_t));
      for(const uint64_t *p = v; v < end; ++v)
      {
        uint64_t t = *p;
        ByteOrder::reverseBytes(t);
        write(reinterpret_cast<const byte_t *>(t), sizeof(uint64_t));
      }
    }
  }

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(float v) throw(IOException)
{
  // assume IEEE-754 floating point representation
  union
  {
    float f;
    uint32_t i;
  } un;

  un.f = v;

  return(operator<<(un.i));
}

/*
 */

DataWriter & DataWriter::operator<<(const float *v) throw(IOException)
{
  // assume IEEE-754 floating point representation

  return(operator<<(reinterpret_cast<const uint32_t *>(v)));
}

/*
 */

DataWriter & DataWriter::operator<<(const double &v) throw(IOException)
{
  // assume IEEE-754 floating point representation
  union
  {
    double d;
    uint64_t i;
  } un;

  un.d = v;

  return(operator<<(un.i));
}

/*
 */

DataWriter & DataWriter::operator<<(const double *v) throw(IOException)
{
  // assume IEEE-754 floating point representation

  return(operator<<(reinterpret_cast<const uint64_t *>(v)));
}

/*
 */

DataWriter & DataWriter::operator<<(const Blob &v) throw(IOException)
{
  const Context &ctx = currentContext();

  size_t vlen = v.getLength();
  size_t wlen = (ctx.length > 0 ? ctx.length : vlen);

  write(v.getData(), std::min(vlen, wlen));

  if((ctx.length > 0) && (vlen < wlen))
    skip(wlen - vlen, ctx.stringPadding ? ' ' : 0);

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const String &v) throw(IOException)
{
  const Context &ctx = currentContext();

  size_t vlen = v.getLength();
  size_t wlen = (ctx.length > 0 ? ctx.length : vlen);

  write(reinterpret_cast<const byte_t *>(v.c_str()), std::min(vlen, wlen));

  if((ctx.length > 0) && (vlen < wlen))
    skip(wlen - vlen, ctx.stringPadding ? ' ' : 0);

  return(*this);
}

/*
 */

DataWriter & DataWriter::operator<<(const Manipulator &manip)
  throw(IOException)
{
  manip.apply(*this);

  return(*this);
}


}; // namespace ccxx

/* end of source file */
