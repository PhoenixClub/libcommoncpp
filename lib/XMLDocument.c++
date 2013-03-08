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

#include <cstdio>

#include "commonc++/XMLDocument.h++"
#include "commonc++/XMLElement.h++"

namespace ccxx {

/*
 */

XMLDocument::XMLDocument(const String &encoding /*="us-ascii"*/)
  : _encoding(encoding), _root(NULL)
{
}

/*
 */

XMLDocument::~XMLDocument() throw()
{
  delete _root;
}

/*
 */

XMLElement & XMLDocument::find(const String &path)
  throw(InvalidArgumentException)
{
  const XMLElement &e = _find(path);
  return(const_cast<XMLElement &>(e));
}

/*
 */

const XMLElement & XMLDocument::find(const String &path) const
  throw(InvalidArgumentException)
{
  const XMLElement &e = _find(path);
  return(e);
}

/*
 */

const XMLElement & XMLDocument::_find(const String &path) const
  throw(InvalidArgumentException)
{
  uint_t index = 0;
  XMLElement *cur = _root;

  if(! cur)
    return(XMLElement::null);

  String tok = path.nextToken(index, "./");
  XMLElement::validateName(tok);

  if(tok != cur->getName())
    return(XMLElement::null);

  for(;;)
  {
    tok = path.nextToken(index, "./");

    if(tok.isNull())
      break;

    XMLElement::validateName(tok);

    cur = &(cur->getChild(tok));

    if(! cur)
      return(XMLElement::null);
  }

  return(cur ? *cur : XMLElement::null);
}

/*
 */

void XMLDocument::read(const String& str) throw(ParseException)
{
  delete _root;
  _root = NULL;

  XMLParser parser(this, _encoding);

  const char *buf = str.c_str();
  size_t len = str.getLength();

  parser.parse(buf, len, true);
}

/*
 */

void XMLDocument::read(std::istream& stream) throw(ParseException, IOException)
{
  delete _root;
  _root = NULL;

  XMLParser parser(this, _encoding);
  char buf[1024];
  bool done = false;

  do
  {
    stream.read(buf, sizeof(buf));
    int len = stream.gcount();

    if(stream.fail() && ! stream.eof())
      throw IOException("Error while reading file");

    done = stream.eof();
    parser.parse(buf, len, done);
  }
  while(! done);
}

/*
 */

void XMLDocument::write(std::ostream& stream, uint_t tabWidth /* = 2 */)
  const throw(IOException)
{
  stream << "<?xml version=\"1.0\" encoding=\"" << _encoding << "\"?>";
  if(tabWidth)
    stream << '\n';

  if(_root != NULL)
    _root->write(stream, 0, tabWidth);
}

/*
 */

void XMLDocument::write(String& str, uint_t tabWidth /* = 2 */) const
{
  std::stringstream ss;

  try
  {
    write(ss, tabWidth);
  }
  catch(IOException& ex)
  {
    // won't happen
  }

  str = ss.str();
}

/*
 */

XMLElement & XMLDocument::getRoot()
{
  return(_root ? *_root : XMLElement::null);
}

/*
 */

const XMLElement & XMLDocument::getRoot() const
{
  return(_root ? *_root : XMLElement::null);
}

/*
 */

XMLElement & XMLDocument::setRoot(const String& name)
  throw(InvalidArgumentException)
{
  XMLElement::validateName(name);

  delete _root;
  _root = new XMLElement(name);

  return(*_root);
}

/*
 */

void XMLDocument::_elementStartHandler(void *userData, const XML_Char *name,
                                       const XML_Char **attr)
{
  XMLParser *parser = reinterpret_cast<XMLParser *>(userData);
  XMLDocument *doc = parser->_document;

  XMLElement *elem;
  if(parser->_current)
    elem = &(parser->_current->addChild(name));
  else
    elem = &(doc->setRoot(name));

  for(int i = 0; attr[i]; i += 2)
  {
    String name = (const char *)attr[i];
    String value = (const char *)attr[i + 1];

    elem->setAttribute(name, value);
  }

  parser->_current = elem; // push
}

/*
 */

void XMLDocument::_elementEndHandler(void *userData, const XML_Char *name)
{
  XMLParser *parser = reinterpret_cast<XMLParser *>(userData);

  if(! parser->_charData.isEmpty())
  {
    parser->_charData.trim();
    parser->_current->setContent(parser->_charData);
    parser->_charData = "";
  }

  parser->_current = &(parser->_current->getParent()); // pop
}

/*
 */

void XMLDocument::_charDataHandler(void *userData, const XML_Char *data,
                                   int len)
{
  XMLParser *parser = reinterpret_cast<XMLParser *>(userData);

  parser->appendCharData(data, len);
}

/*
 */

void XMLDocument::_commentHandler(void *userData, const XML_Char *data)
{
  // XMLParser *parser = reinterpret_cast<XMLParser *>(userData);
  // ignore comments for now
}

/*
 */

XMLDocument::XMLParser::XMLParser(XMLDocument *owner,
                                  const String& encoding /*= "us-ascii"*/)
  : _document(owner),
    _current(NULL),
    _parser(NULL),
    _status(true)
{
  _parser = XML_ParserCreate(encoding.c_str());

  XML_SetElementHandler(_parser, &XMLDocument::_elementStartHandler,
                        &XMLDocument::_elementEndHandler);
  XML_SetCharacterDataHandler(_parser, &XMLDocument::_charDataHandler);
  XML_SetCommentHandler(_parser, &XMLDocument::_commentHandler);

  XML_SetUserData(_parser, this);
}

/*
 */

XMLDocument::XMLParser::~XMLParser()
{
  XML_ParserFree(_parser);
}

/*
 */

void XMLDocument::XMLParser::parse(const char *data, size_t length,
                                   bool done /* = true */)
  throw(ParseException)
{
  if(! XML_Parse(_parser, data, length, done))
    throw(ParseException(
            (char *)(XML_ErrorString(XML_GetErrorCode(_parser))),
            static_cast<int>(XML_GetCurrentLineNumber(_parser)),
            static_cast<int>(XML_GetCurrentColumnNumber(_parser))));
}


}; // namespace ccxx

/* end of source file */
