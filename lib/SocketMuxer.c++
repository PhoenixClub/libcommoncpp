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

#include "commonc++/SocketMuxer.h++"
#include "commonc++/ScopedLock.h++"
#include "commonc++/SocketUtil.h++"
#include "commonc++/System.h++"

#ifdef CCXX_OS_POSIX
#include <sys/time.h>
#include <unistd.h>
#endif

#include <cerrno>
#include <list>

namespace ccxx {

/*
 */

class SocketMuxer::ConnectionList : public std::list<Connection *>
{
};

/*
 */

SocketMuxer::SocketMuxer(uint_t maxConnections /* = 64 */,
                         uint_t defaultIdleLimit /* = 0 */,
                         uint_t sleepInterval /* = 100 */)
  : _connections(new ConnectionList()),
    _mutex(true),
    _pool(maxConnections == 0 ? 1 : maxConnections),
    _idleLimit(defaultIdleLimit),
    _sleepIntervalSec(sleepInterval / 1000),
    _sleepIntervalMicroSec((sleepInterval % 1000) * 1000),
    _ssock(NULL)
{
}

/*
 */

SocketMuxer::~SocketMuxer() throw()
{
  delete _connections;
}

/*
 */

bool SocketMuxer::init(ServerSocket *socket)
{
  if(_ssock != NULL)
    return(socket == _ssock); // ensure idempotence

  if((socket == NULL) || !socket->isListening())
    return(false);

  _ssock = socket;
  return(true);
}

/*
 */

void SocketMuxer::cleanup()
{
  ScopedLock lock(_mutex);

  for(ConnectionList::const_iterator iter = _connections->begin();
      iter != _connections->end();
      ++iter)
  {
    Connection *conn = *iter;
    _connectionClosed(conn);
  }

  _connections->clear();
}

/*
 */

size_t SocketMuxer::getConnectionCount() const
{
  return(_connections->size());
}

/*
 */

void SocketMuxer::dataReceivedOOB(Connection *connection)
{
  // no-op
}

/*
 */

void SocketMuxer::run()
{
  if(! _ssock)
  {
    // Not properly initialized; return immediately.
    return;
  }

  SocketHandle ms = _ssock->getSocketHandle();
  fd_set readfd, writefd, exceptfd;
  struct timeval tv;

  while(! testCancel())
  {
    // populate the fd sets

    FD_ZERO(&readfd);
    FD_ZERO(&writefd);
    FD_ZERO(&exceptfd);

    // server-socket specific:
    FD_SET(ms, &readfd);

    for(ConnectionList::const_iterator iter = _connections->begin();
        iter != _connections->end();
        ++iter)
    {
      Connection *conn = *iter;
      SocketHandle fd = conn->getSocket()->getSocketHandle();

      if(! conn->isReadHigh())
        FD_SET(fd, &readfd);

      if(! conn->isWriteLow())
        FD_SET(fd, &writefd);

      if(! conn->getOOBFlag())
        FD_SET(fd, &exceptfd);
    }

    // select on fd sets

  SELECT:
    tv.tv_sec = _sleepIntervalSec;
    tv.tv_usec = _sleepIntervalMicroSec;
    int r = ::select(FD_SETSIZE, &readfd, &writefd, &exceptfd, &tv);

    if(r < 0)
    {
      if(r == EINTR)
        goto SELECT;

      // otherwise an unrecoverable error
      break;
    }

    // now we may have descriptors ready

    time_ms_t now = System::currentTimeMillis();

    // server-socket specific:
    // check if master socket is ready for read

    if((r > 0) && FD_ISSET(ms, &readfd))
    {
      // a new connection is pending
      StreamSocket *sock = NULL;

      try
      {
        sock = _pool.reserve();
        _ssock->accept(*sock);

        Connection *conn = connectionReady(sock->getRemoteAddress());
        if(! conn)
        {
          sock->close();
          _pool.release(sock);
        }
        else
        {
          conn->attach(sock);
          conn->setTimestamp(now);
          _connections->push_back(conn);
        }
      }
      catch(const ObjectPoolException &)
      {
        // too many connections
        SocketUtil::closeSocket(::accept(ms, NULL, NULL));
      }
      catch(const SocketException &)
      {
        // accept failed
        _pool.release(sock);
      }
    }

    // now check all active connections

    ScopedLock lock(_mutex);

    for(ConnectionList::iterator iter = _connections->begin();
        iter != _connections->end();
      )
    {
      Connection *conn = *iter;
      StreamSocket *sock = conn->getSocket();
      SocketHandle fd = sock->getSocketHandle();

      if(conn->isClosePending())
      {
        // had a pending close; so preform it now
        _connectionClosed(conn);
      }
      else if((r > 0) && FD_ISSET(fd, &readfd))
      {
        try
        {
          bool rcvd = false;

          conn->_readLock.enter();

          // read as much data as possible
          conn->read();
          conn->setTimestamp(now);

          if(! conn->isReadLow())
            rcvd = true;

          conn->setOOBFlag(false);

          conn->_readLock.leave();

          if(rcvd)
            dataReceived(conn);
        }
        catch(const EOFException &)
        {
          _connectionClosed(conn);
        }
        catch(const IOException& ex)
        {
          exceptionOccurred(conn, ex);
        }
      }
      else if(FD_ISSET(fd, &writefd))
      {
        try
        {
          bool low = false;

          conn->_writeLock.enter();

          // write as much data as possible
          conn->write();
          low = conn->isWriteLow();

          conn->_writeLock.leave();

          if(low)
            dataSent(conn);
        }
        catch(const EOFException &)
        {
          _connectionClosed(conn);
        }
        catch(const IOException& ex)
        {
          exceptionOccurred(conn, ex);
        }
      }
      else if((r > 0) && FD_ISSET(fd, &exceptfd))
      {
        try
        {
          conn->_readLock.enter();

          conn->readOOB();
          conn->setOOBFlag(true);
          dataReceivedOOB(conn);

          conn->_readLock.leave();
        }
        catch(const IOException& ex)
        {
          exceptionOccurred(conn, ex);
        }
      }
      else
      {
        // descriptor NOT set...check if connection timed out
        if((_idleLimit > 0) && ((now - conn->getTimestamp())
                                > static_cast<int64_t>(_idleLimit)))
        {
          _connectionTimedOut(conn);
        }
      }

      // if socket is no longer connected, remove connection from list
      if(! sock->isConnected())
        iter = _connections->erase(iter);
      else
        ++iter;
    }
  }
}

/*
 */

void SocketMuxer::_connectionClosed(Connection *conn)
{
  StreamSocket* sock = conn->getSocket();

  conn->close(true);
  _pool.release(sock);

  connectionClosed(conn);
}

/*
 */

void SocketMuxer::_connectionTimedOut(Connection *conn)
{
  StreamSocket* sock = conn->getSocket();

  conn->close(true);
  _pool.release(sock);

  connectionTimedOut(conn);
}

/*
 */

void SocketMuxer::exceptionOccurred(Connection *connection,
                                    const IOException& ex)
{
  connection->close(true);
}

/*
 */

void SocketMuxer::dataSent(Connection *connection)
{
  // no-op
}

/*
 */

uint_t SocketMuxer::writeAll(const byte_t *buf, size_t count)
{
  ScopedLock lock(_mutex);

  uint_t n = 0;

  for(ConnectionList::iterator iter = _connections->begin();
      iter != _connections->end();
      ++iter)
  {
    Connection *conn = *iter;

    if(conn->writeData(buf, count))
      ++n;
  }

  return(n);
}

/*
 */

const size_t Connection::DEFAULT_BUFFER_SIZE = 4096;

/*
 */

Connection::Connection(size_t bufferSize /* = DEFAULT_BUFFER_SIZE */)
  : _socket(NULL),
    readBuffer(bufferSize),
    writeBuffer(bufferSize),
    _readLoMark(1),
    _readHiMark(bufferSize),
    _writeLoMark(1),
    _writeHiMark(bufferSize),
    _oobFlag(false),
    _closePending(false),
    _oobData(0),
    _lastRecv(INT64_CONST(0))
{
}

/*
 */

Connection::~Connection() throw()
{
}

/*
 */

bool Connection::writeData(ByteBuffer &buffer)
{
  ScopedLock guard(_writeLock);

  if(writeBuffer.getFree() < buffer.getRemaining())
    return(false);

  writeBuffer.write(buffer);

  return(true);
}

/*
 */

bool Connection::writeData(const byte_t *buf, size_t count)
{
  ScopedLock lock(_writeLock);

  if(writeBuffer.getFree() < count)
    return(false);

  writeBuffer.write(buf, count);

  return(true);
}

/*
 */

bool Connection::writeLine(const String &text)
{
  ScopedLock lock(_writeLock);

  size_t len = text.length();
  if(writeBuffer.getFree() < (len + 2))
    return(false);

  writeBuffer.write((const byte_t *)(text.c_str()), len);
  writeBuffer.write((const byte_t *)"\r\n", 2);

  return(true);
}

/*
 */

size_t Connection::readData(ByteBuffer &buffer, bool fully /* = true */)
{
  ScopedLock lock(_readLock);

  size_t left = readBuffer.getRemaining();

  if(left == 0 || (fully && (left < buffer.getRemaining())))
    return(0);

  return(readBuffer.read(buffer));
}

/*
 */

size_t Connection::readData(byte_t *buf, size_t count, bool fully /* = true */)
{
  ScopedLock lock(_readLock);

  size_t left = readBuffer.getRemaining();

  if(left == 0 || (fully && (left < count)))
    return(0);

  return(readBuffer.read(buf, count));
}

/*
 */

size_t Connection::readLine(String &text, size_t maxLen)
{
  ScopedLock lock(_readLock);

  text.setLength(0);
  bool found = false;
  size_t len = readBuffer.peek('\n', maxLen, found);

  if(len > 0)
  {
    size_t ext = readBuffer.getReadExtent();
    size_t left = len;

    if(left > ext)
    {
      text.append((const char *)readBuffer.getReadPos(), ext);
      left -= ext;
      readBuffer.advanceReadPos(ext);
    }

    text.append((const char *)readBuffer.getReadPos(), left);
    readBuffer.advanceReadPos(left);
  }

  return(len);
}

/*
 */

void Connection::setReadLowWaterMark(size_t count) throw()
{
  if((count > 0) && (count < _readHiMark))
    _readLoMark = count;
}

/*
 */

void Connection::setReadHighWaterMark(size_t count) throw()
{
  if((count > _readLoMark) && (count <= readBuffer.getSize()))
    _readHiMark = count;
}

/*
 */

void Connection::setWriteLowWaterMark(size_t count) throw()
{
  if((count > 0) && (count < _writeHiMark))
    _writeLoMark = count;
}

/*
 */

void Connection::setWriteHighWaterMark(size_t count) throw()
{
  if((count > _writeLoMark) && (count <= readBuffer.getSize()))
    _writeHiMark = count;
}

/*
 */

void Connection::attach(StreamSocket *socket) throw()
{
  _socket = socket;
  _closePending = false;
}

/*
 */

bool Connection::isClosePending() const throw()
{
  return(_closePending && writeBuffer.isEmpty());
}

/*
 */

void Connection::read() throw(IOException)
{
  readBuffer.write(*_socket);
}

/*
 */

void Connection::write() throw(IOException)
{
  writeBuffer.read(*_socket);
}

/*
 */

void Connection::readOOB() throw(IOException)
{
  _oobData = _socket->readOOB();
}

/*
 */

void Connection::close(bool immediate /* = false */) throw()
{
  if(_socket)
  {
    if(immediate)
      _socket->close();
    else
      _closePending = true;
  }
}

/*
 */

bool Connection::isReadLow() const throw()
{
  ScopedLock lock(_readLock);

  return(readBuffer.getRemaining() < _readLoMark);
}

/*
 */

bool Connection::isReadHigh() const throw()
{
  ScopedLock lock(_readLock);

  return(readBuffer.getRemaining() >= _readHiMark);
}

/*
 */

bool Connection::isWriteLow() const throw()
{
  ScopedLock lock(_writeLock);

  return(writeBuffer.getRemaining() < _writeLoMark);
}

/*
 */

bool Connection::isWriteHigh() const throw()
{
  ScopedLock lock(_writeLock);

  return(writeBuffer.getRemaining() >= _writeHiMark);
}


} // namespace ccxx

/* end of source file */
