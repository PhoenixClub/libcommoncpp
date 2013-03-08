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

#include "commonc++/JavaContext.h++"
#include "commonc++/Log.h++"

namespace ccxx {

/*
 */

JavaContext::JavaContext(JNIEnv *env)
  : _jvm(NULL), _env(env), _verbose(false)
{
}

/*
 */

JavaContext::~JavaContext()
{
}

/*
 */

String JavaContext::encodeType(const String &type)
{
  const char *p, *q, *s = type.c_str();
  int dim = 0, len = 0;
  uint_t i;
  bool end = false;
  String buf;

  for(p = s; *p; p++)
  {
    if(*p == '[')
    {
      end = true;
      dim++;
    }

    if(! end)
      len++;
  }

  for(i = dim; i--; buf += '[');

  if(! ::strncmp(s, "boolean", len))
    buf += 'Z';
  else if(! ::strncmp(s, "byte", len))
    buf += 'B';
  else if(! ::strncmp(s, "char", len))
    buf += 'C';
  else if(! ::strncmp(s, "short", len))
    buf += 'S';
  else if(! ::strncmp(s, "int", len))
    buf += 'I';
  else if(! ::strncmp(s, "long", len))
    buf += 'J';
  else if(! ::strncmp(s, "float", len))
    buf += 'F';
  else if(! ::strncmp(s, "double", len))
    buf += 'D';
  else if(! ::strncmp(s, "String", len))
    buf += "Ljava/lang/String;";
  else if(! ::strncmp(s, "Object", len))
    buf += "Ljava/lang/Object;";
  else if(! ::strncmp(s, "Class", len))
    buf += "Ljava/lang/Class;";
  else if(! ::strncmp(s, "void", len))
    buf += 'V';
  else
  {
    buf += 'L';
    for(i = len, q = s; i--; q++)
      buf += ((*q == '.') ? '/' : *q);
    buf += ';';
  }

  return(buf);
}

/*
 */

bool JavaContext::parseSignature(const String &signature, String& method,
                                 String& descriptor, bool& isStatic)
{
  static const char *init = "<init>", *void_type = "void";
  uint_t index = 0;
  String tok, ret;

  isStatic = false;

  if(signature[0] == '(')
  {
    method = init;
    ret = void_type;
    index++;
  }
  else
  {
    tok = signature.nextToken(index, " ");

    if(! tok)
      return(false);

    if(tok == "static")
    {
      isStatic = true;

      ret = signature.nextToken(index, " ");

      if(! ret)
        return(false);
    }
    else
      ret = tok;

    tok = signature.nextToken(index, " (");
    if(! tok)
      return(false);

    method = tok;
  }

  descriptor = "(";

  for(;;)
  {
    tok = signature.nextToken(index, "(, )");
    if(! tok)
      break;

    String type = encodeType(tok);
    descriptor += type;
  }

  descriptor += ')';

  String type = encodeType(ret);
  descriptor += type;

  return(true);
}

/*
 */

jclass JavaContext::findClass(const String &name) throw(JavaException)
{
  Log_debug("JavaContext::findClass(%s)", name.c_str());

  String tname = name;
  tname.replace('.', '/');

  jclass cl = _env->FindClass(tname.c_str());

  if(checkException(_verbose))
    throw JavaClassNotFoundException(name);

  return(cl);
}

/*
 */

jmethodID JavaContext::findMethod(jclass clazz, const String &signature)
  throw(JavaException)
{
  Log_debug("JavaContext::findMethod('%s')", signature.c_str());

  jmethodID mid;
  bool isStatic;
  String descriptor, method;

  if(! parseSignature(signature, method, descriptor, isStatic))
    throw JavaSignatureException();

  Log_debug("Signature: %s, descriptor: %s, isStatic = %s", signature.c_str(),
            descriptor.c_str(), (isStatic ? "true" : "false"));

  if(isStatic)
    mid = _env->GetStaticMethodID(clazz, method.c_str(), descriptor.c_str());
  else
    mid = _env->GetMethodID(clazz, method.c_str(), descriptor.c_str());

  if(checkException(_verbose))
    throw JavaMethodNotFoundException("no such method: " + method
                                      + descriptor);

  return(mid);
}

/*
 */

jobject JavaContext::createObject(jclass clazz, jmethodID constructor, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::createObject()");

  va_list args;
  jobject obj;

  va_start(args, constructor);

  try
  {
    obj = createObjectVA(clazz, constructor, args);
  }
  catch(JavaException)
  {
    va_end(args);
    throw;
  }

  va_end(args);

  return(obj);
}

/*
 */

jobject JavaContext::createObjectVA(jclass clazz, jmethodID constructor,
                                    va_list args)
  throw(JavaException)
{
  Log_debug("JavaContext::createObjectVA()");

  jobject obj;

  obj = _env->NewObjectV(clazz, constructor, args);

  if(checkException(_verbose))
  {
    throw JavaInstantiationException();
  }

  return(obj);
}

/*
 */

void JavaContext::callVoidMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callVoidMethod()");

  va_list arglist;

  va_start(arglist, method);
  _env->CallVoidMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();
}

/*
 */

jobject JavaContext::callObjectMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callObjectMethod()");

  jobject r = NULL;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallObjectMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(r);
}

/*
 */

String JavaContext::callStringMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callStringMethod()");

  jobject r = NULL;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallObjectMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  if (r == NULL)
    return(String::null);
  else
    return(decodeString(static_cast<jstring>(r)));
}

/*
 */

WString JavaContext::callWStringMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callWStringMethod()");

  jobject r = NULL;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallObjectMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  if (r == NULL)
    return(WString::null);
  else
    return(decodeWString(static_cast<jstring>(r)));
}

/*
 */

bool JavaContext::callBooleanMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callBooleanMethod()");

  jboolean r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallBooleanMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<bool>(r));
}

/*
 */

byte_t JavaContext::callByteMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callByteMethod()");

  jbyte r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallByteMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<byte_t>(r));
}

/*
 */

char16_t JavaContext::callCharMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callCharMethod()");

  jchar r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallCharMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<char16_t>(r));
}

/*
 */

int16_t JavaContext::callShortMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callShortMethod()");

  jshort r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallShortMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int16_t>(r));
}

/*
 */

int32_t JavaContext::callIntMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callIntMethod()");

  jint r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallIntMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int32_t>(r));
}

/*
 */

int64_t JavaContext::callLongMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callLongMethod()");

  jlong r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallLongMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int64_t>(r));
}

/*
 */

float JavaContext::callFloatMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callFloatMethod()");

  jfloat r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallFloatMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<float>(r));
}

/*
 */

double JavaContext::callDoubleMethod(jobject object, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callDoubleMethod()");

  jdouble r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallDoubleMethodV(object, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<double>(r));
}

/*
 */

void JavaContext::callNonvirtualVoidMethod(jobject object, jclass clazz,
                                           jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callNonvirtualVoidMethod()");

  va_list arglist;

  va_start(arglist, method);
  _env->CallNonvirtualVoidMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();
}

/*
 */

jobject JavaContext::callNonvirtualObjectMethod(jobject object, jclass clazz,
                                                jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callNonvirtualObjectMethod()");

  jobject r = NULL;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualObjectMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(r);
}

/*
 */

bool JavaContext::callNonvirtualBooleanMethod(jobject object, jclass clazz,
                                              jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callNonvirtualBooleanMethod()");

  jboolean r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualBooleanMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<bool>(r));
}

/*
 */

byte_t JavaContext::callNonvirtualByteMethod(jobject object, jclass clazz,
                                             jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callNonvirtualByteMethod()");

  jbyte r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualByteMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<byte_t>(r));
}

/*
 */

char16_t JavaContext::callNonvirtualCharMethod(jobject object, jclass clazz,
                                               jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callNonvirtualCharMethod()");

  jchar r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualCharMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<char16_t>(r));
}

/*
 */

int16_t JavaContext::callNonvirtualShortMethod(jobject object, jclass clazz,
                                               jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callNonvirtualShortMethod()");

  jshort r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualShortMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int16_t>(r));
}

/*
 */

int32_t JavaContext::callNonvirtualIntMethod(jobject object, jclass clazz,
                                             jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callNonvirtualIntMethod()");

  jint r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualIntMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int32_t>(r));
}

/*
 */

int64_t JavaContext::callNonvirtualLongMethod(jobject object, jclass clazz,
                                              jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callNonvirtualLongMethod()");

  jlong r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualLongMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int64_t>(r));
}

/*
 */

float JavaContext::callNonvirtualFloatMethod(jobject object, jclass clazz,
                                             jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callNonvirtualFloatMethod()");

  jfloat r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualFloatMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<float>(r));
}

/*
 */

double JavaContext::callNonvirtualDoubleMethod(jobject object, jclass clazz,
                                               jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callNonvirtualDoubleMethod()");

  jdouble r = 0;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallNonvirtualDoubleMethodV(object, clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<double>(r));
}

/*
 */

void JavaContext::callStaticVoidMethod(jclass clazz, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callStaticVoidMethod()");

  va_list arglist;

  va_start(arglist, method);
  _env->CallStaticVoidMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();
}

/*
 */

jobject JavaContext::callStaticObjectMethod(jclass clazz, jmethodID method,
                                            ...) throw(JavaException)
{
  Log_debug("JavaContext::callStaticObjectMethod()");

  jobject r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticObjectMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(r);
}

/*
 */

String JavaContext::callStaticStringMethod(jclass clazz, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callStaticStringMethod()");

  jobject r = NULL;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticObjectMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  if (r == NULL)
    return(String::null);
  else
    return(decodeString(static_cast<jstring>(r)));
}

/*
 */

WString JavaContext::callStaticWStringMethod(jclass clazz, jmethodID method,
                                             ...) throw(JavaException)
{
  Log_debug("JavaContext::callStaticWStringMethod()");

  jobject r = NULL;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticObjectMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  if (r == NULL)
    return(WString::null);
  else
    return(decodeWString(static_cast<jstring>(r)));
}

/*
 */

bool JavaContext::callStaticBooleanMethod(jclass clazz, jmethodID method,
                                          ...) throw(JavaException)
{
  Log_debug("JavaContext::callStaticBooleanMethod()");

  jboolean r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticBooleanMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<bool>(r));
}

/*
 */

byte_t JavaContext::callStaticByteMethod(jclass clazz, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callStaticByteMethod()");

  jbyte r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticByteMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<byte_t>(r));
}

/*
 */

char16_t JavaContext::callStaticCharMethod(jclass clazz, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callStaticCharMethod()");

  jchar r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticCharMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<char16_t>(r));
}

/*
 */

int16_t JavaContext::callStaticShortMethod(jclass clazz, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callStaticShortMethod()");

  jshort r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticShortMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int16_t>(r));
}

/*
 */

int32_t JavaContext::callStaticIntMethod(jclass clazz, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callStaticIntMethod()");

  jint r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticIntMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int32_t>(r));
}

/*
 */

int64_t JavaContext::callStaticLongMethod(jclass clazz, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callStaticLongMethod()");

  jlong r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticLongMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<int64_t>(r));
}

/*
 */

float JavaContext::callStaticFloatMethod(jclass clazz, jmethodID method, ...)
  throw(JavaException)
{
  Log_debug("JavaContext::callStaticFloatMethod()");

  jfloat r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticFloatMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<float>(r));
}

/*
 */

double JavaContext::callStaticDoubleMethod(jclass clazz, jmethodID method,
                                           ...) throw(JavaException)
{
  Log_debug("JavaContext::callStaticDoubleMethod()");

  jdouble r;
  va_list arglist;

  va_start(arglist, method);
  r = _env->CallStaticDoubleMethodV(clazz, method, arglist);
  va_end(arglist);

  if(checkException(_verbose))
    throw JavaInvocationException();

  return(static_cast<double>(r));
}

/*
 */

bool JavaContext::checkException(bool verbose /* = false */)
{
  Log_debug("JavaContext::checkException()");

  if(_env->ExceptionCheck() == JNI_TRUE)
  {
    if(verbose)
      _env->ExceptionDescribe();

    return(true);
  }

  return(false);
}

/*
 */

void JavaContext::clearException() throw()
{
  Log_debug("JavaContext::clearException()");

  _env->ExceptionClear();
}

/*
 */

jthrowable JavaContext::getException() throw()
{
  Log_debug("JavaContext::getException()");

  return(_env->ExceptionOccurred());
}

/*
 */

bool JavaContext::throwException(jclass exception, const String &message)
{
  Log_debug("JavaContext::throwException()");

  jint i = _env->ThrowNew(exception, message.c_str());

  return(i == 0);
}

/*
 */

jfieldID JavaContext::findField(jclass clazz, const String &signature)
  throw(JavaException)
{
  Log_debug("JavaContext::findField('%s')", signature.c_str());

  jfieldID fid;
  bool isStatic = false;
  uint_t index = 0;
  String buf;

  // get the field type
  String tok = signature.nextToken(index, " ");
  if(! tok)
    throw JavaSignatureException();

  if(tok == "static")
  {
    isStatic = true;
    tok = signature.nextToken(index, " ");
  }

  buf = encodeType(tok);

  // get the field name
  tok = signature.nextToken(index, " ");

  if(isStatic)
    fid = _env->GetStaticFieldID(clazz, tok.c_str(), buf.c_str());
  else
    fid = _env->GetFieldID(clazz, tok.c_str(), buf.c_str());

  if(checkException(_verbose))
    throw JavaFieldNotFoundException(signature);

  return(fid);
}

/*
 */

jobject JavaContext::getObjectField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getObjectField()");

  jobject r = _env->GetObjectField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(r);
}

/*
 */

bool JavaContext::getBooleanField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getBooleanField()");

  jboolean r = _env->GetBooleanField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<bool>(r));
}

/*
 */

byte_t JavaContext::getByteField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getByteField()");

  jbyte r = _env->GetByteField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<byte_t>(r));
}

/*
 */

char16_t JavaContext::getCharField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getCharField()");

  jchar r = _env->GetCharField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<char16_t>(r));
}

/*
 */

int16_t JavaContext::getShortField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getShortField()");

  jshort r = _env->GetShortField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int16_t>(r));
}

/*
 */

int32_t JavaContext::getIntField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getIntField()");

  jint r = _env->GetIntField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int32_t>(r));
}

/*
 */

int64_t JavaContext::getLongField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getLongField()");

  jlong r = _env->GetLongField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int64_t>(r));
}

/*
 */

float JavaContext::getFloatField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getFloatField()");

  jfloat r = _env->GetFloatField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<float>(r));
}

/*
 */

double JavaContext::getDoubleField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getDoubleField()");

  jdouble r = _env->GetDoubleField(object, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<double>(r));
}

/*
 */

String JavaContext::getStringField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStringField()");

  jstring str = static_cast<jstring>(_env->GetObjectField(object, field));

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(decodeString(str));
}

/*
 */

WString JavaContext::getWStringField(jobject object, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getWStringField()");

  jstring str = static_cast<jstring>(_env->GetObjectField(object, field));

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(decodeWString(str));
}

/*
 */

jobject JavaContext::getStaticObjectField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticObjectField()");

  jobject r = _env->GetStaticObjectField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(r);
}

/*
 */

bool JavaContext::getStaticBooleanField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticBooleanField()");

  jboolean r = _env->GetStaticBooleanField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<bool>(r));
}

/*
 */

byte_t JavaContext::getStaticByteField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticByteField()");

  jbyte r = _env->GetStaticByteField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<byte_t>(r));
}

/*
 */

char16_t JavaContext::getStaticCharField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticCharField()");

  jchar r = _env->GetStaticCharField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<char16_t>(r));
}

/*
 */

int16_t JavaContext::getStaticShortField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticShortField()");

  jshort r = _env->GetStaticShortField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int16_t>(r));
}

/*
 */

int32_t JavaContext::getStaticIntField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticIntField()");

  jint r = _env->GetStaticIntField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int32_t>(r));
}

/*
 */

int64_t JavaContext::getStaticLongField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticLongField()");

  jlong r = _env->GetStaticLongField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<int64_t>(r));
}

/*
 */

float JavaContext::getStaticFloatField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticFloatField()");

  jfloat r = _env->GetStaticFloatField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<float>(r));
}

/*
 */

double JavaContext::getStaticDoubleField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticDoubleField()");

  jdouble r = _env->GetStaticDoubleField(clazz, field);

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(static_cast<double>(r));
}

/*
 */

String JavaContext::getStaticStringField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticStringField()");

  jstring str = static_cast<jstring>(_env->GetStaticObjectField(clazz, field));

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(decodeString(str));
}

/*
 */

WString JavaContext::getStaticWStringField(jclass clazz, jfieldID field)
  throw(JavaException)
{
  Log_debug("JavaContext::getStaticWStringField()");

  jstring str = static_cast<jstring>(_env->GetStaticObjectField(clazz, field));

  if(checkException(_verbose))
    throw JavaFieldAccessException();

  return(decodeWString(str));
}

/*
 */

void JavaContext::setObjectField(jobject object, jfieldID field, jobject value)
  throw(JavaException)
{
  Log_debug("JavaContext::setObjectField()");

  _env->SetObjectField(object, field, value);

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setBooleanField(jobject object, jfieldID field, bool value)
  throw(JavaException)
{
  Log_debug("JavaContext::setBooleanField()");

  _env->SetBooleanField(object, field, static_cast<jboolean>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setByteField(jobject object, jfieldID field, byte_t value)
  throw(JavaException)
{
  Log_debug("JavaContext::setByteField()");

  _env->SetByteField(object, field, static_cast<jbyte>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setCharField(jobject object, jfieldID field, char16_t value)
  throw(JavaException)
{
  Log_debug("JavaContext::setCharField()");

  _env->SetCharField(object, field, static_cast<jchar>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setShortField(jobject object, jfieldID field, int16_t value)
  throw(JavaException)
{
  Log_debug("JavaContext::setShortField()");

  _env->SetShortField(object, field, static_cast<jshort>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setIntField(jobject object, jfieldID field, int32_t value)
  throw(JavaException)
{
  Log_debug("JavaContext::setIntField()");

  _env->SetIntField(object, field, static_cast<jint>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setLongField(jobject object, jfieldID field, int64_t value)
  throw(JavaException)
{
  Log_debug("JavaContext::setLongField()");

  _env->SetLongField(object, field, static_cast<jlong>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setFloatField(jobject object, jfieldID field, float value)
  throw(JavaException)
{
  Log_debug("JavaContext::setFloatField()");

  _env->SetFloatField(object, field, static_cast<jfloat>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setDoubleField(jobject object, jfieldID field,
                                 double value) throw(JavaException)
{
  Log_debug("JavaContext::setDoubleField()");

  _env->SetDoubleField(object, field, static_cast<jdouble>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStringField(jobject object, jfieldID field,
                                 const String &value) throw(JavaException)
{
  Log_debug("JavaContext::setStringField()");

  jstring jstr = encodeString(value);
  _env->SetObjectField(object, field, jstr);

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setWStringField(jobject object, jfieldID field,
                                  const WString &value) throw(JavaException)
{
  Log_debug("JavaContext::setWStringField()");

  jstring jstr = encodeWString(value);
  _env->SetObjectField(object, field, jstr);

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticObjectField(jclass clazz, jfieldID field,
                                       jobject value) throw(JavaException)
{
  Log_debug("JavaContext::setStaticObjectField()");

  _env->SetStaticObjectField(clazz, field, value);

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticBooleanField(jclass clazz, jfieldID field,
                                        bool value) throw(JavaException)
{
  Log_debug("JavaContext::setStaticBooleanField()");

  _env->SetStaticBooleanField(clazz, field, static_cast<jboolean>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticByteField(jclass clazz, jfieldID field,
                                     byte_t value) throw(JavaException)
{
  Log_debug("JavaContext::setStaticByteField()");

  _env->SetStaticByteField(clazz, field, static_cast<jbyte>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticCharField(jclass clazz, jfieldID field,
                                     char16_t value) throw(JavaException)
{
  Log_debug("JavaContext::setStaticCharField()");

  _env->SetStaticCharField(clazz, field, static_cast<jchar>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticShortField(jclass clazz, jfieldID field,
                                      int16_t value) throw(JavaException)
{
  Log_debug("JavaContext::setStaticShortField()");

  _env->SetStaticShortField(clazz, field, static_cast<jshort>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticIntField(jclass clazz, jfieldID field,
                                    int32_t value) throw(JavaException)
{
  Log_debug("JavaContext::setStaticIntField()");

  _env->SetStaticIntField(clazz, field, static_cast<jint>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticLongField(jclass clazz, jfieldID field,
                                     int64_t value) throw(JavaException)
{
  Log_debug("JavaContext::setStaticLongField()");

  _env->SetStaticLongField(clazz, field, static_cast<jlong>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticFloatField(jclass clazz, jfieldID field,
                                      float value) throw(JavaException)
{
  Log_debug("JavaContext::setStaticFloatField()");

  _env->SetStaticFloatField(clazz, field, static_cast<float>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setStaticDoubleField(jclass clazz, jfieldID field,
                                       double value) throw(JavaException)
{
  Log_debug("JavaContext::setStaticDoubleField()");

  _env->SetStaticDoubleField(clazz, field, static_cast<jdouble>(value));

  if(checkException(_verbose))
    throw JavaFieldAccessException();
}

/*
 */

void JavaContext::setObjectArrayElement(jobjectArray array, uint_t index,
                                        jobject value) throw(JavaException)
{
  Log_debug("JavaContext::setObjectArrayElement()");

  _env->SetObjectArrayElement(array, static_cast<jsize>(index), value);

  if(checkException(_verbose))
    throw JavaArrayAccessException();
}

/*
 */

jobject JavaContext::getObjectArrayElement(jobjectArray array, uint_t index)
  throw(JavaException)
{
  Log_debug("JavaContext::getObjectArrayElement()");

  jobject r = _env->GetObjectArrayElement(array, static_cast<jsize>(index));

  if(checkException(_verbose))
    throw JavaArrayAccessException();

  return(r);
}

/*
 */

uint_t JavaContext::getArrayLength(jarray array) throw(JavaException)
{
  Log_debug("JavaContext::getArrayLength()");

  jsize r = _env->GetArrayLength(array);

  if(checkException(_verbose))
    throw JavaArrayAccessException();

  return((uint_t)r);
}

/*
 */

jbyteArray JavaContext::createByteArray(uint_t length) throw(JavaException)

{
  Log_debug("JavaContext::createByteArray()");

  jbyteArray r = _env->NewByteArray((jsize)length);

  if(checkException(_verbose))
    throw JavaInstantiationException();

  return(r);
}

/*
 */

jarray JavaContext::createObjectArray(jclass type, uint_t length,
                                      jobject initialValue /* = NULL */)
  throw(JavaException)
{
  Log_debug("JavaContext::createObjectArray()");

  jarray r = _env->NewObjectArray(static_cast<jsize>(length), type,
                                 initialValue);

  if(checkException(_verbose))
    throw JavaInstantiationException();

  return(r);
}

/*
 */

jobject JavaContext::createDirectByteBuffer(byte_t *buf, size_t size)
  throw(JavaException)
{
  Log_debug("JavaContext::createDirectByteBuffer()");

  jobject r = _env->NewDirectByteBuffer((void *)buf, static_cast<jlong>(size));

  if(checkException(_verbose))
    throw JavaOutOfMemoryException();

  return(r);
}

/*
 */

void *JavaContext::getDirectBufferRegion(jobject buf, uint64_t& size)
{
  Log_debug("JavaContext::getDirectBufferRegion()");

  void *base = _env->GetDirectBufferAddress(buf);
  if(base)
  {
    jlong sz = _env->GetDirectBufferCapacity(buf);
    size = static_cast<uint64_t>(sz);
  }

  return(base);
}

/*
 */

jobject JavaContext::createLocalRef(jobject object) throw(JavaException)
{
  Log_debug("JavaContext::createLocalRef()");

  jobject r = _env->NewLocalRef(object);

  if(r == NULL)
    throw JavaOutOfMemoryException();

  return(r);
}

/*
 */

void JavaContext::deleteLocalRef(jobject ref)
{
  Log_debug("JavaContext::deleteLocalRef()");

  _env->DeleteLocalRef(ref);
}

/*
 */

void JavaContext::ensureLocalCapacity(uint_t capacity) throw(JavaException)
{
  Log_debug("JavaContext::ensureLocalCapacity()");

  jint r = _env->EnsureLocalCapacity(static_cast<jint>(capacity));

  if(checkException(_verbose) || (r != 0))
    throw JavaOutOfMemoryException();
}

/*
 */

jobject JavaContext::createGlobalRef(jobject object) throw(JavaException)

{
  Log_debug("JavaContext::createGlobalRef()");

  jobject r = _env->NewGlobalRef(object);

  if(r == NULL)
    throw JavaOutOfMemoryException();

  return(r);
}

/*
 */

void JavaContext::deleteGlobalRef(jobject ref)
{
  Log_debug("JavaContext::deleteGlobalRef()");

  _env->DeleteGlobalRef(ref); // NULL ref is OK
}

/*
 */

String JavaContext::decodeString(jstring str) throw(JavaException)
{
  Log_debug("JavaContext::decodeString()");

  jboolean iscopy;
  const char *r = _env->GetStringUTFChars(str, &iscopy);
  jsize len = _env->GetStringLength(str);

  String s(r, 0, static_cast<size_t>(len));
  _env->ReleaseStringUTFChars(str, r);

  return(s);
}

/*
 */

WString JavaContext::decodeWString(jstring str) throw(JavaException)
{
  Log_debug("JavaContext::decodeWString()");

  jboolean iscopy;
  const char *r = _env->GetStringUTFChars(str, &iscopy);
  WString ws = WString::fromUTF8(r);

  _env->ReleaseStringUTFChars(str, r);

  return(ws);
}

/*
 */

jstring JavaContext::encodeString(const String &str) throw(JavaException)
{
  Log_debug("JavaContext::encodeString()");

  if(!str)
    return(NULL);

  jstring r = _env->NewStringUTF(str.c_str());

  return(r);
}

/*
 */

jstring JavaContext::encodeWString(const WString &str) throw(JavaException)
{
  Log_debug("JavaContext::encodeWString()");

  if(!str)
    return(NULL);

  Blob blob = str.getBytes("UTF-8");
  blob << 0x00;
  jstring r = _env->NewStringUTF(
    reinterpret_cast<const char *>(blob.getData()));

  return(r);
}

/*
 */

uint_t JavaContext::getStringLength(jstring str)
{
  Log_debug("JavaContext::getStringLength()");

  jsize sz = _env->GetStringLength(str);
  return(static_cast<uint_t>(sz));
}

/*
 */

void JavaContext::releaseArrayData(jarray array, void *data)
{
  Log_debug("JavaContext::releaseArrayData()");

  _env->ReleasePrimitiveArrayCritical(array, data, 0);
}

/*
 */

void *JavaContext::getArrayData(jarray array, uint_t& length,
                                bool* isCopy /* = NULL */)
  throw(JavaException)
{
  Log_debug("JavaContext::getArrayData()");

  length = _env->GetArrayLength(array);

  jboolean copy = false;
  void *r = _env->GetPrimitiveArrayCritical(array, &copy);

  if(checkException(_verbose))
    throw JavaOutOfMemoryException();

  if(isCopy)
    *isCopy = static_cast<bool>(copy);

  return(r);
}

/*
 */

jbyteArray JavaContext::createByteArray(const byte_t *data, size_t size)
  throw(JavaException)
{
  Log_debug("JavaContext::createByteArray()");

  jbyteArray array = createByteArray(size);

  _env->SetByteArrayRegion(array, (jsize)0, (jsize)size, (jbyte *)data);

  if(checkException(_verbose))
    throw JavaArrayAccessException();

  return(array);
}

/*
 */

bool JavaContext::isSameObject(jobject object1, jobject object2)
{
  Log_debug("JVM::isSameObject");

  jboolean b = _env->IsSameObject(object1, object2);

  return(b == JNI_TRUE);
}

/*
 */

bool JavaContext::isInstanceOf(jobject object, jclass clazz)
{
  Log_debug("JavaContext::isInstanceOf()");

  jboolean b = _env->IsInstanceOf(object, clazz);

  return(b == JNI_TRUE);
}

/*
 */

bool JavaContext::isAssignableFrom(jclass clazz1, jclass clazz2)
{
  Log_debug("JavaContext::isAssignableFrom()");

  jboolean b = _env->IsAssignableFrom(clazz1, clazz2);

  return(b == JNI_TRUE);
}

/*
 */

jclass JavaContext::getClassForObject(jobject object)
{
  Log_debug("JavaContext::getClassForObject()");

  return(_env->GetObjectClass(object));
}

/*
 */

jclass JavaContext::getSuperclass(jclass clazz)
{
  Log_debug("JavaContext::getSuperclass()");

  return(_env->GetSuperclass(clazz));
}

/*
 */

void JavaContext::enterMonitor(jobject object) throw(JavaException)
{
  Log_debug("JavaContext::enterMonitor()");

  jint r = _env->MonitorEnter(object);

  if(checkException(_verbose) || (r != 0))
    throw JavaMonitorException(); // may also mean memory exhausted
}

/*
 */

void JavaContext::exitMonitor(jobject object) throw(JavaException)
{
  Log_debug("JavaContext::exitMonitor()");

  jint r = _env->MonitorExit(object);

  if(checkException(_verbose) || (r != 0))
    throw JavaMonitorException(); // may also mean memory exhausted
}

/*
 */

void JavaContext::registerNativeMethods(
  jclass clazz, const JavaNativeMethod methods[]) throw(JavaException)
{
  Log_debug("JavaContext::registerNativeMethods()");
  int i, len, r = 0;
  String method, descriptor, signature;
  bool is_static, ok = true;

  // figure out how long the vector is
  for(len = 0; methods[len].signature != NULL; len++);

  JNINativeMethod *jnim = new JNINativeMethod[len];

  for(i = 0; i < len; i++)
  {
    signature = methods[i].signature;

    if(! signature)
      break;

    if(parseSignature(signature, method, descriptor, is_static))
    {
      jnim[i].name = const_cast<char *>(method.c_str());
      jnim[i].signature = const_cast<char *>(descriptor.c_str());
      jnim[i].fnPtr = methods[i].function;
    }
    else
    {
      jnim[i].name = NULL;
      jnim[i].signature = NULL;
    }
  }

  if(ok)
    r = (int)_env->RegisterNatives(clazz, jnim, len);

  delete[] jnim;

  if(! ok || (r < 0))
  {
    checkException(_verbose);
    throw JavaMethodNotFoundException();
  }
}

/*
 */

void JavaContext::unregisterNativeMethods(jclass clazz)
{
  Log_debug("JavaContext::unregisterNativeMethods()");

  _env->UnregisterNatives(clazz);
}

/*
 */

void JavaContext::pushLocalFrame(uint_t capacity) throw(JavaException)
{
  Log_debug("JavaContext::pushLocalFrame()");

  jint r = _env->PushLocalFrame((jint)capacity);

  if(checkException(_verbose) || (r != 0))
    throw JavaOutOfMemoryException();
}

/*
 */

void JavaContext::popLocalFrame()
{
  Log_debug("JavaContext::popLocalFrame()");

  _env->PopLocalFrame(NULL);
}

/*
 */

void JavaContext::attach(JavaVirtualMachine *jvm, JNIEnv* env, bool verbose)
  throw()
{
  _jvm = jvm;
  _env = env;
  _verbose = verbose;
}

void JavaContext::detach() throw()
{
  _jvm = NULL;
  _env = NULL;
  _verbose = false;
}

} // namespace ccxx

/* end of source file */
