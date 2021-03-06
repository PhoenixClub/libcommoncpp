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

#include "commonc++/PluginLoader.h++"

namespace ccxx {

/*
 */

PluginLoader::PluginLoader(const String& path /* = String::null */)
  : _module(path)
{
}

/*
 */

PluginLoader::~PluginLoader()
{
}

/*
 */

String PluginLoader::getName()
{
  return(_fetchString("ccxx_plugin_name"));
}

/*
 */

String PluginLoader::getVersion()
{
  return(_fetchString("ccxx_plugin_version"));
}

/*
 */

String PluginLoader::getAuthor()
{
  return(_fetchString("ccxx_plugin_author"));
}

/*
 */

String PluginLoader::getClassName()
{
  return(_fetchString("ccxx_plugin_class_name"));
}

/*
 */

String PluginLoader::getBuildDate()
{
  return(_fetchString("ccxx_plugin_build_date"));
}

/*
 */

String PluginLoader::_fetchString(const char *symbol)
{
  if(! _module.isOpen())
    return(String::null);

  void *f = _module[symbol];
  if(! f)
    return(String::null);

  const char *(*func)() = (const char *(*)())f;
  return String(func());
}

/*
 */

Plugin *PluginLoader::_newInstance()
{
  if(! _module.isOpen())
    return(NULL);

  void *f = _module["ccxx_plugin_new"];
  if(! f)
    return(NULL);

  Plugin *(*func)() = (Plugin *(*)())f;

  Plugin *p = func();

  return(p);
}


} // namespace ccxx

/* end of source file */
