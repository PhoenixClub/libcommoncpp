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

#include "commonc++/UChar.h++"

#include <cwctype>

namespace ccxx {

// static
const UChar UChar::nul;

/*
 */

UChar::UChar()
  : _char(0)
{
}

/*
 */

UChar::UChar(char c)
  : _char(static_cast<char16_t>(c))
{
}

/*
 */

UChar::UChar(char16_t c)
  : _char(c)
{
}

/*
 */

UChar::UChar(int c)
  : _char(static_cast<char16_t>(c & 0xFFFF))
{
}

/*
 */

UChar::~UChar()
{
}

/*
 */

UChar& UChar::operator=(char c) throw()
{
  _char = static_cast<char16_t>(c);

  return(*this);
}

/*
 */

UChar& UChar::operator=(char16_t c) throw()
{
  _char = c;

  return(*this);
}

/*
 */

UChar& UChar::operator=(int c) throw()
{
  _char = static_cast<char16_t>(c & 0xFFFF);

  return(*this);
}

/*
 */

bool UChar::isNul() const throw()
{
  return(_char == 0);
}

/*
 */

bool UChar::isUpperCase() const throw()
{
  return(isUpperCase(_char));
}

/*
 */

bool UChar::isLowerCase() const throw()
{
  return(isLowerCase(_char));
}

/*
 */

UChar UChar::toLowerCase() const throw()
{
  return(UChar(toLowerCase(_char)));
}

/*
 */

UChar UChar::toUpperCase() const throw()
{
  return(UChar(toUpperCase(_char)));
}

/*
 */

bool UChar::isSign() const throw()
{
  return(isSign(_char));
}

/*
 */

bool UChar::isLetter() const throw()
{
  return(isLetter(_char));
}

/*
 */

bool UChar::isDigit() const throw()
{
  return(isDigit(_char));
}

/*
 */

bool UChar::isWhitespace() const throw()
{
  return(UChar::isWhitespace(_char));
}

/*
 */

bool UChar::isLowSurrogate() const throw()
{
  return(UChar::isLowSurrogate(_char));
}

/*
 */

bool UChar::isHighSurrogate() const throw()
{
  return(UChar::isHighSurrogate(_char));
}

/*
 */

// static
bool UChar::isNul(char16_t c) throw()
{
  return(c == 0);
}

/*
 */

// static
bool UChar::isLetter(char16_t c) throw()
{
  return(::iswalpha(static_cast<wint_t>(c)));
}

/*
 */

// static
bool UChar::isDigit(char16_t c) throw()
{
  return(::iswdigit(static_cast<wint_t>(c)));
}

/*
 */

// static
bool UChar::isSign(char16_t c) throw()
{
  return((c == '+') || (c == '-'));
}

/*
 */

//static
bool UChar::isWhitespace(char16_t c) throw()
{
  return(::iswspace(static_cast<wint_t>(c)));
}

/*
 */

// static
bool UChar::isLowerCase(char16_t c) throw()
{
  return(::iswlower(static_cast<wint_t>(c)));
}

/*
 */

// static
bool UChar::isUpperCase(char16_t c) throw()
{
  return(::iswupper(static_cast<wint_t>(c)));
}

/*
 */

// static
char16_t UChar::toUpperCase(char16_t c) throw()
{
  return(static_cast<char16_t>(::towupper(static_cast<wint_t>(c))));
}

/*
 */

// static
char16_t UChar::toLowerCase(char16_t c) throw()
{
  return(static_cast<char16_t>(::towlower(static_cast<wint_t>(c))));
}

/*
 */

// static
bool UChar::isLowSurrogate(char16_t c) throw()
{
  return((c & 0xFC00) == 0xDC00);
}

/*
 */

// static
bool UChar::isHighSurrogate(char16_t c) throw()
{
  return((c & 0xFC00) == 0xD800);
}

/*
 */

// static
char UChar::toASCII(char16_t c) throw()
{
  return((c <= 0xFF) ? static_cast<char>(c & 0xFF) : 0);
}


} // namespace ccxx
