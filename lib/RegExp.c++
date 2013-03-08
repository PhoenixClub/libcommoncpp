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

#include "commonc++/RegExp.h++"

#ifdef CCXX_OS_WINDOWS_GNU
#define PCRE_STATIC
#endif

#include "pcreposix.h"

namespace ccxx {

/*
 */

RegExp::RegExp() throw()
  : _pattern(String::null),
    _compiled(false)
{
}

/*
 */

RegExp::RegExp(const RegExp& other) throw(ParseException)
  : _compiled(false)
{
  setPattern(other._pattern);
}

/*
 */

RegExp::~RegExp() throw()
{
  if(_compiled)
    regfree((regex_t *)(void *)&_regex);
}

/*
 */

RegExp& RegExp::operator=(const RegExp& other)
{
  if(this != &other)
    setPattern(other._pattern);

  return(*this);
}

/*
 */

void RegExp::setPattern(const String &pattern,
                        bool caseInsensitive /* = false */)
  throw(ParseException)
{
  setPattern(pattern.c_str(), caseInsensitive);
}

/*
 */

void RegExp::setPattern(const char *pattern, bool caseInsensitive /* = false*/)
  throw(ParseException)
{
  if(_compiled)
  {
    regfree((regex_t *)(void *)&_regex); // free old pattern
    _compiled = false;
  }

  if(pattern == NULL)
  {
    _pattern = String::null;
    return;
  }

  int flags = REG_EXTENDED;

  if(caseInsensitive)
    flags |= REG_ICASE;

  int r = regcomp((regex_t *)(void *)&_regex, pattern, flags);

  if(r != 0)
  {
    char errorBuf[128];
    regerror(r, (regex_t *)(void *)&_regex, errorBuf, sizeof(errorBuf));

    throw ParseException(errorBuf);
  }
  else
  {
    _compiled = true;
    _pattern = pattern;
  }
}

/*
 */

bool RegExp::match(const String &text) const throw()
{
  return(match(text.c_str()));
}

/*
 */

bool RegExp::match(const char *text) const throw()
{
  if(! _compiled)
    return(false);

  int r = regexec((regex_t *)(void *)&_regex, text, 0, NULL, REG_EXTENDED);

  return(r == 0);
}

/*
 */

bool RegExp::match(const char *text, RegExpMatch matches[], int numMatches)
  const throw()
{
  if(! _compiled)
    return(false);

  int r = regexec((regex_t *)(void *)&_regex, text,
                  static_cast<size_t>(numMatches), (regmatch_t *)matches,
                  REG_EXTENDED);

  return(r == 0);
}

/*
 */

bool RegExp::match(const String &text, RegExpMatch matches[], int numMatches)
  const throw()
{
  return(match(text.c_str(), matches, numMatches));
}

/*
 */

String RegExp::escapeMeta(const String &text)
{
  if(text.isEmpty())
    return(text);

  String s;

  for(uint_t i = 0; i < text.length(); ++i)
  {
    char c = text[i];

    if(!(CharTraits::isAlphaNumeric(c) || (c == '_') || ((c & 0x80) != 0)))
      s += '\\';

    s += c;
  }

  return(s);
}

}; // namespace ccxx

/* end of source file */
