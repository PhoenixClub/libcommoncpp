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

#include "WStringTest.h++"
#include "TestUtils.h++"

#include <cppunit/TestCaller.h>
#include <cppunit/extensions/HelperMacros.h>

#include "commonc++/Common.h++"
#include "commonc++/Char.h++"

using namespace ccxx;

CPPUNIT_TEST_SUITE_REGISTRATION(WStringTest);

/*
 */

CppUnit::Test *WStringTest::suite()
{
  CCXX_TESTSUITE_BEGIN(WStringTest);
  CCXX_TESTSUITE_TEST(WStringTest, testAppend);
  CCXX_TESTSUITE_TEST(WStringTest, testInsert);
  CCXX_TESTSUITE_TEST(WStringTest, testEquals);
  CCXX_TESTSUITE_TEST(WStringTest, testUpperLowerCase);
  CCXX_TESTSUITE_TEST(WStringTest, testNumeric);
  CCXX_TESTSUITE_TEST(WStringTest, testTrim);
  CCXX_TESTSUITE_TEST(WStringTest, testReplace);
  CCXX_TESTSUITE_TEST(WStringTest, testNull);
  CCXX_TESTSUITE_TEST(WStringTest, testPadFill);
  CCXX_TESTSUITE_TEST(WStringTest, testPrintf);
  CCXX_TESTSUITE_END();
}

/*
 */

void WStringTest::setUp()
{
  _s0 = L"Hello";
  _s1 = L"Hello, world!";
}

/*
 */

void WStringTest::tearDown()
{
}

/*
 */

void WStringTest::testEquals()
{
  WString original = _s1;
  const wchar_t *c_cmp = L"Hello, world!";
  WString cmp = L"Hello, world!";

  CPPUNIT_ASSERT_EQUAL(0, original.compareTo(cmp));
  CPPUNIT_ASSERT_EQUAL(0, original.compareTo(c_cmp));
}

/*
 */

void WStringTest::testAppend()
{
  WString app = L", world!";

  WString s = _s0;
  s.append(L", world!");

  CPPUNIT_ASSERT_EQUAL(_s1, s);

  s = _s0;
  s.append(app);

  CPPUNIT_ASSERT_EQUAL(_s1, s);

  s = _s0;
  s.append(123456);
  CPPUNIT_ASSERT(s == L"Hello123456");

  s = _s0;
  s.append(L'!');
  CPPUNIT_ASSERT(s == L"Hello!");

  s = _s0;
  s.append(3.1415, 4);
  CPPUNIT_ASSERT(s == L"Hello3.1415");

  s = _s0;
  s.append(false);
  CPPUNIT_ASSERT(s == L"Hellofalse");

  wchar_t c = L'!';
  s.append(c);
  CPPUNIT_ASSERT(s == L"Hellofalse!");
}

/*
 */

void WStringTest::testInsert()
{
  WString s = L"defghi";

  s.insert(L"abc", 0, 3);

  CPPUNIT_ASSERT(s == L"abcdefghi");

  s.insert(L"jkXX", WString::END, 2);

  CPPUNIT_ASSERT(s == L"abcdefghijk");

  s.insert(L"lm", WString::END);

  CPPUNIT_ASSERT(s == L"abcdefghijklm");

  s.insert(L"123456", 5);

  CPPUNIT_ASSERT(s == L"abcde123456fghijklm");
}

/*
 */

void WStringTest::testUpperLowerCase()
{
  WString original = L"12345 Hello, !@#$% world!";

  WString s = original;
  s.toLowerCase();

  CPPUNIT_ASSERT(s == L"12345 hello, !@#$% world!");

  s = original;
  s.toUpperCase();

  CPPUNIT_ASSERT(s == L"12345 HELLO, !@#$% WORLD!");
}

/*
 */

void WStringTest::testNumeric()
{
  static const wchar_t *s_bool[] = { L"true", L"True", L"TRUE", L"false",
                                     L"FaLsE", L"blah" L"1", NULL };

  static const bool v_bool[] = { true, true, true, false, false, false,
                                 false };

  static const wchar_t *s_int[] = { L"0", L"123456789", L"-", L"+555",
                                    L"blah", L"+-+-+", NULL };

  static const int v_int[] = { 0, 123456789, 0, 555, 0, 0 };

  static const wchar_t *s_double[] = { L"1E6", L"1.1", L"0", L"+-+-+", L"E",
                                       L"3.141592654", L"-1.1E-0003", NULL };

  static const double v_double[] = { 1E6, 1.1, 0, 0, 0, 3.141592654, -1.1E-3 };

  const wchar_t **p;
  int i;

  for(i = 0, p = s_bool; *p; p++, i++)
  {
    WString s = *p;
    bool val = s.toBool();

    CPPUNIT_ASSERT(val == v_bool[i]);
  }

  for(i = 0, p = s_int; *p; p++, i++)
  {
    WString s = *p;
    int val = s.toInt();

    CPPUNIT_ASSERT(val == v_int[i]);
  }

  for(i = 0, p = s_double; *p; p++, i++)
  {
    WString s = *p;
    double val = s.toDouble();

    CPPUNIT_ASSERT(val == v_double[i]);
  }
}

/*
 */

void WStringTest::testTrim()
{
  WString s = L"   Hello, world!\t\n";
  s.trim();

  CPPUNIT_ASSERT(s == _s1);
}

/*
 */

void WStringTest::testReplace()
{
  WString s = L"Test this test is a test String xxtestxx. test";

  s.replace(L"test", L"ZZZ");

  CPPUNIT_ASSERT(s == L"Test this ZZZ is a ZZZ String xxZZZxx. ZZZ");

  s = L"rEplacE all uppErcasE E charactErs";

  s.replace(L'E', L'e');

  CPPUNIT_ASSERT(s == L"replace all uppercase e characters");
}

/*
 */

void WStringTest::testPrepend()
{
  WString s = L"World!";
  s.prepend(L"ello, ");
  s.prepend(L'H');

  CPPUNIT_ASSERT(s == L"Hello, World!");
}

/*
 */

void WStringTest::testRemove()
{
}

/*
 */

void WStringTest::testIndexOf()
{
}

/*
 */

void WStringTest::testContains()
{
  WString s = L"A quick brown fox jumped over the lazy dog.";

  CPPUNIT_ASSERT(s.contains(L"fox"));
  CPPUNIT_ASSERT(! s.contains(L'@'));
  CPPUNIT_ASSERT(s.contains(L'x'));
  CPPUNIT_ASSERT(s.contains(L"dog."));
}

/*
 */

void WStringTest::testIndex()
{
  WString s = L"123 456";

  CPPUNIT_ASSERT(s[0] == L'1');
  CPPUNIT_ASSERT(s[100] == WNUL);

  s[3] = 0;
  CPPUNIT_ASSERT(s == L"1230456");

  bool flag = false;
  try
  {
    s[100] = L'x';
  }
  catch(OutOfBoundsException& ex)
  {
    flag = true;
  }
  CPPUNIT_ASSERT(flag);
}

/*
 */

void WStringTest::testStartsEndsWith()
{
  WString s = L"123 @@@ xyz";

  CPPUNIT_ASSERT(s.startsWith(L'1'));
  CPPUNIT_ASSERT(s.startsWith(L"123"));

  CPPUNIT_ASSERT(s.endsWith(L'z'));
  CPPUNIT_ASSERT(s.endsWith(L" xyz"));
}

/*
 */

void WStringTest::testNull()
{
  CPPUNIT_ASSERT(WString::null.isNull());

  WString s = WString::null;

  CPPUNIT_ASSERT(s.isNull());

  s += L"foo";
  CPPUNIT_ASSERT(s == L"foo");
}

/*
 */

void WStringTest::testPadFill()
{
  WString s = L"Hello, world!";
  s.pad(' ', 5);

  CPPUNIT_ASSERT(s == L"Hello, world!     ");

  s.fill(L'!', 10);

  CPPUNIT_ASSERT(s == L"!!!!!!!!!!");
}

/*
 */

void WStringTest::testPrintf()
{
  WString s;
  int val = 20;
  s.printf(L"test %04u", val);
  std::cout << s << std::endl;
  s.clear();
  s.printf(L"another %04u", ++val);
  std::cout << s << std::endl;

  for(int i = 0; i < 200; ++i)
  {
    s.printf(L"here's some more %s #%d. ", "crap to fill the buffer with", i);
  }

  std::cout << s << std::endl;

}

/* end of source file */
