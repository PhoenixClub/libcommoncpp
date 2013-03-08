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

namespace ccxx {

#ifdef CCXX_OS_MACOSX

static int wcscasecmp(const wchar_t *s1, const wchar_t *s2)
{
  const wchar_t *p, *q;

  for(p = s1, q = s2; *p && *q; ++p, ++q)
  {
    wchar_t a = towlower(*p);
    wchar_t b = towlower(*q);

    if(a < b)
      return(-1);
    else if(a > b)
      return(1);
  }

  if(*p)
    return(1);
  else if(*q)
    return(-1);
  else
    return(0);
}

static int wcsncasecmp(const wchar_t *s1, const wchar_t *s2, size_t n)
{
  const wchar_t *p, *q;
  size_t i;

  for(p = s1, q = s2, i = 0; *p && *q && (i < n); ++p, ++q, ++i)
  {
    wchar_t a = towlower(*p);
    wchar_t b = towlower(*q);

    if(a < b)
      return(-1);
    else if(a > b)
      return(1);
  }

  if(*p)
    return(1);
  else if(*q)
    return(-1);
  else
    return(0);
}

#endif // CCXX_OS_MACOSX

/*
 */

static const wchar_t *__wcsrstr(const wchar_t *str, const wchar_t *s,
                                uint_t fromIndex)
{
  size_t slen = std::wcslen(s);

  if(fromIndex < slen)
    return(NULL);

  const wchar_t *p = str + fromIndex;
  const wchar_t *q, *lq;
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
  size_t BasicCharTraits<wchar_t>::length(const wchar_t *str) throw()
{
  return(std::wcslen(str));
}

/*
 */

template<> COMMONCPP_API
  wchar_t * BasicCharTraits<wchar_t>::copy(wchar_t *dest, const wchar_t *src,
                                           size_t n) throw()
{
  return(std::wcsncpy(dest, src, n));
}

/*
 */

template<> COMMONCPP_API
  wchar_t * BasicCharTraits<wchar_t>::rawCopy(wchar_t *dest,
                                              const wchar_t *src,
                                              size_t n) throw()
{
  std::memcpy(dest, src, n * sizeof(wchar_t));
  return(dest);
}

/*
 */

template<> COMMONCPP_API
  wchar_t * BasicCharTraits<wchar_t>::concat(wchar_t *dest, const wchar_t *src,
                                             size_t n) throw()
{
  return(std::wcsncat(dest, src, n));
}

/*
 */

template<> COMMONCPP_API
  wchar_t * BasicCharTraits<wchar_t>::move(wchar_t *dest, wchar_t *src,
                                           size_t n) throw()
{
  std::memmove(reinterpret_cast<void *>(dest),
               reinterpret_cast<void *>(src), n * sizeof(wchar_t));
  return(dest);
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<wchar_t>::compare(const wchar_t *str1,
                                        const wchar_t *str2,
                                        bool ignoreCase /* = false */)
  throw()
{
  return(ignoreCase ? wcscasecmp(str1, str2) : std::wcscmp(str1, str2));
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<wchar_t>::rawCompare(const wchar_t *str1,
                                           const wchar_t *str2,
                                           size_t n) throw()
{
  return(std::wmemcmp(str1, str2, n));
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<wchar_t>::collate(const wchar_t *str1,
                                        const wchar_t *str2) throw()
{
  return(std::wcscoll(str1, str2));
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<wchar_t>::compare(const wchar_t *str1,
                                        const wchar_t *str2, size_t n,
                                        bool ignoreCase /* = false */)
  throw()
{
  return(ignoreCase ? wcsncasecmp(str1, str2, n)
         : std::wcsncmp(str1, str2, n));
}

/*
 */

template<> COMMONCPP_API
  wchar_t BasicCharTraits<wchar_t>::toLowerCase(wchar_t c) throw()
{
  return(static_cast<wchar_t>(std::towlower(c)));
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<wchar_t>::compare(wchar_t c1, wchar_t c2,
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
  size_t BasicCharTraits<wchar_t>::span(const wchar_t *str,
                                        const wchar_t *accept) throw()
{
  return(std::wcsspn(str, accept));
}

/*
 */

template<> COMMONCPP_API
  int32_t BasicCharTraits<wchar_t>::toLong(const wchar_t *str,
                                           wchar_t **endptr /* = NULL */,
                                           int base /* = 10 */) throw()
{
  return(std::wcstol(str, endptr, base));
}

/*
 */

template<> COMMONCPP_API
  uint32_t BasicCharTraits<wchar_t>::toULong(const wchar_t *str,
                                             wchar_t **endptr /* = NULL */,
                                             int base /* = 10 */) throw()
{
  return(std::wcstoul(str, endptr, base));
}

/*
 */

template<> COMMONCPP_API
  int64_t BasicCharTraits<wchar_t>::toLongLong(const wchar_t *str,
                                               wchar_t **endptr /* = NULL */,
                                               int base /* = 10 */) throw()
{
  return(wcstoll(str, endptr, base));
}

/*
 */

template<> COMMONCPP_API
  uint64_t BasicCharTraits<wchar_t>::toULongLong(const wchar_t *str,
                                                 wchar_t **endptr /* = NULL */,
                                                 int base /* = 10 */) throw()
{
  return(wcstoull(str, endptr, base));
}

/*
 */

template<> COMMONCPP_API
  float BasicCharTraits<wchar_t>::toFloat(const wchar_t *str,
                                          wchar_t **endptr /* = NULL */)
  throw()
{

#ifdef CCXX_OS_WINDOWS
  return(static_cast<float>(std::wcstod(str, endptr)));
#else
  return(std::wcstof(str, endptr));
#endif
}

/*
 */

template<> COMMONCPP_API
  double BasicCharTraits<wchar_t>::toDouble(const wchar_t *str,
                                            wchar_t **endptr /* = NULL */)
  throw()
{
  return(std::wcstod(str, endptr));
}

/*
 */

template<> COMMONCPP_API
  wchar_t * BasicCharTraits<wchar_t>::find(wchar_t *str, wchar_t c,
                                           size_t len /* = 0 */) throw()
{
  return(len == 0 ? std::wcschr(str, c) : std::wmemchr(str, c, len));
}

/*
 */

template<> COMMONCPP_API
  const wchar_t * BasicCharTraits<wchar_t>::find(const wchar_t *str, wchar_t c,
                                                 size_t len /* = 0 */) throw()
{
  return(len == 0 ? std::wcschr(str, c) : std::wmemchr(str, c, len));
}

/*
 */

template<> COMMONCPP_API
  wchar_t * BasicCharTraits<wchar_t>::findLast(wchar_t *str, wchar_t c,
                                               uint_t fromIndex /* = END */)
  throw()
{
  if(fromIndex == END)
    fromIndex = std::wcslen(str) - 1;

  for(wchar_t *p = str + fromIndex; p >= str; --p)
  {
    if(*p == c)
      return(p);
  }

  return(NULL);
}

/*
 */

template<> COMMONCPP_API
  const wchar_t * BasicCharTraits<wchar_t>::findLast(
    const wchar_t *str, wchar_t c, uint_t fromIndex /* = END */) throw()
{
  if(fromIndex == END)
    fromIndex = std::wcslen(str) - 1;

  for(const wchar_t *p = str; p >= str; --p)
  {
    if(*p == c)
      return(p);
  }

  return(NULL);
}

/*
 */

template<> COMMONCPP_API
  wchar_t * BasicCharTraits<wchar_t>::find(wchar_t *str, const wchar_t *s)
  throw()
{
  return(std::wcsstr(str, s));
}


/*
 */

template<> COMMONCPP_API
  const wchar_t * BasicCharTraits<wchar_t>::find(const wchar_t *str,
                                                 const wchar_t *s) throw()
{
  return(std::wcsstr(str, s));
}

/*
 */

template<> COMMONCPP_API
  wchar_t * BasicCharTraits<wchar_t>::findLast(wchar_t *str, const wchar_t *s,
                                               uint_t fromIndex /* = END */)
  throw()
{
  if(fromIndex == END)
    fromIndex = std::wcslen(str) - 1;

  return(const_cast<wchar_t *>(__wcsrstr(str, s, fromIndex)));
}

/*
 */

template<> COMMONCPP_API
  const wchar_t * BasicCharTraits<wchar_t>::findLast(
    const wchar_t *str, const wchar_t *s, uint_t fromIndex /* = END */) throw()
{
  if(fromIndex == END)
    fromIndex = std::wcslen(str) - 1;

  return(__wcsrstr(str, s, fromIndex));
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<wchar_t>::vprintf(wchar_t *str, size_t len,
                                        const wchar_t *format, va_list vp)
  throw()
{
#ifdef CCXX_OS_WINDOWS_GNU
  return(::_vsnwprintf(str, len, format, vp));
#else
  return(std::vswprintf(str, len, format, vp));
#endif
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<wchar_t>::printf(wchar_t *str, size_t len,
                                       const wchar_t *format, ...) throw()
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
  wchar_t BasicCharTraits<wchar_t>::nul() throw()
{
  return(WNUL);
}


/*
 */

template<> COMMONCPP_API
  wchar_t BasicCharTraits<wchar_t>::plus() throw()
{
  return(L'+');
}

/*
 */

template<> COMMONCPP_API
  wchar_t BasicCharTraits<wchar_t>::minus() throw()
{
  return(L'-');
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::null() throw()
{
  return((const wchar_t *)NULL);
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::nullRep() throw()
{
  return(L"(null)");
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::trueRep() throw()
{
  return(L"true");
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::falseRep() throw()
{
  return(L"false");
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::empty() throw()
{
  return(L"");
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::whitespace() throw()
{
  return(L" \t\v\f\r\n");
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::digits() throw()
{
  return(L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::intFormat() throw()
{
  return(L"%d");
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::uintFormat() throw()
{
  return(L"%u");
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::int64Format() throw()
{
  return(INT64_WIDE_FMT);
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::uint64Format() throw()
{
  return(UINT64_WIDE_FMT);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isAlpha(wchar_t c) throw()
{
  return(std::iswalpha(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  const wchar_t *BasicCharTraits<wchar_t>::doubleFormat() throw()
{
  return(L"%.*f");
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isAlphaNumeric(wchar_t c) throw()
{
  return((std::iswalpha(c) != 0) || (std::iswdigit(c) != 0));
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isWhitespace(wchar_t c) throw()
{
  return(std::iswspace(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isPrintable(wchar_t c) throw()
{
  return(std::iswprint(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isGraphical(wchar_t c) throw()
{
  return(std::iswgraph(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isNul(wchar_t c) throw()
{
  return(c == WNUL);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isControl(wchar_t c) throw()
{
  return(std::iswcntrl(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isDigit(wchar_t c) throw()
{
  return(std::iswdigit(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isSign(wchar_t c) throw()
{
  return((c == L'+') || (c == L'-'));
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isHexDigit(wchar_t c) throw()
{
  return(std::iswxdigit(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isLowerCase(wchar_t c) throw()
{
  return(std::iswlower(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  bool BasicCharTraits<wchar_t>::isUpperCase(wchar_t c) throw()
{
  return(std::iswupper(c) != 0);
}

/*
 */

template<> COMMONCPP_API
  wchar_t BasicCharTraits<wchar_t>::toUpperCase(wchar_t c) throw()
{
  return(static_cast<wchar_t>(std::towupper(c)));
}

/*
 */

template<> COMMONCPP_API
  const char *BasicCharTraits<wchar_t>::encoding() throw()
{
#ifdef CCXX_OS_MACOSX
  // workaround for libiconv problem
  return("UCS-4-INTERNAL//IGNORE");
#else
  return("WCHAR_T//IGNORE");
#endif
}

/*
 */

template<> COMMONCPP_API
  int BasicCharTraits<wchar_t>::toInt(wchar_t c) throw()
{
  return(std::iswdigit(c) ? static_cast<int>(c - L'0') : 0);
}

/*
 */

template<> COMMONCPP_API
  wchar_t BasicCharTraits<wchar_t>::fromInt(int v) throw()
{
  return(((v >= 0) && (v <= 9)) ? (L'0' + v) : L'0');
}

template class BasicCharTraits<wchar_t>;


}; // namespace ccxx

/* end of source file */
