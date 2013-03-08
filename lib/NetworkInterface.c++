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

#include "commonc++/NetworkInterface.h++"

namespace ccxx {

/*
 */

const uint32_t NetworkInterface::FLAG_UP          = 0x0001;
const uint32_t NetworkInterface::FLAG_BROADCAST   = 0x0002;
const uint32_t NetworkInterface::FLAG_DEBUG       = 0x0004;
const uint32_t NetworkInterface::FLAG_LOOPBACK    = 0x0008;
const uint32_t NetworkInterface::FLAG_PPP         = 0x0010;
const uint32_t NetworkInterface::FLAG_NOTRAILERS  = 0x0020;
const uint32_t NetworkInterface::FLAG_RUNNING     = 0x0040;
const uint32_t NetworkInterface::FLAG_NOARP       = 0x0080;
const uint32_t NetworkInterface::FLAG_PROMISCUOUS = 0x0100;
const uint32_t NetworkInterface::FLAG_MULTICAST   = 0x0200;

const uint32_t NetworkInterface::FLAG_MASK        = 0x03FF;

/*
 */

NetworkInterface::NetworkInterface() throw()
  : _mtu(0),
    _metric(0),
    _prefixLength(0),
    _flags(0)
{
}

/*
 */

NetworkInterface::~NetworkInterface() throw()
{
}

/*
 */

void NetworkInterface::setNetmask(const InetAddress &addr)
{
  _netmask = addr;

  uint32_t mask = addr.getAddress();
  _prefixLength = 0;
  while(mask != 0)
  {
    ++_prefixLength;
    mask <<= 1;
  }
}


}; // namespace ccxx

/* end of source file */
