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

#include "commonc++/Char.h++"

/*
 */

namespace ccxx {

template<>
  const BasicChar<char> BasicChar<char>::EndOfFile = BasicChar<char>(4);
template<>
  const BasicChar<char> BasicChar<char>::Bell = BasicChar<char>('\a');
template<>
  const BasicChar<char> BasicChar<char>::Backspace = BasicChar<char>('\b');
template<>
  const BasicChar<char> BasicChar<char>::Tab = BasicChar<char>('\t');
template<>
  const BasicChar<char> BasicChar<char>::NewLine = BasicChar<char>('\x0A');
template<>
  const BasicChar<char> BasicChar<char>::VTab = BasicChar<char>('\v');
template<>
  const BasicChar<char> BasicChar<char>::FormFeed = BasicChar<char>('\f');
template<>
  const BasicChar<char> BasicChar<char>::Return = BasicChar<char>('\r');
template<>
  const BasicChar<char> BasicChar<char>::Escape = BasicChar<char>(27);
template<>
  const BasicChar<char> BasicChar<char>::Space = BasicChar<char>(' ');
template<>
  const BasicChar<char> BasicChar<char>::Delete = BasicChar<char>(127);

template class BasicChar<char>;

}; // namespace ccxx

/* end of source file */
