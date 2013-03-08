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

#include "commonc++/DataEncoder.h++"
#include "commonc++/ByteOrder.h++"

#include <deque>

namespace ccxx {

/*
 */

class DataEncoder::ContextStack : public std::deque<Context> { };

/*
 */

DataEncoder::SetEndianness::SetEndianness(Endianness endianness) throw()
  : _endianness(endianness)
{
}

/*
 */

DataEncoder::SetEndianness::~SetEndianness() throw()
{
}

/*
 */

void DataEncoder::SetEndianness::apply(DataEncoder &encoder) const
  throw(IOException)
{
  encoder.setEndianness(_endianness);
}

/*
 */

DataEncoder::SetLength::SetLength(size_t length) throw()
  : _length(length)
{
}

/*
 */

DataEncoder::SetLength::~SetLength() throw()
{
}

/*
 */

void DataEncoder::SetLength::apply(DataEncoder &encoder) const
  throw(IOException)
{
  encoder.setLength(_length);
}

/*
 */

DataEncoder::Skip::Skip(size_t count) throw()
  : _count(count)
{
}

/*
 */

DataEncoder::Skip::~Skip() throw()
{
}

/*
 */

void DataEncoder::Skip::apply(DataEncoder &encoder) const throw(IOException)
{
  if(_count > 0)
    encoder.skip(_count);
}

/*
 */

DataEncoder::Align::Align(size_t size) throw()
  : _size(size)
{
}

/*
 */

DataEncoder::Align::~Align() throw()
{
}

/*
 */

void DataEncoder::Align::apply(DataEncoder &encoder) const throw(IOException)
{
  encoder.align(_size);
}

/*
 */

DataEncoder::SetLimit::SetLimit(int64_t limit) throw()
  : _limit(limit)
{
}

/*
 */

DataEncoder::SetLimit::~SetLimit() throw()
{
}

/*
 */

void DataEncoder::SetLimit::apply(DataEncoder &encoder) const
  throw(IOException)
{
  encoder.setLimit(_limit);
}

/*
 */

DataEncoder::SetStringPadding::SetStringPadding(bool stringPadding) throw()
  : _stringPadding(stringPadding)
{
}

/*
 */

DataEncoder::SetStringPadding::~SetStringPadding() throw()
{
}

/*
 */

void DataEncoder::SetStringPadding::apply(DataEncoder &encoder) const
  throw(IOException)
{
  encoder.setStringPadding(_stringPadding);
}

/*
 */

DataEncoder::DataEncoder() throw()
{
  _contexts = new ContextStack();

  _contexts->push_back(Context());
}

/*
 */

DataEncoder::~DataEncoder() throw()
{
  delete _contexts;
}

/*
 */

void DataEncoder::pushContext()
{
  Context &parent = currentContext();
  Context ctx;
  ctx.endianness = parent.endianness;
  ctx.stringPadding = parent.stringPadding;

  if(parent.limit != 0)
  {
    ctx.parentLimit = parent.limit - parent.offset;
    ctx.limit = ctx.parentLimit;
  }

  _contexts->push_back(ctx);
}

/*
 */

void DataEncoder::popContext()
{
  if(_contexts->size() > 1)
  {
    int64_t offset = currentContext().offset;
    _contexts->pop_back();

    Context &ctx = currentContext();
    ctx.offset += offset;

    ctx.maxOffset = std::max(ctx.maxOffset, ctx.offset);
  }
}

/*
 */

bool DataEncoder::isTopContext() const throw()
{
  return(_contexts->size() == 1);
}

/*
 */

uint_t DataEncoder::getContextDepth() const throw()
{
  return(_contexts->size());
}

/*
 */

void DataEncoder::setEndianness(Endianness endianness) throw()
{
  Context &ctx = currentContext();
  ctx.endianness = endianness;
}

/*
 */

Endianness DataEncoder::getEndianness() const throw()
{
  const Context &ctx = currentContext();
  return(ctx.endianness);
}

/*
 */

void DataEncoder::setLength(size_t length) throw()
{
  Context &ctx = currentContext();
  ctx.length = length;
}

/*
 */

size_t DataEncoder::getLength() const throw()
{
  const Context &ctx = currentContext();
  return(ctx.length);
}

/*
 */

void DataEncoder::setLimit(int64_t limit) throw(IOException)
{
  Context &ctx = currentContext();

  if(ctx.parentLimit == 0)
  {
    if((limit != 0) && (limit < ctx.maxOffset))
      throw IOException("out of bounds");
  }
  else
  {
    if((limit == 0) || (limit < ctx.maxOffset) || (limit > ctx.parentLimit))
      throw IOException(String() << "out of bounds: limit " << limit
                        << " is outside valid range");
  }

  ctx.limit = limit;
}

/*
 */

int64_t DataEncoder::getLimit() const throw()
{
  const Context &ctx = currentContext();
  return(ctx.limit);
}

/*
 */

void DataEncoder::setStringPadding(bool stringPadding) throw()
{
  Context &ctx = currentContext();
  ctx.stringPadding = stringPadding;
}

/*
 */

bool DataEncoder::getStringPadding() const throw()
{
  const Context &ctx = currentContext();
  return(ctx.stringPadding);
}

/*
 */

int64_t DataEncoder::getOffset() const throw()
{
  const Context &ctx = currentContext();
  return(ctx.offset);
}

/*
 */

int64_t DataEncoder::getCumulativeOffset() const throw()
{
  int64_t offset = INT64_CONST(0);

  for(ContextStack::const_iterator iter = _contexts->begin();
      iter != _contexts->end();
      ++iter)
  {
    offset += iter->offset;
  }

  return(offset);
}

/*
 */

void DataEncoder::reset() throw(IOException)
{
  while(_contexts->size() > 1)
    _contexts->pop_back();

  Context &ctx = currentContext();
  ctx.reset();
}

/*
 */

void DataEncoder::align(size_t size) throw(IOException)
{
  size_t pos = getCumulativeOffset() % size;
  if(pos)
    skip(size - pos);
}

/*
 */

int64_t DataEncoder::getRemaining() const throw()
{
  const Context &ctx = currentContext();

  return((ctx.limit == 0) ? INT64_MAX : (ctx.limit - ctx.offset));
}

/*
 */

void DataEncoder::checkRemaining(const Context &ctx, size_t count)
  const throw(IOException)
{
  if((ctx.limit != 0) && ((ctx.limit - ctx.offset) < count))
    throw IOException("overflow/underflow");
}

/*
 */

bool DataEncoder::isSameEndianness() const throw()
{
  const Context &ctx = currentContext();
  return(ctx.endianness == ByteOrder::getEndianness());
}

/*
 */

DataEncoder::Context &DataEncoder::currentContext()
{
  return(_contexts->back());
}

/*
 */

const DataEncoder::Context &DataEncoder::currentContext() const
{
  return(_contexts->back());
}

/*
 */

DataEncoder::Context::Context()
{
  reset();
}

/*
 */

void DataEncoder::Context::reset()
{
  endianness = ByteOrder::getEndianness();
  length = 0;
  limit = INT64_CONST(0);
  parentLimit = INT64_CONST(0);
  offset = INT64_CONST(0);
  maxOffset = INT64_CONST(0);
  stringPadding = false;
}

/*
 */

void DataEncoder::Context::bumpOffset(size_t delta)
{
  offset += delta;
  maxOffset = std::max(maxOffset, offset);
}


}; // namespace ccxx

/* end of source file */
