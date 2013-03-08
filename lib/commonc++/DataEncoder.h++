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

#ifndef __ccxx_DataEncoder_hxx
#define __ccxx_DataEncoder_hxx

#include <commonc++/IOException.h++>

#include <deque>

namespace ccxx {

class DataEncoder; // fwd decl

/** An abstract class with basic functionality that is common to all
 * data decoders (readers) and data encoders (writers). A
 * DataEncoder operates on an input source or an output destination,
 * such as a stream or buffer. Subclasses provide specific
 * input/output functionality, such as reading and writing primitive
 * data types.
 *
 * The class supports nested encoding contexts. Initially, there is
 * a top-level context, which represents the entire range of bytes
 * being read or written. A nested context represents a sub-range of
 * bytes within its parent context. Each context has an offset and a
 * limit; the offset is the position, relative to the beginning of
 * the context, of the next byte to be read or written, and the
 * limit is the maximum offset within the context beyond which
 * further reading or writing is not allowed.
 *
 * A new context is created by calling pushContext(). The new
 * context inherits the encoder attributes&mdash;such as byte
 * endianness, value length, and string padding mode&mdash;of its
 * parent context, and its offset is initially 0. It represents the
 * range of bytes between the parent context's current offset and
 * limit.
 *
 * A context is destroyed by calling popContext(). The parent context
 * then becomes the current context, and its offset is incremented by the
 * offset of the popped context.
 *
 * Though data is typically read or written sequentially, random
 * access is also possible. The offset in the current context can be
 * changed insofar as it remains within the bounds of that
 * context. When reading, the offset can be set to any value between
 * 0 and the current context's limit. When writing, the offset
 * can be set to any value between 0 and the maximum offset to which
 * data has already been written within the current context.
 *
 * Nested contexts can be used to implement an encoder that reads or
 * writes hierarchical data structures, such as "chunks" in a RIFF file.
 *
 * @author Mark Lindner
 */

class COMMONCPP_API DataEncoder
{
  public:

  /** A base class for DataEncoder manipulators. A manipulator modifies the
   * behavior of the encoder in some way, and can be inlined with other
   * stream operators, in much the same way as the C++ standard library's
   * I/O manipulators work with C++ streams.
   *
   * @author Mark Lindner
   */

  class COMMONCPP_API Manipulator
  {
    public:

    /** Destructor. */
    virtual ~Manipulator() throw() { }

    /** Apply the DataManipulator to a DataEncoder.
     *
     * @param encoder The target DataEncoder.
     * @throw IOException If an I/O error occurs.
     */
    virtual void apply(DataEncoder &encoder) const throw(IOException) = 0;
  };

  /** A manipulator that specifies the endianness of integer values to be
   * subsequently encoded/decoded.
   *
   * @author Mark Lindner
   */

  class COMMONCPP_API SetEndianness : public Manipulator
  {
    public:

    /** Construct a new SetEndianness manipulator.
     *
     * @param endianness The endianness.
     */
    SetEndianness(Endianness endianness) throw();

    /** Destructor. */
    ~SetEndianness() throw();

    protected:

    void apply(DataEncoder &encoder) const throw(IOException);

    private:

    Endianness _endianness;
  };

  /** A manipulator that specifies the length of String, Blob, and array
   * values to be subsequently encoded/decoded. Depending on whether
   * string padding is enabled or disabled, encoded strings will be
   * padded with either space characters or NUL bytes, respectively.
   *
   * @author Mark Lindner
   */

  class COMMONCPP_API SetLength : public Manipulator
  {
    public:

    /** Construct a new SetLength manipulator.
     *
     * @param length The string length.
     */
    SetLength(size_t length) throw();

    /** Destructor. */
    ~SetLength() throw();

    protected:

    void apply(DataEncoder &encoder) const throw(IOException);

    private:

    size_t _length;
  };

  /** A manipulator that skips past bytes in the stream. When reading,
   * the bytes are simply skipped; when writing, the bytes are filled
   * with NULs.
   *
   * @author Mark Lindner
   */

  class COMMONCPP_API Skip : public Manipulator
  {
    public:

    /** Construct a new Skip manipulator.
     *
     * @param count The number of bytes to skip.
     */
    Skip(size_t count) throw();

    /** Destructor. */
    ~Skip() throw();

    protected:

    void apply(DataEncoder &encoder) const throw(IOException);

    private:

    size_t _count;
  };

  /** A manipulator that skips past enough bytes to re-align the
   * data stream at a specified boundary. When reading, the bytes
   * are simply skipped; when writing, the bytes are filled with
   * NULs.
   *
   * @author Mark Lindner
   */

  class COMMONCPP_API Align : public Manipulator
  {
    public:

    /** Construct a new Align manipulator.
     *
     * @param size The alignment size. Typical values include 2, 4, or 8.
     * Other values are allowed but are of limited usefulness.
     */
    Align(size_t size) throw();

    /** Destructor. */
    ~Align() throw();

    protected:

    void apply(DataEncoder &encoder) const throw(IOException);

    private:

    size_t _size;
  };

  /** A manipulator that specifies the maximum number of bytes that
   * can be read/written. Attempts to read/write past this limit will
   * raise an IOException.
   *
   * @author Mark Lindner
   */

  class COMMONCPP_API SetLimit : public Manipulator
  {
    public:

    /** Construct a new SetLimit manipulator.
     *
     * @param limit The limit, in bytes. A value of 0 indicates no limit.
     */
    SetLimit(int64_t limit) throw();

    /** Destructor. */
    ~SetLimit() throw();

    protected:

    void apply(DataEncoder &encoder) const throw(IOException);

    private:

    int64_t _limit;
  };

  /** A manipulator that enables or disables string padding. When enabled,
   * strings are padded with spaces during writing; otherwise they are
   * padded with NUL bytes.
   *
   * @author Mark Lindner
   */

  class COMMONCPP_API SetStringPadding : public Manipulator
  {
    public:

    /** Construct a new SetStringPadding manipulator.
     *
     * @param stringPadding A flag indicating the type of padding;
     * <b>true</b> for spaces, <b>false</b> for NUL bytes.
     */
    SetStringPadding(bool stringPadding) throw();

    /** Destructor. */
    ~SetStringPadding() throw();

    protected:

    void apply(DataEncoder &encoder) const throw(IOException);

    private:

    bool _stringPadding;
  };

  /** Reset the encoder. Discards all but the topmost context, and resets
   * the topmost context to default values.
   */
  virtual void reset() throw(IOException);

  /** Push a new encoder context. */
  void pushContext();

  /** Pop the active encoder context. The previous context becomes the
   * active context, and the offset in the current context is incremented
   * by the offset of the popped context.
   */
  void popContext();

  /** Determine if the active context is the topmost (and only) context. */
  bool isTopContext() const throw();

  /** Get the current context depth. */
  uint_t getContextDepth() const throw();

  /** Specify the byte-endianness for reading/writing primitive
   * integer types in the active context.
   */
  void setEndianness(Endianness endianness) throw();

  /** Get the byte-endianness in the active context. */
  Endianness getEndianness() const throw();

  /** Specify the length of String, Blob, and array values for
   * subsequent reading/writing in the active context. Depending
   * on whether string padding is enabled or disabled, written
   * strings will be padded with either space characters or NUL
   * bytes, respectively.
   */
  void setLength(size_t length) throw();

  /** Get the current length for string values. */
  size_t getLength() const throw();

  /** Specify the maximum number of bytes that can be
   * read/written in the active context. Attempts to
   * read/write past this limit will raise an IOException.
   */
  void setLimit(int64_t limit) throw(IOException);

  /** Get the current limit. */
  int64_t getLimit() const throw();

  /** Enables or disable string padding in the active context. When
   * enabled, strings are padded with spaces during writing;
   * otherwise they are padded with NUL bytes.
   *
   * @param stringPadding A flag indicating the type of padding;
   * <b>true</b> for spaces, <b>false</b> for NUL bytes.
   */
  void setStringPadding(bool stringPadding) throw();

  /** Get the current string padding status. */
  bool getStringPadding() const throw();

  /** Get the current encoding/decoding offset. */
  int64_t getOffset() const throw();

  /** Set the current reading/writing offset. */
  virtual void setOffset(int64_t offset) throw(IOException) = 0;

  /** Get the cumulative reading/writing offset. This is a sum of
   * the offsets across all contexts.
   */
  int64_t getCumulativeOffset() const throw();

  /** Get the number of bytes between the current offset and the
   * limit, i.e., the number of bytes remaining to be read or
   * written.
   */
  int64_t getRemaining() const throw();

  /** %Skip past enough bytes to re-align the data stream at a specified
   * boundary. When reading, the bytes are simply skipped over; when
   * writing, the bytes are filled with NULs.
   *
   * @param size The alignment size. Typical values include 2, 4, or 8.
   * Other values are allowed but are of limited usefulness.
   * @throw IOException If an I/O error occurs.
   */
  void align(size_t size) throw(IOException);

  /** %Skip past the given number of bytes in the data stream. When
   * reading, the bytes are simply skipped over; when writing, the
   * bytes are filled with NULs.
   *
   * @param count The number of bytes to skip.
   * @throw IOException If an I/O error occurs.
   */
  virtual void skip(size_t count) throw(IOException) = 0;

  /** Destructor. */
  virtual ~DataEncoder() throw();

  protected:

  /** Constructor. */
  DataEncoder() throw();

  /** Test if the endianness of the encoder is the same as the endianness
   * of the host system.
   */
  bool isSameEndianness() const throw();

  /** A DataEncoder context.
   *
   * @author Mark Lindner
   */

  struct Context
  {
    /** Constructor. */
    Context();

    void reset();

    void bumpOffset(size_t delta);

    /** The current byte endianness. */
    Endianness endianness;
    /** The current value length. */
    size_t length;
    /** The current limit. */
    int64_t limit;
    /** The parent (enclosing) limit. */
    int64_t parentLimit;
    /** The current offset. */
    int64_t offset;
    /** The maximum offset. */
    int64_t maxOffset;
    /** The current string padding mode. */
    bool stringPadding;
  };

  /** Get a reference to the current context. */
  Context &currentContext();

  /** Get a reference to the current context. */
  const Context &currentContext() const;

  /** Check if at least the given number of bytes are available to be
   * read or written.
   *
   * @param ctx The context to check.
   * @param count The number of bytes.
   * @throw IOException If there are not enough bytes remaining.
   */
  void checkRemaining(const Context &ctx, size_t count) const
    throw(IOException);

  class ContextStack; // fwd decl

  /** @cond INTERNAL */
  ContextStack *_contexts;
  int64_t _maxOffset;
  /** @endcond */

  private:

  CCXX_COPY_DECLS(DataEncoder);
};

}; // namespace ccxx

#endif // __ccxx_DataEncoder_hxx

/* end of header file */
