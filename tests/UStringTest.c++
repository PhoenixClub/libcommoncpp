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

#include "UStringTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/UChar.h++"
#include "commonc++/UString.h++"
#include "commonc++/Hex.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(UStringTest);

/*
 */

CppUnit::Test *UStringTest::suite()
{
  CCXX_TESTSUITE_BEGIN(UStringTest);
  CCXX_TESTSUITE_TEST(UStringTest, testEquals);
  CCXX_TESTSUITE_TEST(UStringTest, testAppend);
  CCXX_TESTSUITE_TEST(UStringTest, testInsert);
  CCXX_TESTSUITE_TEST(UStringTest, testUpperLowerCase);
  CCXX_TESTSUITE_TEST(UStringTest, testNumeric);
  CCXX_TESTSUITE_TEST(UStringTest, testTrim);
  CCXX_TESTSUITE_TEST(UStringTest, testReplace);
  CCXX_TESTSUITE_TEST(UStringTest, testNull);
  CCXX_TESTSUITE_TEST(UStringTest, testPadFill);
  CCXX_TESTSUITE_TEST(UStringTest, testPrintf);
  CCXX_TESTSUITE_TEST(UStringTest, testReserve);
  CCXX_TESTSUITE_TEST(UStringTest, testTokenize);
  CCXX_TESTSUITE_END();
}

/*
 */

void UStringTest::setUp()
{
  _s0 = "Hello";
  _s1 = "Hello, world!";
}

/*
 */

void UStringTest::tearDown()
{
}

/*
 */

void UStringTest::testEquals()
{
  UString original = _s1;
  const char *c_cmp = "Hello, world!";
  UString cmp = "Hello, world!";

  CPPUNIT_ASSERT_EQUAL(0, original.compareTo(cmp));
  CPPUNIT_ASSERT_EQUAL(0, original.compareTo(c_cmp));
}

/*
 */

void UStringTest::testAppend()
{
  UString bar("bar");
  CPPUNIT_ASSERT(bar == "bar");

  UString app = ", world!";

  UString s = _s0;
  s.append(", world!");

  CPPUNIT_ASSERT(s == _s1);

  s = _s0;
  s.append(app);

  CPPUNIT_ASSERT(s == _s1);

  s = _s0;
  s.append(123456);
  CPPUNIT_ASSERT_EQUAL(UString("Hello123456"), s);

  s = _s0;
  s.append('!');
  CPPUNIT_ASSERT_EQUAL(UString("Hello!"), s);

  s = _s0;
  s.append(3.1415, 4);
  CPPUNIT_ASSERT_EQUAL(UString("Hello3.1415"), s);

  s = _s0;
  s.append(false);
  CPPUNIT_ASSERT_EQUAL(UString("Hellofalse"), s);

  UChar c = '!';
  s.append(c);
  CPPUNIT_ASSERT_EQUAL(UString("Hellofalse!"), s);
}

/*
 */

void UStringTest::testInsert()
{
  UString s = "defghi";

  s.insert("abc", 0, 3);

  CPPUNIT_ASSERT_EQUAL(UString("abcdefghi"), s);

  s.insert("jkXX", UString::END, 2);

  CPPUNIT_ASSERT_EQUAL(UString("abcdefghijk"), s);

  s.insert("lm", UString::END);

  CPPUNIT_ASSERT_EQUAL(UString("abcdefghijklm"), s);

  s.insert("123456", 5);

  CPPUNIT_ASSERT_EQUAL(UString("abcde123456fghijklm"), s);
}

/*
 */

void UStringTest::testUpperLowerCase()
{
  UString original = "12345 Hello, !@#$% world!";

  UString s = original;
  s.toLowerCase();

  CPPUNIT_ASSERT_EQUAL(UString("12345 hello, !@#$% world!"), s);

  s = original;
  s.toUpperCase();

  CPPUNIT_ASSERT_EQUAL(UString("12345 HELLO, !@#$% WORLD!"), s);
}

/*
 */

void UStringTest::testNumeric()
{
  static const char *s_bool[] = { "true", "True", "TRUE", "false", "FaLsE",
                                  "blah" "1", NULL };

  static const bool v_bool[] = { true, true, true, false, false, false,
                                 false };

  static const char *s_int[] = { "0", "123456789", "-", "+555", "blah",
                                 "+-+-+", NULL };

  static const int v_int[] = { 0, 123456789, 0, 555, 0, 0 };

  static const char *s_double[] = { "1E6", "1.1", "0", "+-+-+", "E",
                                    "3.141592654", "-1.1E-0003", NULL };

  static const double v_double[] = { 1E6, 1.1, 0, 0, 0, 3.141592654, -1.1E-3 };

  const char **p;
  int i;

  for(i = 0, p = s_bool; *p; p++, i++)
  {
    UString s = *p;
    bool val = s.toBool();

    CPPUNIT_ASSERT_EQUAL(v_bool[i], val);
  }

  for(i = 0, p = s_int; *p; p++, i++)
  {
    UString s = *p;
    int val = s.toInt();

    CPPUNIT_ASSERT_EQUAL(v_int[i], val);
  }

  for(i = 0, p = s_double; *p; p++, i++)
  {
    UString s = *p;
    double val = s.toDouble();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(v_double[i], val, .00001);
  }
}

/*
 */

void UStringTest::testTrim()
{
  UString s = "   Hello, world!\t\n";
  s.trim();

  CPPUNIT_ASSERT_EQUAL(_s1, s);

  UString s2 = "Hello, world!";
  s2.trim();

  CPPUNIT_ASSERT_EQUAL(_s1, s2);
}

/*
 */

void UStringTest::testReplace()
{
  UString s = "Test this test is a test String xxtestxx. test";

  s.replace("test", "ZZZ");

  CPPUNIT_ASSERT_EQUAL(UString("Test this ZZZ is a ZZZ String xxZZZxx. ZZZ"),
                       s);

  s = "rEplacE all uppErcasE E charactErs";

  s.replace('E', 'e');

  CPPUNIT_ASSERT_EQUAL(UString("replace all uppercase e characters"), s);
}

/*
 */

void UStringTest::testPrepend()
{
  UString s = "World!";
  s.prepend("ello, ");
  s.prepend('H');

  CPPUNIT_ASSERT_EQUAL(UString("Hello, World!"), s);
}

/*
 */

void UStringTest::testRemove()
{
  UString s = "A quick brown fox jumped over the lazy dog.";

  s.remove(2, 7);
  CPPUNIT_ASSERT_EQUAL(UString("A brown fox jumped over the lazy dog."), s);

  s.remove(18);
  CPPUNIT_ASSERT_EQUAL(UString("A brown fox jumped"), s);

  s.remove(0, 0);
  CPPUNIT_ASSERT_EQUAL(UString("A brown fox jumped"), s);

  s.remove(0, 1);
  CPPUNIT_ASSERT_EQUAL(UString(" brown fox jumped"), s);

  s.remove(100, 1);
  CPPUNIT_ASSERT_EQUAL(UString(" brown fox jumped"), s);

  s.remove(16, 100);
  CPPUNIT_ASSERT_EQUAL(UString(" brown fox jumpe"), s);
}

/*
 */

void UStringTest::testIndexOf()
{
  UString s = "A quick brown fox jumped over the lazy dog.";

  CPPUNIT_ASSERT_EQUAL(2, s.indexOf("quick"));
  CPPUNIT_ASSERT_EQUAL(34, s.indexOf("lazy"));
  CPPUNIT_ASSERT_EQUAL(42, s.indexOf('.'));
  CPPUNIT_ASSERT_EQUAL(-1, s.indexOf('?'));
  CPPUNIT_ASSERT_EQUAL(-1, s.indexOf("dogg"));
}

/*
 */

void UStringTest::testContains()
{
  UString s = "A quick brown fox jumped over the lazy dog.";

  CPPUNIT_ASSERT_EQUAL(true, s.contains("fox"));
  CPPUNIT_ASSERT_EQUAL(false, s.contains('@'));
  CPPUNIT_ASSERT_EQUAL(true, s.contains('x'));
  CPPUNIT_ASSERT_EQUAL(true, s.contains("dog."));
}

/*
 */

void UStringTest::testIndex()
{
  UString s = "123 456";

//  CPPUNIT_ASSERT_EQUAL(UChar('1'), s[0]);
//  CPPUNIT_ASSERT_EQUAL(NUL, s[100]);

  s[3] = 0;
  CPPUNIT_ASSERT(s == "1230456");

  try
  {
    s[100] = 'x';
    CPPUNIT_FAIL("No OutOfBoundsException thrown");
  }
  catch(OutOfBoundsException& ex)
  {
    // expected
  }
}

/*
 */

void UStringTest::testStartsEndsWith()
{
  UString s = "123 @@@ xyz";

  CPPUNIT_ASSERT(s.startsWith('1'));
  CPPUNIT_ASSERT(s.startsWith("123"));

  CPPUNIT_ASSERT(s.endsWith('z'));
  CPPUNIT_ASSERT(s.endsWith(" xyz"));
}

/*
 */

void UStringTest::testNull()
{
  CPPUNIT_ASSERT_EQUAL(true, UString::null.isNull());

  UString s = UString::null;

  CPPUNIT_ASSERT_EQUAL(true, s.isNull());

  UString t;

  CPPUNIT_ASSERT_EQUAL(true, t.isNull());
  CPPUNIT_ASSERT(s == t);

  s += "foo";
  CPPUNIT_ASSERT_EQUAL(UString("foo"), s);

  s = NULL;
  CPPUNIT_ASSERT_EQUAL(true, s.isNull());
}

/*
 */

void UStringTest::testPadFill()
{
  UString s = "Hello, world!";
  s.pad(' ', 5);

  CPPUNIT_ASSERT_EQUAL(UString("Hello, world!     "), s);

  s.fill('!', 10);

  CPPUNIT_ASSERT_EQUAL(UString("!!!!!!!!!!"), s);
}

/*
 */

void UStringTest::testPrintf()
{
  /*
  String s;
  int val = 20;
  s.printf("test %04u", val);
  CPPUNIT_ASSERT_EQUAL(String("test 0020"), s);

  s.clear();
  s.printf("another %04u", ++val);
  CPPUNIT_ASSERT_EQUAL(String("another 0021"), s);

  for(int i = 0; i < 200; ++i)
  {
    s.printf("here's some more %s #%d. ", "text to fill the buffer with", i);
  }
  CPPUNIT_ASSERT_EQUAL(10302U, s.length());
  */
}

/*
 */

void UStringTest::testReserve()
{
  UString s;
  s = "abc";
  s.reserve(30);
  s += "def";

  CPPUNIT_ASSERT_EQUAL(32U, s.getCapacity());

  for(int i = 0; i < 10; ++i)
  {
    s += "ABCDEF";
  }
  CPPUNIT_ASSERT_EQUAL(72U, s.getCapacity());

  CPPUNIT_ASSERT_EQUAL(UString("abcdefABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF"), s);
}

/*
 */

void UStringTest::testConcat()
{
  UString s;
  s.clear() << "abc" << "123";

  CPPUNIT_ASSERT_EQUAL(UString("abc123"), s);
}

/*
 */

void UStringTest::testTokenize()
{
  UString s = "The quick brown fox, jumped over...the lazy dog!";

  UStringVec tokens;
  uint_t n = s.split(tokens, " .,!");
  CPPUNIT_ASSERT_EQUAL(9U, n);
  CPPUNIT_ASSERT_EQUAL(9U, tokens.size());

  CPPUNIT_ASSERT_EQUAL(UString("The"), tokens[0]);
  CPPUNIT_ASSERT_EQUAL(UString("quick"), tokens[1]);
  CPPUNIT_ASSERT_EQUAL(UString("brown"), tokens[2]);
  CPPUNIT_ASSERT_EQUAL(UString("fox"), tokens[3]);
  CPPUNIT_ASSERT_EQUAL(UString("jumped"), tokens[4]);
  CPPUNIT_ASSERT_EQUAL(UString("over"), tokens[5]);
  CPPUNIT_ASSERT_EQUAL(UString("the"), tokens[6]);
  CPPUNIT_ASSERT_EQUAL(UString("lazy"), tokens[7]);
  CPPUNIT_ASSERT_EQUAL(UString("dog"), tokens[8]);

  UString t = "/Mary/had/a/little///lamb//";

  uint_t index = 0;

  UString tok = t.nextToken(index, "/");
  CPPUNIT_ASSERT_EQUAL(UString("Mary"), tok);
  CPPUNIT_ASSERT_EQUAL(5U, index);

  tok = t.nextToken(index, "/");
  CPPUNIT_ASSERT_EQUAL(UString("had"), tok);
  CPPUNIT_ASSERT_EQUAL(9U, index);

  tok = t.nextToken(index, "/");
  CPPUNIT_ASSERT_EQUAL(UString("a"), tok);
  CPPUNIT_ASSERT_EQUAL(11U, index);

  tok = t.nextToken(index, "/");
  CPPUNIT_ASSERT_EQUAL(UString("little"), tok);
  CPPUNIT_ASSERT_EQUAL(18U, index);

  tok = t.nextToken(index, "/");
  CPPUNIT_ASSERT_EQUAL(UString("lamb"), tok);
  CPPUNIT_ASSERT_EQUAL(25U, index);

  tok = t.nextToken(index, "/");
  CPPUNIT_ASSERT(tok.isNull());
  CPPUNIT_ASSERT_EQUAL(25U, index);
}

/* end of source file */
