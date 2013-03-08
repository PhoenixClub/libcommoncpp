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

#include "commonc++/CharTraits.h++"

#ifdef CCXX_OS_WINDOWS
#include "cbits.h"
#endif

#include <cstdlib>

namespace ccxx {

/*
 */

static const char *__strrstr(const char *str, const char *s, uint_t fromIndex)
{
  size_t slen = std::strlen(s);

  if(fromIndex < slen)
    return(NULL);

  const char *p = str + fromIndex;
  const char *q, *lq;
  q = lq = s + slen - 1;
  size_t c = 0;

  while(p >= str)
  {
    if(*p == *q)
    {
      if(++c == slen)
        return(p);
      --q;
    }
    else
    {
      c = 0;
      q = lq;
    }

    --p;
  }

  return(NULL);
}

/*
 */

template<> COMMONCPP_API
  size_t BasicCharTraits<char>::length(const char *str) throw()
{
  return(std::strlen(str));
}

/*
 */

template<> COMMONCPP_API
  char * BasicCharTraits<char>::copy(char *dest, const char *src, size_t n)
  throw()
{
  return(std::strncpy(dest, src, n));
}

/*
 */

template<> COMMONCPP_API
  char * BasicCharTraits<char>::rawCopy(char *dest, const char *src,
                                        size_t n) throw()
{
  std::memcpy(dest, src, n * sizeof(char));
  return(dest);
}

/*
 */

template<> COMMONCPP_API
  char * BasicCharTraits<char>::concat(char *dest, const char *src, size_t n)
  throw()
{
  return(std::strncat(dest, src, n));
}

/*
 */

template<> COMMONCPP_API
  char * BasicCharTraits<char>::move(char *dest, char *src, size_t n) throw()
{
  std::memmove(reinterpret_cast<void *>(dest),
               reinterpret_cast<void *>(src), n * sizeof(char));
  return(dest);
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<char>::compare(const char *str1, const char *str2,
                                     bool ignoreCase /* = false */) throw()
{
  return(ignoreCase ? strcasecmp(str1, str2) : std::strcmp(str1, str2));
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<char>::rawCompare(const char *str1, const char *str2,
                                        size_t n) throw()
{
  return(std::memcmp(reinterpret_cast<const void *>(str1),
                     reinterpret_cast<const void *>(str2), n));
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<char>::collate(const char *str1, const char *str2)
  throw()
{
  return(std::strcoll(str1, str2));
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<char>::compare(const char *str1, const char *str2,
                                     size_t n, bool ignoreCase /* = false */)
  throw()
{
  return(ignoreCase ? strncasecmp(str1, str2, n)
         : std::strncmp(str1, str2, n));
}

/*
 */

template<> COMMONCPP_API
  char BasicCharTraits<char>::toLowerCase(char c) throw()
{
  return(static_cast<char>(std::tolower(c)));
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<char>::compare(char c1, char c2,
                                     bool ignoreCase /* = false */) throw()
{
  if(ignoreCase)
  {
    c1 = toLowerCase(c1);
    c2 = toLowerCase(c2);
  }

  if(c1 < c2)
    return(-1);
  else if(c1 > c2)
    return(1);
  else
    return(0);
}

/*
 */

template<> COMMONCPP_API
  size_t BasicCharTraits<char>::span(const char *str, const char *accept)
  throw()
{
  return(std::strspn(str, accept));
}

/*
 */

template<> COMMONCPP_API
  int32_t BasicCharTraits<char>::toLong(const char *str,
                                        char **endptr /* = NULL */,
                                        int base /* = 10 */) throw()
{
  return(std::strtol(str, endptr, base));
}

/*
 */

template<> COMMONCPP_API
  uint32_t BasicCharTraits<char>::toULong(const char *str,
                                          char **endptr /* = NULL */,
                                          int base /* = 10 */) throw()
{
  return(std::strtoul(str, endptr, base));
}

/*
 */

template<> COMMONCPP_API
  int64_t BasicCharTraits<char>::toLongLong(const char *str,
                                            char **endptr /* = NULL */,
                                            int base /* = 10 */) throw()
{
  return(::strtoll(str, endptr, base));
}

/*
 */

template<> COMMONCPP_API
  uint64_t BasicCharTraits<char>::toULongLong(const char *str,
                                              char **endptr /* = NULL */,
                                              int base /* = 10 */) throw()
{
  return(::strtoull(str, endptr, base));
}

/*
 */

template<> COMMONCPP_API
  float BasicCharTraits<char>::toFloat(const char *str,
                                       char **endptr /* = NULL */) throw()
{
#ifdef CCXX_OS_WINDOWS
  return(static_cast<float>(std::strtod(str, endptr)));
#else
  return(/*std::*/strtof(str, endptr));
#endif
}

/*
 */

template<> COMMONCPP_API
  double BasicCharTraits<char>::toDouble(const char *str,
                                         char **endptr /* = NULL */)
  throw()
{
  return(std::strtod(str, endptr));
}

/*
 */

template<> COMMONCPP_API
  char * BasicCharTraits<char>::find(char *str, char c, size_t len /* = 0 */)
  throw()
{
  return(len == 0 ? std::strchr(str, c)
         : reinterpret_cast<char *>(std::memchr(str, c, len)));
}

/*
 */

template<> COMMONCPP_API
  const char * BasicCharTraits<char>::find(const char *str, char c,
    size_t len /* = 0 */) throw()
{
  return(len == 0 ? std::strchr(str, c)
         : reinterpret_cast<const char *>(std::memchr(str, c, len)));
}

/*
 */

template<> COMMONCPP_API
  char * BasicCharTraits<char>::findLast(char *str, char c,
                                         uint_t fromIndex /* = END */) throw()
{
  if(fromIndex == END)
    fromIndex = std::strlen(str) - 1;

  for(char *p = str + fromIndex; p >= str; --p)
  {
    if(*p == c)
      return(p);
  }

  return(NULL);
}

/*
 */

template<> COMMONCPP_API
  const char * BasicCharTraits<char>::findLast(const char *str, char c,
                                               uint_t fromIndex /* = END */)
  throw()
{
  if(fromIndex == END)
    fromIndex = std::strlen(str) - 1;

  for(const char *p = str + fromIndex; p >= str; --p)
  {
    if(*p == c)
      return(p);
  }

  return(NULL);
}

/*
 */

template<> COMMONCPP_API
  char * BasicCharTraits<char>::find(char *str, const char *s) throw()
{
  return(std::strstr(str, s));
}

/*
 */

template<> COMMONCPP_API
  const char * BasicCharTraits<char>::find(const char *str, const char *s)
  throw()
{
  return(std::strstr(str, s));
}

/*
 */

template<> COMMONCPP_API
  char * BasicCharTraits<char>::findLast(char *str, const char *s,
                                         uint_t fromIndex /* = END */)
  throw()
{
  if(fromIndex == END)
    fromIndex = std::strlen(str) - 1;

  return(const_cast<char *>(__strrstr(str, s, fromIndex)));
}

/*
 */

template<> COMMONCPP_API
  const char * BasicCharTraits<char>::findLast(const char *str, const char *s,
                                               uint_t fromIndex /* = END */)
  throw()
{
  if(fromIndex == END)
    fromIndex = std::strlen(str) - 1;

  return(__strrstr(str, s, fromIndex));
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<char>::vprintf(char *str, size_t len, const char *format,
                                     va_list vp) throw()
{
  int r = ::vsnprintf(str, len, format, vp);

  // return value isn't portable; make it so
  if(r > static_cast<int>(len))
    r = -1;

  return(r);
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<char>::printf(char *str, size_t len,
                                    const char *format, ...) throw()
{
  va_list vp;
  int r = 0;

  va_start(vp, format);
  r = vprintf(str, len, format, vp);
  va_end(vp);

  return(r);
}

/*
 */

template<> COMMONCPP_API
  char BasicCharTraits<char>::nul() throw()
{
  return(NUL);
}

/*
 */

template<> COMMONCPP_API
  char BasicCharTraits<char>::plus() throw()
{
  return('+');
}

/*
 */

template<> COMMONCPP_API
  char BasicCharTraits<char>::minus() throw()
{
  return('-');
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::null() throw()
{
  return((const char *)NULL);
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::nullRep() throw()
{
  return("(null)");
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::trueRep() throw()
{
  return("true");
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::falseRep() throw()
{
  return("false");
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::empty() throw()
{
  return("");
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::whitespace() throw()
{
  return(" \t\v\f\r\n");
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::digits() throw()
{
  return("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::intFormat() throw()
{
  return("%d");
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::uintFormat() throw()
{
  return("%u");
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::int64Format() throw()
{
  return(INT64_FMT);
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::uint64Format() throw()
{
  return(UINT64_FMT);
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::doubleFormat() throw()
{
  return("%.*f");
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isAlpha(char c) throw()
{
  return(std::isalpha(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isAlphaNumeric(char c) throw()
{
  return((std::isalpha(c) != 0) || (std::isdigit(c) != 0));
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isWhitespace(char c) throw()
{
  return(std::isspace(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isPrintable(char c) throw()
{
  return(std::isprint(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isGraphical(char c) throw()
{
  return(std::isgraph(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isNul(char c) throw()
{
  return(c == NUL);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isControl(char c) throw()
{
  return(std::iscntrl(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isDigit(char c) throw()
{
  return(std::isdigit(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isSign(char c) throw()
{
  return((c == '+') || (c == '-'));
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isHexDigit(char c) throw()
{
  return(std::isxdigit(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isLowerCase(char c) throw()
{
  return(std::islower(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<char>::isUpperCase(char c) throw()
{
  return(std::isupper(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  char BasicCharTraits<char>::toUpperCase(char c) throw()
{
  return(static_cast<char>(std::toupper(c)));
}


/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<char>::toInt(char c) throw()
{
  return(std::isdigit(c) ? static_cast<int>(c - '0') : 0);
}

/*
 */

template<> COMMONCPP_API
  char BasicCharTraits<char>::fromInt(int v) throw()
{
  return(((v >= 0) && (v <= 9)) ? ('0' + v) : '0');
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<char>::encoding() throw()
{
  return("ASCII//IGNORE");
}

template class BasicCharTraits<char>;


}; // namespace ccxx

/* end of source file */

