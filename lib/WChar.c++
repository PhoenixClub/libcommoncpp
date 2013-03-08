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
  const BasicChar<wchar_t> BasicChar<wchar_t>::EndOfFile
  = BasicChar<wchar_t>(L'\4');
template<>
  const BasicChar<wchar_t> BasicChar<wchar_t>::Bell
  = BasicChar<wchar_t>(L'\a');
template<>
  const BasicChar<wchar_t> BasicChar<wchar_t>::Backspace
  = BasicChar<wchar_t>(L'\b');
template<>
  const BasicChar<wchar_t> BasicChar<wchar_t>::Tab
  = BasicChar<wchar_t>(L'\t');
template<>
  const BasicChar<wchar_t> BasicChar<wchar_t>::NewLine
  = BasicChar<wchar_t>(L'\n');
template<>
  const BasicChar<wchar_t> BasicChar<wchar_t>::VTab
  = BasicChar<wchar_t>(L'\v');
template<>
  const BasicChar<wchar_t> BasicChar<wchar_t>::FormFeed
  = BasicChar<wchar_t>(L'\f');
template<>
  const BasicChar<wchar_t> BasicChar<wchar_t>::Return
  = BasicChar<wchar_t>(L'\r');
template<>
  const BasicChar<wchar_t> BasicChar<wchar_t>::Escape
  = BasicChar<wchar_t>((wchar_t)27);
template<>
  const BasicChar<wchar_t> BasicChar<wchar_t>::Space
  = BasicChar<wchar_t>(L' ');
template<>
  const BasicChar<wchar_t> BasicChar<wchar_t>::Delete
  = BasicChar<wchar_t>((wchar_t)127);

template class BasicChar<wchar_t>;


}; // namespace ccxx

/* end of source file */
