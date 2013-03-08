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

#include "StringTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Char.h++"
#include "commonc++/String.h++"
#include "commonc++/Hex.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(StringTest);

/*
 */

CppUnit::Test *StringTest::suite()
{
  CCXX_TESTSUITE_BEGIN(StringTest);
  CCXX_TESTSUITE_TEST(StringTest, testAppend);
  CCXX_TESTSUITE_TEST(StringTest, testInsert);
  CCXX_TESTSUITE_TEST(StringTest, testEquals);
  CCXX_TESTSUITE_TEST(StringTest, testUpperLowerCase);
  CCXX_TESTSUITE_TEST(StringTest, testNumeric);
  CCXX_TESTSUITE_TEST(StringTest, testTrim);
  CCXX_TESTSUITE_TEST(StringTest, testReplace);
  CCXX_TESTSUITE_TEST(StringTest, testNull);
  CCXX_TESTSUITE_TEST(StringTest, testPadFill);
  CCXX_TESTSUITE_TEST(StringTest, testPrintf);
  CCXX_TESTSUITE_TEST(StringTest, testCharset);
  CCXX_TESTSUITE_TEST(StringTest, testReserve);
  CCXX_TESTSUITE_END();
}

/*
 */

void StringTest::setUp()
{
  _s0 = "Hello";
  _s1 = "Hello, world!";
}

/*
 */

void StringTest::tearDown()
{
}

/*
 */

void StringTest::testEquals()
{
  String original = _s1;
  const char *c_cmp = "Hello, world!";
  String cmp = "Hello, world!";

  CPPUNIT_ASSERT_EQUAL(0, original.compareTo(cmp));
  CPPUNIT_ASSERT_EQUAL(0, original.compareTo(c_cmp));
}

/*
 */

void StringTest::testAppend()
{
  String app = ", world!";

  String s = _s0;
  s.append(", world!");

  CPPUNIT_ASSERT(s == _s1);

  s = _s0;
  s.append(app);

  CPPUNIT_ASSERT(s == _s1);

  s = _s0;
  s.append(123456);
  CPPUNIT_ASSERT_EQUAL(String("Hello123456"), s);

  s = _s0;
  s.append('!');
  CPPUNIT_ASSERT_EQUAL(String("Hello!"), s);

  s = _s0;
  s.append(3.1415, 4);
  CPPUNIT_ASSERT_EQUAL(String("Hello3.1415"), s);

  s = _s0;
  s.append(false);
  CPPUNIT_ASSERT_EQUAL(String("Hellofalse"), s);

  Char c = '!';
  s.append(c);
  CPPUNIT_ASSERT_EQUAL(String("Hellofalse!"), s);
}

/*
 */

void StringTest::testInsert()
{
  String s = "defghi";

  s.insert("abc", 0, 3);

  CPPUNIT_ASSERT_EQUAL(String("abcdefghi"), s);

  s.insert("jkXX", String::END, 2);

  CPPUNIT_ASSERT_EQUAL(String("abcdefghijk"), s);

  s.insert("lm", String::END);

  CPPUNIT_ASSERT_EQUAL(String("abcdefghijklm"), s);

  s.insert("123456", 5);

  CPPUNIT_ASSERT_EQUAL(String("abcde123456fghijklm"), s);
}

/*
 */

void StringTest::testUpperLowerCase()
{
  String original = "12345 Hello, !@#$% world!";

  String s = original;
  s.toLowerCase();

  CPPUNIT_ASSERT_EQUAL(String("12345 hello, !@#$% world!"), s);

  s = original;
  s.toUpperCase();

  CPPUNIT_ASSERT_EQUAL(String("12345 HELLO, !@#$% WORLD!"), s);
}

/*
 */

void StringTest::testNumeric()
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
    String s = *p;
    bool val = s.toBool();

    CPPUNIT_ASSERT_EQUAL(v_bool[i], val);
  }

  for(i = 0, p = s_int; *p; p++, i++)
  {
    String s = *p;
    int val = s.toInt();

    CPPUNIT_ASSERT_EQUAL(v_int[i], val);
  }

  for(i = 0, p = s_double; *p; p++, i++)
  {
    String s = *p;
    double val = s.toDouble();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(v_double[i], val, .00001);
  }
}

/*
 */

void StringTest::testTrim()
{
  String s = "   Hello, world!\t\n";
  s.trim();

  CPPUNIT_ASSERT_EQUAL(_s1, s);
}

/*
 */

void StringTest::testReplace()
{
  String s = "Test this test is a test String xxtestxx. test";

  s.replace("test", "ZZZ");

  CPPUNIT_ASSERT_EQUAL(String("Test this ZZZ is a ZZZ String xxZZZxx. ZZZ"),
                       s);

  s = "rEplacE all uppErcasE E charactErs";

  s.replace('E', 'e');

  CPPUNIT_ASSERT_EQUAL(String("replace all uppercase e characters"), s);
}

/*
 */

void StringTest::testPrepend()
{
  String s = "World!";
  s.prepend("ello, ");
  s.prepend('H');

  CPPUNIT_ASSERT_EQUAL(String("Hello, World!"), s);
}

/*
 */

void StringTest::testRemove()
{
}

/*
 */

void StringTest::testIndexOf()
{
}

/*
 */

void StringTest::testContains()
{
  String s = "A quick brown fox jumped over the lazy dog.";

  CPPUNIT_ASSERT_EQUAL(true, s.contains("fox"));
  CPPUNIT_ASSERT_EQUAL(false, s.contains('@'));
  CPPUNIT_ASSERT_EQUAL(true, s.contains('x'));
  CPPUNIT_ASSERT_EQUAL(true, s.contains("dog."));
}

/*
 */

void StringTest::testIndex()
{
  String s = "123 456";

  CPPUNIT_ASSERT_EQUAL('1', s[0]);
  CPPUNIT_ASSERT_EQUAL(NUL, s[100]);

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

void StringTest::testStartsEndsWith()
{
  String s = "123 @@@ xyz";

  CPPUNIT_ASSERT(s.startsWith('1'));
  CPPUNIT_ASSERT(s.startsWith("123"));

  CPPUNIT_ASSERT(s.endsWith('z'));
  CPPUNIT_ASSERT(s.endsWith(" xyz"));
}

/*
 */

void StringTest::testNull()
{
  CPPUNIT_ASSERT_EQUAL(true, String::null.isNull());

  String s = String::null;

  CPPUNIT_ASSERT_EQUAL(true, s.isNull());

  String t;

  CPPUNIT_ASSERT_EQUAL(true, t.isNull());
  CPPUNIT_ASSERT(s == t);

  s += "foo";
  CPPUNIT_ASSERT_EQUAL(String("foo"), s);

  s = NULL;
  CPPUNIT_ASSERT_EQUAL(true, s.isNull());
}

/*
 */

void StringTest::testPadFill()
{
  String s = "Hello, world!";
  s.pad(' ', 5);

  CPPUNIT_ASSERT_EQUAL(String("Hello, world!     "), s);

  s.fill('!', 10);

  CPPUNIT_ASSERT_EQUAL(String("!!!!!!!!!!"), s);
}

/*
 */

void StringTest::testPrintf()
{
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
}

/*
 */

void StringTest::testReserve()
{
  String s;
  s = "abc";
  s.reserve(30);
  s += "def";

  CPPUNIT_ASSERT_EQUAL(32U, s.getCapacity());

  for(int i = 0; i < 10; ++i)
  {
    s += "ABCDEF";
  }
  CPPUNIT_ASSERT_EQUAL(72U, s.getCapacity());

  CPPUNIT_ASSERT_EQUAL(String("abcdefABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF"), s);
}

/*
 */

void StringTest::testConcat()
{
  String s;
  s.clear() << "abc" << "123";

  CPPUNIT_ASSERT_EQUAL(String("abc123"), s);
}

/*
 */

void StringTest::testCharset()
{
  fwide(stderr, 1);

  String s = "Hello, world!";

  Blob b = s.getBytes("UTF-16LE");

  WString ws(b.getData(), b.getLength(), "UTF-16LE");

  WString ws2 = WString::fromCharArray("Hello, world! 2");

  const wchar_t *ws2_c = L"Hello, world! 2";

  CPPUNIT_ASSERT(ws2 == ws2_c);


  const char *text = "Alice was beginning to get very tired of sitting by her sister on the bank, and of having nothing to do: once or twice she had peeped into the book her sister was reading, but it had no pictures or conversations in it, `and what is the use of a book,' thought Alice `without pictures or conversation?' So she was considering in her own mind (as well as she could, for the hot day made her feel very sleepy and stupid), whether the pleasure of making a daisy-chain would be worth the trouble of getting up and picking the daisies, when suddenly a White Rabbit with pink eyes ran close by her.";

  WString alice;
  alice = WString::fromCharArray(text);

  fwprintf(stderr, L"%ls\n", alice.c_str());

  const wchar_t *utext = L"\u2049 \u2031 \u2022";

  WString unicode(utext);

  Blob ub = unicode.getBytes("UTF-8");

  CPPUNIT_ASSERT_EQUAL(String("E2818920E280B120E280A200"), Hex::encode(ub));

//  printf("%*s\n", ub.getLength(), ub.getData());
}

/* end of source file */
