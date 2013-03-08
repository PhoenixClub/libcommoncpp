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

#ifndef __ccxx_UString_hxx
#define __ccxx_UString_hxx

#include <commonc++/Common.h++>
#include <commonc++/AtomicCounter.h++>
#include <commonc++/UChar.h++>
#include <commonc++/CharRef.h++>
#include <commonc++/CString.h++>
#include <commonc++/Hash.h++>
#include <commonc++/OutOfBoundsException.h++>

#include <string>
#include <vector>
#include <cerrno>
#include <iosfwd>

#include <stdio.h> // remove

namespace ccxx {

class UStringVec;

/** A flexible, reference counted, copy-on-write, thread-safe, nullable
 * string. <b>This class will be renamed %String and will replace
 * BasicString<T> in a future release of commonc++</b>.
 *
 * @author Mark Lindner
 */

class COMMONCPP_API UString
{
  public:

  /** Construct a new, empty string. */
  UString();

  /** Construct a new string from a C string.
   *
   * @param str The C string to copy or inherit.
   * @param offset The number of characters from the beginning of the string
   * to skip over.
   * @param count The number of characters from the offset to use, or 0 to
   * use all the characters to the end of the string.
   * @param copy If <b>true</b>, the C string is copied into the BasicString;
   * otherwise the C string is inherited and the length parameter is ignored.
   */
  UString(char16_t *str, uint_t offset = 0, uint_t count = 0,
          bool copy = true);

  UString(const char16_t *str, uint_t offset = 0, uint_t count = 0);

  UString(const char *str, uint_t offset = 0, uint_t count = 0);

  /** Copy constructor. */
  UString(const UString &other);

  /** Destructor. */
  ~UString() throw();

  /** Test if the string ends with the given string.
   *
   * @param str The other string.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if this string ends with <i>str</i>, <b>false</b>
   * otherwise.
   */
  bool endsWith(const UString &str, bool ignoreCase = false) const throw();

  /** Test if the string ends with the given character.
   *
   * @param c The character.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if the string ends with <i>c</i>, <b>false</b>
   * otherwise.
   */
  bool endsWith(const UChar &c, bool ignoreCase = false) const throw();

  /** Test if the string ends with any character in a given set of
   * characters.
   *
   * @param chars The set of characters.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if the string ends with one of the characters
   * in the set, <b>false</b> otherwise.
   */
  bool endsWithAnyOf(const UString &chars, bool ignoreCase = false) const
    throw();

  /** Test if the string starts with the given string.
   *
   * @param str The other string.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if the BasicString begins with <i>str</i>,
   * <b>false</b> otherwise.
   */
  bool startsWith(const UString &str, bool ignoreCase = false) const throw();

  /** Test if the string starts with the given character.
   *
   * @param c The character.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if the string begins with <i>c</i>, <b>false</b>
   * otherwise.
   */
  bool startsWith(const UChar &c, bool ignoreCase = false) const throw();

  /** Test if the string starts with any character in a given set of
   * characters.
   *
   * @param chars The set of characters.
   * @param ignoreCase A flag indicating whether the comparison should be
   * case-insensitive.
   * @return <b>true</b> if the string starts with one of the
   * characters in the set, <b>false</b> otherwise.
   */
  bool startsWithAnyOf(const UString &chars, bool ignoreCase = false)
    const throw();

  /** Get the length of the string.
   *
   * @return The length, in characters, or 0 if the BasicString is
   * <b>null</b>.
   */
  uint_t getLength() const throw();

  /** Get the length of the string.
   *
   * @return The length, in characters, or 0 if the string is <b>null</b>.
   */
  inline uint_t length() const throw()
  { return(getLength()); }

  /** Set the length of the string. Allows the string to be
   * truncated to an arbitrary length.
   *
   * @param length The new length. The new length must be less than the
   * current length; otherwise the string is left unmodified.
   */
  void setLength(uint_t length);

  /** Reserve space in the string for the given number of characters. If the
   * new capacity is less than or equal to the current capacity, the string
   * is not modified.
   *
   * @param capacity The new capacity.
   */
  void reserve(uint_t capacity);

  /** Get the current capacity of the string, in characters. This value
   * takes into account the space required for the final NUL terminator.
   */
  uint_t getCapacity() const throw();

  /** Compact the string so that it occupies the minimum amount of memory
   * required.
   */
  void compact();

  /** Clear the string. Sets the length of the string to 0.
   *
   * @return The string.
   */
  inline UString& clear()
  {
    setLength(0);
    return(*this);
  }

  /** Test if the string is empty.
   *
   * @return <b>true</b> if the string is <b>null</b> or has length 0,
   * <b>false</b> otherwise.
   */
  bool isEmpty() const throw();

  /** Test if the string is <b>null</b>.
   *
   * @return <b>true</b> if the string is <b>null</b>, <b>false</b>
   * otherwise.
   */
  bool isNull() const throw();

  /** Test if the string holds an integer. For example, "123", "+5", and
   * "-123" are integers.
   *
   * @return <b>true</b> if the string is an integer, <b>false</b>
   * otherwise.
   */
  bool isInteger() const throw();

  /** Compare the string to another.
   *
   * @param other The string to compare to.
   * @param ignoreCase A flag indicating whether the compare should be
   * case insensitive.
   * @return A negative value if the string is lexicographically "less"
   * than <i>other</i>, a positive value if it is "greater" than
   * <i>other</i>, and 0 if they are "equal".
   */
  int compareTo(const UString &other, bool ignoreCase = false) const throw();

  /** Trim leading and trailing characters from the string.
   *
   * @return The string.
   */
  UString& trim(const UString &chars = whitespaceChars, bool begin = true,
               bool end = true);

  /** Trim characters from the beginning of the string.
   *
   * @param chars The set of characters to trim (the set of whitespace
   * characters by default).
   */
  inline UString& trimBegin(const UString &chars = whitespaceChars)
  { return(trim(chars, true, false)); }

  /** Trim characters from the end of the string.
   *
   * @param chars The set of characters to trim (the set of whitespace
   * characters by default).
   */
  inline UString& trimEnd(const UString &chars = whitespaceChars)
  { return(trim(chars, false, true)); }

  /** Truncate the string at the first occurrence of the given character.
   *
   * @param c The character at which to truncate.
   * @return The string.
   */
  UString& chop(const UChar &c);

  /** Truncate the string at the last occurrence of the given character.
   *
   * @param c The character at which to truncate.
   * @return The string.
   */
  UString& reverseChop(const UChar &c);

  /** Replace all occurences of one character with another.
   *
   * @param a The character to replace.
   * @param b The replacement character.
   * @return The string.
   */
  UString& replace(const UChar &a, const UChar &b);

  /** Replace all occurences of one substring with another.
   *
   * @param a The substring to replace.
   * @param b The replacement substring.
   * @return The string.
   */
  UString& replace(const UString &a, const UString &b);

  /** Insert a character into the string.
   *
   * @param c The character to insert.
   * @param index The index at which to insert the character. If the index
   * is out of range, the character is appended to the end of the string.
   */
  UString& insert(const UChar &c, uint_t index = 0);

  /** Insert a string into this string.
   *
   * @param str The string to insert.
   * @param index The index at which to insert the string. If the index
   * is out of range, the string is appended to the end of this string.
   * @param length The maximum number of characters from <i>str</i> to
   * insert.
   */
  UString& insert(const UString &str, uint_t index = 0, uint_t length = 0);

  UString& insert(const char16_t *str, uint_t index = 0, uint_t count = 0);

  inline UString& insert(const char *str, uint_t index = 0,
                         uint_t count = 0)
  { return(insert(UString(str), index, count)); }

  /** Append a string representation of a boolean value to the end of
   * the string.
   *
   * @param v The value to append.
   */
  UString& append(bool v);

  inline UString& append(char c)
  { return(append(UChar(c))); }

  /** Append a string representation of an integer value onto the end of
   * the string.
   *
   * @param val The value to append.
   */
  UString& append(int val);

  /** Append a string representation of an integer value in the
   * given base onto the end of the string.
   *
   * @param val The value to append.
   * @param base The numerical base in which to output the value. The valid
   * range is 2 - 36, inclusive.
   */
  UString& append(int val, uint_t base);

  /** Append a string representation of an unsigned integer value
   * onto the end of the string.
   *
   * @param val The value to append.
   */
  UString& append(uint_t val);

  /** Append a string representation of an unsigned integer value in the
   * given base onto the end of the string.
   *
   * @param val The value to append.
   * @param base The numerical base in which to output the value. The valid
   * range is 2 - 36, inclusive.
   */
  UString& append(uint_t val, uint_t base);

  /** Append a string representation of a long long (64-bit) integer onto
   * the end of the string.
   *
   * @param val The value to append.
   */
  UString& append(const int64_t &val);

  /** Append a string representation of double-precision floating point
   * value onto the end of the string.
   *
   * @param val The value to append.
   * @param precision The precision (the number of digits to include after
   * the radix).
   */
  UString& append(const double &val, uint_t precision = 6);

  /** Append a string representation of an unsigned long long
   * (64-bit) integer onto the end of the string.
   *
   * @param val The value to append.
   */
  UString& append(const uint64_t &val);

  /** Append a character to the end of the string.
   *
   * @param c The character to append.
   */
  inline UString& append(const UChar &c)
  { return(insert(c, END)); }

  /** Append a string to the end of the string.
   *
   * @param str The character to append.
   * @param count The maximum number of characters from <i>str</i> to
   * append, or 0 to append all of <i>str</i>.
   */
  inline UString& append(const UString &str, uint_t count = 0)
  { return(insert(str, END, count)); }

  inline UString& append(const char16_t *str, uint_t count = 0)
  { return(insert(str, END, count)); }

  /** Append a string to the end of the string.
   *
   * @param str The string to append.
   * @param count The maximum number of characters from <i>str</i> to
   * append, or 0 to append all of <i>str</i>.
   */
  inline UString& append(const char *str, uint_t count = 0)
  { return(insert(UString(str), END, count)); }

  /** Prepend a character to the beginning of the string.
   *
   * @param c The character to prepend.
   */
  inline UString& prepend(const UChar &c)
  { return(insert(c, 0)); }

  /** Prepend a string to the beginning of the string.
   *
   * @param str The character to prepend.
   * @param count The maximum number of characters from <i>str</i> to
   * prepend, or 0 to prepend all of <i>str</i>.
   */
  inline UString& prepend(const UString &str, uint_t count = 0)
  { return(insert(str, 0, count)); }

  inline UString& prepend(const char16_t *str, uint_t count = 0)
  { return(insert(str, 0, count)); }

  inline UString& prepend(const char *str, uint_t count = 0)
  { return(insert(UString(str), 0, count)); }

  /** Remove a portion of the string.
   *
   * @param fromIndex The start index of the range to remove.
   * @param toIndex The end index of the range to remove, or <i>END</i>
   * to remove to the end of the string.
   */
  UString& remove(uint_t fromIndex, uint_t toIndex = END);

  UString& setCharAt(uint_t index, const UChar &c) throw(OutOfBoundsException);

  /** Compute a hashcode for the string.
   *
   * @param modulo The desired range for the hashcode.
   * @return A hashcode in the range [0, <i>modulo</i>).
   */
  uint_t hash(uint_t modulo = 256) const throw();

  /** Extract a substring from the string.
   *
   * @param fromIndex The start index of the substring.
   * @param toIndex The end index of the substring, or <i>END</i> to indicate
   * the end of the string.
   * @return The substring as a new string. If fromIndex and toIndex are
   * equal, an empty string is returned.
   */
  UString substring(uint_t fromIndex, uint_t toIndex = END) const;

  /** Find the first occurrence of a given character in the string.
   *
   * @param c The character to search for.
   * @param fromIndex The start index from which to begin searching.
   * @return The index of the first occurrence of the character, or -1
   * if the character was not found.
   */
  int indexOf(const UChar& c, uint_t fromIndex = 0) const throw();

  /** Find the first occurrence of a given string in this string.
   *
   * @param str The string to search for.
   * @param fromIndex The start index from which to begin searching.
   * @return The index of the first occurrence of the string, or -1
   * if the string was not found.
   */
  inline int indexOf(const char *str, uint_t fromIndex = 0) const throw()
  { return(indexOf(UString(str), fromIndex)); }

  /** Find the first occurrence in this string of any character in a set
   * of characters.
   *
   * @param chars The set of characters search for.
   * @param fromIndex The start index from which to begin searching.
   * @return The index of the first occurrence of any character in the set,
   * or -1 if no occurrence was not found.
   */
  int indexOfAnyOf(const UString &chars, uint_t fromIndex = 0) const throw();

  /** Find the first occurrence of a given string in this string.
   *
   * @param str The string to search for.
   * @param fromIndex The start index from which to begin searching.
   * @return The index of the first occurrence of the string, or -1
   * if the string was not found.
   */
  int indexOf(const UString &str, uint_t fromIndex = 0) const throw();

  /** Find the last occurrence of a given character in this string.
   *
   * @param c The character to search for.
   * @param fromIndex The index from which to begin searching backwards,
   * or <b>END</b> to start at the end of the string.
   * @return The index of the last occurrence of the character, or -1
   * if the character was not found.
   */
  int lastIndexOf(const UChar &c, uint_t fromIndex = END) const throw();

  /** Find the last occurrence in the string of any character in a set
   * of characters.
   *
   * @param chars The set of characters search for.
   * @param fromIndex The index from which to begin searching backwards,
   * or <b>END</b> to start at the end of the string.
   * @return The index of the last occurrence of any character in the set,
   * or -1 if no occurrence was not found.
   */
  int lastIndexOfAnyOf(const UString &chars, uint_t fromIndex = END) const
    throw();

  /** Find the last occurrence of a given string in this string.
   *
   * @param str The string to search for.
   * @param fromIndex The index from which to begin searching backwards,
   * or <b>END</b> to start at the end of the string.
   * @return The index of the last occurrence of the string, or -1
   * if the string was not found.
   */
  int lastIndexOf(const UString &str, uint_t fromIndex = END) const throw();

  /** Determine if the string contains a given character.
   *
   * @param c The character to search for.
   * @return <b>true</b> if the string contains at least one occurrence of
   * the character, <b>false</b> otherwise.
   */
  inline bool contains(const UChar &c) const throw()
  { return(indexOf(c) >= 0); }

  /** Determine if the string contains a given substring.
   *
   * @param str The substring to search for.
   * @return <b>true</b> if the string contains at least one occurrence of
   * the substring, <b>false</b> otherwise.
   */
  inline bool contains(const UString &str) const throw()
  { return(indexOf(str) >= 0); }

  /** Determine if the string contains any character in a given set of
   * characters.
   *
   * @param chars The set of characters.
   * @return <b>true</b> if the string contains at least one occurrence of
   * any character in the set, <b>false</b> otherwise.
   */
  inline bool containsAnyOf(const char *chars) const throw()
  { return(indexOfAnyOf(chars) >= 0); }

  /** Extract a token from the string.
   *
   * @param fromIndex The start index at which to begin searching for a
   * token. Will be modified on return to point to one character past the
   * extracted token.
   * @param delim The set of possible token delimiter characters (the set
   * of whitespace characters by default).
   * @return The extracted token, or <b>String::null</b> if the end of the
   * string has been reached.
   */
  UString nextToken(uint_t &fromIndex, const UString &delim = whitespaceChars)
    const;

  /** Split the string into a series of tokens, without modifying the
   * string.
   *
   * @param tokens The vector in which to place the extracted tokens. If
   * a non-empty vector is supplied, the tokens are added to the end of
   * the vector.
   * @param delim The set of possible token delimiter characters (the set
   * of whitespace characters by default).
   * @param limit The maximum number of tokens to extract, or 0 for no
   * limit (the default).
   * @return The number of tokens extracted.
   */
  uint_t split(UStringVec &tokens, const UString &delim = whitespaceChars,
               uint_t limit = 0) const;

  /** Convert the string to lower case. */
  UString& toLowerCase();

  /** Convert the string to upper case. */
  UString& toUpperCase();

  /** Fill the string with a character.
   *
   * @param c The fill character.
   * @param count The number of characters to fill.
   */
  UString& fill(const UChar &c, uint_t count);

  /** Pad the string with a character.
   *
   * @param c The pad character.
   * @param count The number of characters to pad.
   */
  UString& pad(const UChar &c, uint_t count);

  /** Parse a boolean value from the string.
   *
   * @param index The index to begin parsing at.
   * @return The parsed value.
   */
  bool toBool(uint_t index = 0) const throw();

  /** Parse an integer from the string.
   *
   * @param index The index to begin parsing at.
   * @param base The base (2 - 36).
   * @return The parsed value.
   */
  int toInt(uint_t index = 0, uint_t base = 10) const throw();

  /** Parse an unsigned integer from the string.
   *
   * @param index The index to begin parsing at.
   * @param base The base (2 - 36).
   * @return The parsed value.
   */
  uint_t toUInt(uint_t index = 0, uint_t base = 10) const throw();

  /** Parse a 64-bit integer from the string.
   *
   * @param index The index to begin parsing at.
   * @param base The base (2 - 36).
   * @return The parsed value.
   */
  int64_t toInt64(uint_t index = 0, uint_t base = 10) const throw();

  /** Parse an unsigned 64-bit integer from the string.
   *
   * @param index The index to begin parsing at.
   * @param base The base (2 - 36).
   * @return The parsed value.
   */
  uint64_t toUInt64(uint_t index = 0, uint_t base = 10) const throw();

  /** Parse a floating point value from the string.
   *
   * @param index The index to begin parsing at.
   * @return The parsed value.
   */
  float toFloat(uint_t index = 0) const throw();

  /** Parse a double precision floating point value from the string.
   *
   * @param index The index to begin parsing at.
   * @return The parsed value.
   */
  double toDouble(uint_t index = 0) const throw();

  /** Pointer cast operator. */
  operator const void *() const throw();

  const char16_t* data() const throw();

  /** Return the contents of the string as an immutable C string. */
  CString toUTF8() const;

  /** Assignment operator. */
  UString& operator=(const char *other);

  /** Assignment operator. */
  UString& operator=(const UString &other);

  /** Assignment operator. */
  inline UString& operator=(const std::string &other)
  { return(operator=(other.c_str())); }

  /** %Array index operator. Obtain a reference to the character at the given
   * offset in the string.
   *
   * @param index The index.
   * @return A reference to the character at the specified index.
   * @throw OutOfBoundsException If <i>index</I> is out of range.
   */
  CharRef operator[](int index) throw(OutOfBoundsException);

  /** %Array index operator. Obtain a copy of the character at the given
   * offset in the string.
   *
   * @param index The index.
   * @return The character at the specified index, as a UChar.
   * @throw OutOfBoundsException If <i>index</I> is out of range.
   */
  const UChar operator[](int index) const throw(OutOfBoundsException);

  /** Substring operator. Extract a substring from the string.
   *
   * @param fromIndex The start index of the substring.
   * @param toIndex The end index of the substring, or <i>END</i> to indicate
   * the end of the string.
   * @return The substring as a new string. If fromIndex and toIndex are
   * equal, an empty string is returned.
   */
  inline UString operator()(uint_t fromIndex, uint_t toIndex = END) const
  { return(substring(fromIndex, toIndex)); }

  /** Unary NOT operator.
   *
   * @return <b>true</b> if the string is <b>null</b>, <b>false</b>
   * otherwise.
   */
  inline bool operator!() const throw()
  { return(isNull()); }

  /** Append a character onto the end of the string.
   *
   * @param c The character to append.
   */
  inline UString& operator+=(const UChar &c)
  { return(append(c)); }

  /** Append a string onto the end of this string.
   *
   * @param str The string to append.
   */
  inline UString& operator+=(const char *str)
  { return(append(str)); }

  /** Append a string onto the end of this string.
   *
   * @param str The string to append.
   */
  inline UString& operator+=(const UString &str)
  { return(append(str)); }

  /** Append a string representation of a boolean value onto the end of
   * the string.
   *
   * @param val The value to append.
   */
  inline UString& operator+=(bool val)
  { return(append(val)); }

  /** Append a string representation of an integer value onto the end of
   * the string.
   *
   * @param val The value to append.
   */
  inline UString& operator+=(int val)
  { return(append(val)); }

  /** Append a string representation of an unsigned integer value onto the
   * end of the string.
   *
   * @param val The value to append.
   */
  inline UString& operator+=(uint_t val)
  { return(append(val)); }

  /** Append a string representation of double-precision floating point
   * value onto the end of the string.
   *
   * @param val The value to append.
   */
  inline UString& operator+=(double val)
  { return(append(val)); }

  /** Append a string representation of a long long (64-bit) integer onto
   * the end of the string.
   *
   * @param val The value to append.
   */
  inline UString& operator+=(int64_t val)
  { return(append(val)); }

  /** Append a string representation of an unsigned long long
   * (64-bit) integer onto the end of the string.
   *
   * @param val The value to append.
   */
  inline UString& operator+=(uint64_t val)
  { return(append(val)); }

  /** Append operator. */
  inline UString& operator<<(const char *str)
  { return(append(str)); }

  /** Append operator. */
  inline UString& operator<<(const UString &str)
  { return(append(str)); }

  /** Append operator. */
  inline UString& operator<<(bool val)
  { return(append(val)); }

  /** Append operator. */
  inline UString& operator<<(const UChar &c)
  { return(append(c)); }

  /** Append operator. */
  inline UString& operator<<(int val)
  { return(append(val)); }

  /** Append operator. */
  inline UString& operator<<(uint_t val)
  { return(append(val)); }

  /** Append operator. */
  inline UString& operator<<(const int64_t &val)
  { return(append(val)); }

  /** Append operator. */
  inline UString& operator<<(const uint64_t &val)
  { return(append(val)); }

  /** Append operator. */
  inline UString& operator<<(const double &val)
  { return(append(val)); }

  /** Prepend operator. */
  inline UString& operator^=(const UChar &c)
  { return(prepend(c)); }

  /** Prepend operator. */
  inline UString& operator^=(const char *str)
  { return(prepend(str)); }

  /** Prepend operator. */
  inline UString& operator^=(const UString &str)
  { return(prepend(str)); }

  UString operator+(const UString &str);
  UString operator+(const char *str);
  UString operator+(const UChar &c);

  /** Write the string (in UTF-8 form) to a stream. */
  void write(std::ostream& stream) const;

  /** A pseudo-index indicating the end of the string. */
  static const uint_t END;

  /** The <b>null</b> string. */
  static const UString null;

  /** The empty string. */
  static const UString empty;

  /** The set of whitespace delimiter characters. */
  static const UString whitespaceChars;

  private:

  class StringBuf;

  StringBuf* _buf;

  void _insert(const char16_t *str, uint_t length, uint_t index, uint_t count);
  void _release();
  void _makeCopy(size_t size, bool unshareable = false);
  char16_t* _makeGap(uint_t index, uint_t gaplen);
  void _fromUTF8(const char *str, uint_t len, uint_t offset);
};

/** A UString vector. A trivial extension of
 * <b>std::vector<String ></b>.
 *
 * @author Mark Lindner
 */

class UStringVec : public std::vector<UString>
{
  public:

  UStringVec()
  { }

  /** Append operator. Pushes a string onto the end (back) of the vector. */
  inline UStringVec& operator<<(UString s)
  { push_back(s); return(*this); }

  /** An empty UStringVec. */
  static const UStringVec emptyVec;
};

inline bool operator==(const UString &s1, const UString &s2) throw()
{ return(s1.compareTo(s2) == 0); }

inline bool operator==(const UString &s1, const char *s2) throw()
{ return(s1.compareTo(s2) == 0); }

inline bool operator==(const char *s1, const UString &s2) throw()
{ return(s2.compareTo(s1) == 0); }

inline bool operator!=(const UString &s1, const UString &s2) throw()
{ return(s1.compareTo(s2) != 0); }

inline bool operator!=(const UString &s1, const char *s2) throw()
{ return(s1.compareTo(s2) != 0); }

inline bool operator!=(const char *s1, const UString &s2) throw()
{ return(s2.compareTo(s1) != 0); }

inline bool operator<(const UString &s1, const UString &s2) throw()
{ return(s1.compareTo(s2) < 0); }

inline bool operator<(const UString &s1, const char *s2) throw()
{ return(s1.compareTo(s2) < 0); }

inline bool operator<(const char *s1, const UString &s2) throw()
{ return(s2.compareTo(s1) > 0); }

inline bool operator>(const UString &s1, const UString &s2) throw()
{ return(s1.compareTo(s2) > 0); }

inline bool operator>(const UString &s1, const char *s2) throw()
{ return(s1.compareTo(s2) > 0); }

inline bool operator>(const char *s1, const UString &s2) throw()
{ return(s2.compareTo(s1) < 0); }

inline bool operator<=(const UString &s1, const UString &s2) throw()
{ return(s1.compareTo(s2) <= 0); }

inline bool operator<=(const UString &s1, const char *s2) throw()
{ return(s1.compareTo(s2) <= 0); }

inline bool operator<=(const char *s1, const UString &s2) throw()
{ return(s2.compareTo(s1) > 0); }

inline bool operator>=(const UString &s1, const UString &s2) throw()
{ return(s1.compareTo(s2) >= 0); }

inline bool operator>=(const UString &s1, const char *s2) throw()
{ return(s1.compareTo(s2) >= 0); }

inline bool operator>=(const char *s1, const UString &s2) throw()
{ return(s2.compareTo(s1) < 0); }

inline UString operator+(const char *s1, const UString &s2)
{ return(UString(s1) + s2); }

inline UString operator+(const UChar &c, const UString &s)
{ UString t; t += c; t += s; return(t); }

inline std::ostream& operator<<(std::ostream &stream, const UString &s)
{ s.write(stream); return(stream); }

} // namespace ccxx

#endif // __ccxx_String_hxx
