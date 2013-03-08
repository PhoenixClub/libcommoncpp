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

#ifndef __ccxx_CharOps_hxx
#define __ccxx_CharOps_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 *
 * @author Mark Lindner
 */
class COMMONCPP_API CharOps
{
  public:

  static int compare(const char16_t* a, const char16_t* b,
                     bool ignoreCase = false) throw();

  static int compare(const char16_t a, const char16_t b,
                     bool ignoreCase = false) throw();

  static int compare(const char16_t* a, const char16_t* b, size_t n,
                     bool ignoreCase = false) throw();

  static char16_t* move(char16_t* dest, char16_t* src, size_t n) throw();

  static const char16_t* find(const char16_t* str, char16_t c, size_t len = 0)
    throw();

  static char16_t* find(char16_t* str, char16_t c, size_t len = 0) throw();

  static const char16_t* find(const char16_t* str, const char16_t* s) throw();

  static char16_t* find(char16_t* str, const char16_t* s) throw();

  static const char16_t* findLast(const char16_t* str, char16_t c,
                                  uint_t fromIndex = END) throw();

  static char16_t* findLast(char16_t* str, char16_t c, uint_t fromIndex = END)
    throw();

  static const char16_t* findLast(const char16_t* str, const char16_t* s,
                                  uint_t fromIndex = END) throw();

  static char16_t* findLast(char16_t* str, const char16_t* s,
                            uint_t fromIndex = END) throw();

  static size_t length(const char16_t* str) throw();

  static char16_t* copy(char16_t* dest, const char16_t* src, size_t n) throw();

  static char16_t* copyRaw(char16_t* dest, const char16_t* src, size_t n)
    throw();

  static int32_t toLong(const char16_t* str, int base = 10) throw();

  static uint32_t toULong(const char16_t* str, int base = 10) throw();

  static int64_t toLongLong(const char16_t* str, int base = 10) throw();

  static uint64_t toULongLong(const char16_t* str, int base = 10) throw();

  static float toFloat(const char16_t* str) throw();

  static double toDouble(const char16_t* str) throw();

  static const uint_t END;
  static const char16_t TRUE_REP[];
  static const char16_t FALSE_REP[];
  static const char16_t DIGITS[];

  private:

  static char* _toASCII(const char16_t* str, char* buf, size_t len) throw();
};

} // namespace ccxx

#endif // __ccxx_CharOps_hxx
