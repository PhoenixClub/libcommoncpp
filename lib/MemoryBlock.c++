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

#include "commonc++/MemoryBlock.h++"

#include <cstring>

namespace ccxx {

/*
 */

void MemoryBlock::fill(byte_t value) throw()
{
  std::memset(_base, value, _size);
}

/*
 */

bool MemoryBlock::operator==(const MemoryBlock& other) const throw()
{
  if(other._size != _size)
    return(false);

  if(_base == NULL)
    return(other._base == NULL);

  return(std::memcmp(static_cast<void *>(_base),
                     static_cast<void *>(other._base),
                     _size) == 0);
}

/*
 */

byte_t MemoryBlock::operator[](int index) const throw(OutOfBoundsException)
{
  if((_base == NULL) || (index < 0) || (index >= static_cast<int>(_size)))
    throw OutOfBoundsException();
  else
    return(*(_base + index));
}

/*
 */

byte_t& MemoryBlock::operator[](int index) throw(OutOfBoundsException)
{
  if((_base == NULL) || (index < 0) || (index >= static_cast<int>(_size)))
    throw OutOfBoundsException();
  else
    return(*(_base + index));
}

/*
 */

bool MemoryBlock::copyFrom(const MemoryBlock& other) throw()
{
  if((_base == NULL) || (other._base == NULL))
    return(false);

  size_t sz = (other._size > _size) ? _size : other._size;

  if((sz == 0)
     || ((other._base >= _base) && (other._base < (_base + sz)))
     || ((_base >= other._base) && (_base < (other._base + sz))))
    return(false); // zero size, or overlapping

  std::memcpy(_base, other._base, sz);

  return(true);
}

/*
 */

void MemoryBlock::moveFrom(MemoryBlock& other) throw()
{
  if((_base != NULL) && (other._base != NULL))
  {
    size_t sz = (other._size > _size) ? _size : other._size;

    if(sz > 0)
      std::memmove(_base, other._base, sz);
  }
}

/*
 */

int MemoryBlock::indexOf(byte_t val, uint_t startIndex /* = 0 */) const
  throw()
{
  if((_base == NULL) || (startIndex >= _size))
    return(-1);

  void *p = std::memchr((_base + startIndex), static_cast<int>(val),
                        (_size - startIndex));

  return(p ? static_cast<int>((byte_t *)p - _base) : -1);
}


}; // namespace ccxx

/* end of source file */
