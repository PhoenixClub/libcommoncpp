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

#ifndef __ccxx_UTF8Encoder_hxx
#define __ccxx_UTF8Encoder_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/** A UTF-16 to UTF-8 string transcoder. A UTF-16 character array is
 * supplied to the encoder via its constructor. The array can then be
 * encoded into UTF-8 by calling encode() one or more times; each time
 * encode() is called, it writes UTF-8 data to the supplied buffer,
 * and returns the number of bytes actually written, returning 0 when
 * all of the UTF-16 data has been transcoded.
 * <p>
 * Since the String class provides facilities for converting text to/from
 * UTF-8, this class will generally not need to be used directly.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API UTF8Encoder
{
  public:

  /** Construct a new UTF8Encoder which will encode the given UTF-16 data.
   *
   * @param str A pointer to the character array containing the UTF-16 data.
   * @param offset The offset within the array at which to begin encoding.
   * @param length The number of UTF-16 characters to encode.
   */
  UTF8Encoder(const char16_t *str, size_t offset = 0, size_t length = 0)
    throw();

  /** Destructor. */
  ~UTF8Encoder() throw();

  /** Transcode the UTF-16 data into UTF-8.
   *
   * @param buf The buffer in which to place the UTF-8 data. Does <i>not</i>
   * NUL-terminate the buffer.
   * @param length The length of the buffer.
   * Wreturn Returns number of bytes encoded, or 0 if encoding is complete.
   */
  size_t encode(char *buf, size_t length) throw();

  /** Calculate the transcoded length of the UTF-8 data, without actually
   * transcoding the string.
   *
   * @return The decoded length of the UTF-8 string, in bytes, not counting
   * the NUL terminator.
   */
  size_t encodedLength() const throw();

  private:

  void outputChar(char **buf, size_t *length, uint32_t uchar) throw();

  const char16_t *_str;
  size_t _offset;
  size_t _length;
  const char16_t *_inp;
  size_t _left;
  char _extra[3];
  int _numExtra;

  CCXX_COPY_DECLS(UTF8Encoder);
};

} // namespace ccxx

#endif // __ccxx_UTF8Encoder_hxx
