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

#include "commonc++/BitSet.h++"
#include "commonc++/Private.h++"

#include <algorithm>
#include <cstdlib>

namespace ccxx {

/*
 */

// masks for 'first N bits' (counting from LSB)

static const int32_t __firstN[]
  = { 0x00000000,
      0x00000001, 0x00000003, 0x00000007, 0x0000000F,
      0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
      0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
      0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
      0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
      0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF,
      0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
      0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF };

// masks for 'last N bits' (counting from MSB)

static const int32_t __lastN[]
  = { 0x00000000,
      0x80000000, 0xC0000000, 0xE0000000, 0xF0000000,
      0xF8000000, 0xFC000000, 0xFE000000, 0xFF000000,
      0xFF800000, 0xFFC00000, 0xFFE00000, 0xFFF00000,
      0xFFF80000, 0xFFFC0000, 0xFFFE0000, 0xFFFF0000,
      0xFFFF8000, 0xFFFFC000, 0xFFFFE000, 0xFFFFF000,
      0xFFFFF800, 0xFFFFFC00, 0xFFFFFE00, 0xFFFFFF00,
      0xFFFFFF80, 0xFFFFFFC0, 0xFFFFFFE0, 0xFFFFFFF0,
      0xFFFFFFF8, 0xFFFFFFFC, 0xFFFFFFFE, 0xFFFFFFFF };

/*
 */

BitSet::Bit::Bit(BitSet& set, uint_t pos) throw()
  : _set(set), _pos(pos)
{
}

/*
 */

BitSet::Bit::Bit(const BitSet::Bit &other) throw()
  : _set(other._set), _pos(other._pos)
{
}

/*
 */

BitSet::Bit &BitSet::Bit::operator=(const BitSet::Bit &other) throw()
{
  _set = other._set;
  _pos = other._pos;

  return(*this);
}

/*
 */

BitSet::Bit::~Bit() throw()
{
}

/*
 */

BitSet::Bit& BitSet::Bit::operator=(bool val) throw()
{
  _set.set(_pos, val);

  return(*this);
}

/*
 */

BitSet::Bit::operator bool() const throw()
{
  return(_set.isSet(_pos));
}

/*
 */

BitSet::Bit& BitSet::Bit::flip() throw()
{
  _set.set(_pos, ! _set.isSet(_pos));

  return(*this);
}

/*
 */

BitSet::BitSet(uint_t size)
{
  _init(size);

  std::memset(_words, 0, _length * sizeof(uint32_t));
}

/*
 */

BitSet::BitSet(const BitSet& other)
{
  _init(other._size);

  std::memcpy(_words, other._words, _length * sizeof(uint32_t));
}

/*
 */

BitSet::BitSet(const String& bits)
{
  _init(bits.getLength());

  for(uint_t i = 0; i < _size; ++i)
    set(i, (bits[i] == '1'));
}

/*
 */

void BitSet::_init(uint_t size)
{
  if (size < 1)
    size = 1;

  _length = (size + 31) / 32;

  _words = new uint32_t[_length];

  _size = size;

  int mod = size % 32;
  if(mod == 0)
    mod = 32;

  _tailMask = __firstN[mod];
}

/*
 */

BitSet::~BitSet() throw()
{
  delete[] _words;
}

/*
 */

BitSet& BitSet::set(uint_t pos, bool value /* = true */) throw()
{
  if(pos < _size)
  {
    div_t dv = std::div(pos, 32);

    if(value)
      CCXX_BIT_SET(_words[dv.quot], dv.rem);
    else
      CCXX_BIT_CLEAR(_words[dv.quot], dv.rem);
  }

  return(*this);
}

/*
 */

BitSet& BitSet::setRange(uint_t startPos, uint_t endPos,
                         bool value /* = true */) throw()
{
  div_t dv1, dv2;

  dv1 = std::div(startPos, 32);
  dv2 = std::div(endPos, 32);

  for(int i = dv1.quot; i <= dv2.quot; ++i)
  {
    uint32_t mask = 0;

    if(i == dv1.quot)
    {
      mask = __lastN[32 - dv1.rem];
      if(i == dv2.quot)
        mask &= __firstN[dv2.rem + 1];
    }
    else if(i == dv2.quot)
      mask = __firstN[dv2.rem + 1];
    else
      mask = 0xFFFFFFFF;

    if(value)
      _words[i] |= mask;
    else
      _words[i] &= ~mask;
  }

  return(*this);
}

/*
 */

BitSet& BitSet::setAll(bool value /* = true */) throw()
{
  uint32_t val = (value ? 0xFFFFFFFF : 0);
  std::memset(_words, val, _length * sizeof(uint32_t));

  if(val)
    _words[_length - 1] &= _tailMask;

  return(*this);
}

/*
 */

bool BitSet::isSet(int pos) const throw()
{
  if((pos >= 0) && (pos < static_cast<int>(_size)))
    return(CCXX_BIT_TEST(_words[pos / 32], (pos % 32)));
  else
    return(false);
}

/*
 */

bool BitSet::isAnySet() const throw()
{
  for(uint_t i = 0; i < _length; ++i)
  {
    if(_words[i] != 0)
      return(true);
  }

  return(false);
}

/*
 */

bool BitSet::isAllSet() const throw()
{
  for(uint_t i = 0; i < _length - 1; ++i)
  {
    if(_words[i] != static_cast<uint_t>(0xFFFFFFFF))
      return(false);
  }

  return(_words[_length - 1] == _tailMask);
}

/*
 */

BitSet& BitSet::flip() throw()
{
  for(uint_t i = 0; i < _length; ++i)
    _words[i] = ~_words[i];

  _words[_length - 1] &= _tailMask;

  return(*this);
}

/*
 */

int BitSet::_nextBit(uint_t startPos, bool set) const throw()
{
  if(startPos < _size)
  {
    uint_t offset = startPos / 32;
    uint32_t mask = 1 << startPos % 32;

    while(offset < _length)
    {
      const uint32_t &word = _words[offset];
      do
      {
        if(((word & mask) == 0) ^ set)
          return(startPos);

        mask <<= 1;

        if(++startPos == _size)
          return(-1);
      }
      while(mask != 0);
      mask = 1;
      ++offset;
    }
  }

  return(-1);
}

/*
 */

BitSet& BitSet::flip(uint_t pos) throw(OutOfBoundsException)
{
  if(pos >= _size)
    throw OutOfBoundsException();

  uint32_t &word = _words[pos / 32];
  uint_t bit = pos % 32;

  if(CCXX_BIT_TEST(word, bit))
    CCXX_BIT_CLEAR(word, bit);
  else
    CCXX_BIT_SET(word, bit);

  return(*this);
}

/*
 */

void BitSet::swap(BitSet &other) throw(OutOfBoundsException)
{
  if(_size != other._size)
    throw OutOfBoundsException();

  for(uint_t i = 0; i < _length; ++i)
  {
    uint32_t tmp = _words[i];
    _words[i] = other._words[i];
    other._words[i] = tmp;
  }
}

/*
 */

BitSet::Bit BitSet::operator[](int pos) throw(OutOfBoundsException)
{
  if((pos < 0) || (pos >= static_cast<int>(_size)))
    throw OutOfBoundsException();

  return(Bit(*this, pos));
}

/*
 */

void BitSet::setWord(uint_t index, uint32_t word) throw()
{
  if(index >= _length)
    return;

  _words[index] = word;
  if(index == (_length - 1))
    _words[index] &= _tailMask;
}

/*
 */

String BitSet::toString() const
{
  String s;

  // slow implementation, but we don't expect to be doing this often...
  for(uint_t i = 0; i < _size; ++i)
    s << (get(i) ? '1' : '0');

  return(s);
}

/*
 */

bool BitSet::operator==(const BitSet& other) const throw()
{
  if(other._size != _size)
    return(false);

  return(std::memcmp(_words, other._words, _length * sizeof(uint32_t)) == 0);
}

/*
 */

BitSet& BitSet::operator=(const BitSet& other)
{
  if(other._length != _length)
  {
    uint32_t *newwords = new uint32_t[other._length];

    delete[] _words;
    _words = newwords;
    _length = other._length;
  }

  _size = other._size;
  std::memcpy(_words, other._words, _length * sizeof(uint32_t));

  return(*this);
}

/*
 */

BitSet& BitSet::operator|=(const BitSet& other) throw(OutOfBoundsException)
{
  if(_size != other._size)
    throw OutOfBoundsException();

  for(uint_t i = 0; i < _length; ++i)
    _words[i] |= other._words[i];

  return(*this);
}

/*
 */

BitSet& BitSet::operator&=(const BitSet& other) throw(OutOfBoundsException)
{
  if(_size != other._size)
    throw OutOfBoundsException();

  for(uint_t i = 0; i < _length; ++i)
    _words[i] &= other._words[i];

  return(*this);
}

/*
 */

BitSet& BitSet::operator^=(const BitSet& other) throw(OutOfBoundsException)
{
  if(_size != other._size)
    throw OutOfBoundsException();

  for(uint_t i = 0; i < _length; ++i)
    _words[i] ^= other._words[i];

  return(*this);
}

/*
 */

BitSet& BitSet::operator>>=(size_t n) throw()
{
  n %= _size;
  div_t dv = std::div(n, 32);
  uint_t nwm = _length - dv.quot; // number of words to move

  if(nwm > 0)
  {
    // shift words (backward in memory)

    std::memmove(&_words[0], &_words[dv.quot], nwm * sizeof(uint32_t));
    std::memset(&_words[nwm], 0, dv.quot * sizeof(uint32_t));
  }

  // shift bits

  uint_t nbm = 32 - dv.rem;
  uint_t mx = nwm - 1;

  for(uint_t i = 0; i < mx; ++i)
  {
    _words[i] >>= dv.rem;
    _words[i] |= ((_words[i + 1] & __firstN[dv.rem]) << nbm);
  }

  _words[mx] >>= dv.rem;

  return(*this);
}

/*
 */

BitSet& BitSet::operator<<=(size_t n) throw()
{
  n %= _size;
  div_t dv = std::div(n, 32);
  uint_t nwm = _length - dv.quot; // number of words to move

  if(nwm > 0)
  {
    // shift words (forward in memory)

    std::memmove(&_words[dv.quot], &_words[0], nwm * sizeof(uint32_t));
    std::memset(&_words[0], 0, dv.quot * sizeof(uint32_t));
  }

  // shift bits

  uint_t nbm = 32 - dv.rem;

  for(int i = _length - 1; i > dv.quot; --i)
  {
    _words[i] <<= dv.rem;
    _words[i] |= ((_words[i - 1] & __lastN[dv.rem]) >> nbm);
  }

  _words[dv.quot] <<= dv.rem;
  _words[_length - 1] &= _tailMask;

  return(*this);
}

/*
 */

void BitSet::write(std::ostream &stream) const
{
  // slow implementation, but we don't expect to be doing this often...

  for(uint_t i = 0; i < _size; ++i)
    stream << (get(i) ? '1' : '0');
}


}; // namespace ccxx

/* end of source file */
