#ifndef __ccxx_UChar_hxx
#define __ccxx_UChar_hxx

#include <commonc++/Common.h++>

namespace ccxx {

/**
 * A class representing a UTF-16 character. <b>This class will be
 * renamed %Char and will replace BasicChar<T> in a future release of
 * commonc++</b>.
 */
class COMMONCPP_API UChar
{
  public:

  /** Construct a new, NUL character. */
  UChar();

  /** Construct a new character with the given ASCII value. */
  explicit UChar(char c);

  /** Construct a new character with the given UTF-16 value. */
  explicit UChar(char16_t c);

  /** Construct a new character with the given unicode value. Values above
   * 0xFFFF are truncated. */
  UChar(int c);

  /** Destructor. */
  ~UChar();

  /** Assignment operator. */
  UChar& operator=(char c) throw();

  /** Assignment operator. */
  UChar& operator=(char16_t c) throw();

  /** Assignment operator. */
  UChar& operator=(int c) throw();

  /** Tests if the character is NUL. */
  bool isNul() const throw();

  /** Tests if the character is a letter. */
  bool isLetter() const throw();

  /** Tests if the character is a digit. */
  bool isDigit() const throw();

  /** Tests if the character is a sign symbol (+ or -). */
  bool isSign() const throw();

  /** Tests if the character is whitespace. */
  bool isWhitespace() const throw();

  /** Tests if the character is a lowercase letter. */
  bool isLowerCase() const throw();

  /** Tests if the character is an uppercase letter. */
  bool isUpperCase() const throw();

  /** Returns the lowercase form of the character */
  UChar toLowerCase() const throw();

  /** Returns the uppercase form of the character */
  UChar toUpperCase() const throw();

  /** Tests if the character is a low surrogate. */
  bool isLowSurrogate() const throw();

  /** Tests if the character is a high surrogate. */
  bool isHighSurrogate() const throw();

  /** Cast operator. */
  inline operator char16_t() const throw()
  { return(_char); }

  /** Equivalent to isNul(). */
  inline bool operator!() const throw()
  { return(isNul()); }

  /** The NUL character. */
  static const UChar nul;

  /** Tests if a character is NUL. */
  static bool isNul(char16_t c) throw();

  /** Tests if the character is a letter. */
  static bool isLetter(char16_t c) throw();

  /** Tests if a character is a digit. */
  static bool isDigit(char16_t c) throw();

  /** Tests if a character is a sign symbol (+ or -). */
  static bool isSign(char16_t c) throw();

  /** Tests if a character is whitespace. */
  static bool isWhitespace(char16_t c) throw();

  /** Tests if a character is a lowercase letter. */
  static bool isLowerCase(char16_t c) throw();

  /** Tests if a character is an uppercase letter. */
  static bool isUpperCase(char16_t c) throw();

  /** Returns the lowercase form of a character */
  static char16_t toLowerCase(char16_t c) throw();

  /** Returns the uppercase form of a character */
  static char16_t toUpperCase(char16_t c) throw();

  /** Tests if a character is a low surrogate. */
  static bool isLowSurrogate(char16_t c) throw();

  /** Tests if a character is a high surrogate. */
  static bool isHighSurrogate(char16_t c) throw();

  /** Returns the character in ASCII, or NUL if the character is not in the
   * range of ASCII characters.
   */
  static char toASCII(char16_t c) throw();

  private:

  char16_t _char;
};

} // namespace ccxx

#endif // __ccxx_UChar_hxx
