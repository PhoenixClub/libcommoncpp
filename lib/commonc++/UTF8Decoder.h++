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

#ifndef __ccxx_UTF8Decoder_hxx
#define __ccxx_UTF8Decoder_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/** A UTF-8 to UTF-16 string transcoder. A UTF-8 string is supplied to
 * the decoder via its constructor. The string can then be decoded to
 * UTF-16 by calling decode() one or more times; each time decode() is
 * called, it writes UTF-16 data to the supplied buffer, and returns
 * the number of code points actually written, returning 0 when all of
 * the UTF-8 data has been transcoded.
 * <p>
 * Since the String class provides facilities for converting text to/from
 * UTF-8, this class will generally not need to be used directly.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API UTF8Decoder
{
  public:

  /** Construct a new UTF8Decoder which will decode the given UTF-8 data.
   *
   * @param str A pointer to the string containing the UTF-8 data.
   * @param offset The offset within the string at which to begin decoding.
   * @param length The number of bytes to decode.
   */
  UTF8Decoder(const char *str, size_t offset = 0, size_t length = 0) throw();

  /** Destructor. */
  ~UTF8Decoder() throw();

  /** Transcode the UTF-8 data into UTF-16.
   *
   * @param buf The buffer in which to place the UTF-16 data. Does <i>not</i>
   * NUL-terminate the buffer.
   * @param length The length of the buffer.
   * @return The number of bytes decoded, or 0 if decoding is complete.
   */
  size_t decode(char16_t *buf, size_t length) throw();

  /** Calculate the transcoded length of the UTF-16 data, without actually
   * transcoding the string.
   *
   * @return The decoded length of the UTF-8 string, as a count of UTF-16
   * code points (including surrogates), not counting the NUL terminator.
   */
  size_t decodedLength() const throw();

  private:

  // Outputs the next unicode character as a single UTF-16 char or surrogate
  // pair. Returns false if there's not enough room in the output buffer.
  bool outputChar(char16_t **buf, size_t *len, uint32_t uchar) throw();

  const char *_str;
  size_t _offset;
  size_t _length;
  const char *_inp;
  size_t _left;
  char16_t _extra;
  int _needed;

  CCXX_COPY_DECLS(UTF8Decoder);
};

} // namespace ccxx

#endif // __ccxx_UTF8Decoder_hxx
