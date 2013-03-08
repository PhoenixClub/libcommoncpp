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

#ifndef __ccxx_AbstractBufferImpl_hxx
#define __ccxx_AbstractBufferImpl_hxx

#ifndef __ccxx_AbstractBuffer_hxx
#error "Do not include this header directly from application code!"
#endif

/*
 */

template <typename T>
  AbstractBuffer<T>::AbstractBuffer(size_t size)
  : _size(size)
{
  _data = new T[_size];
}

/*
 */

template <typename T>
  AbstractBuffer<T>::~AbstractBuffer() throw()
{
  delete[] _data;
}

/*
 */

template <typename T>
  void AbstractBuffer<T>::setSize(size_t newSize)
{
  if(newSize < 1)
    return;

  delete[] _data;
  _data = new T[newSize];
  _size = newSize;

  clear();
}

/*
 */

template <typename T>
  void AbstractBuffer<T>::clear() throw()
{
  std::memset(static_cast<void *>(_data), 0, _size * sizeof(T));
}

#endif // __ccxx_AbstractBufferImpl_hxx

/* end of header file */
