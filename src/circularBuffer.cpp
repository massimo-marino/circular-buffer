/* 
 * File:   circularBuffer.cpp
 * Author: massimo
 * 
 * Created on April 12, 2017, 8:17 PM
 */
#include "circularBuffer.h"
////////////////////////////////////////////////////////////////////////////////
namespace circular_buffer
{
// BEGIN: ignore the warnings listed below when compiled with clang from here
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
cbBase::statusStringMap const cbBase::m_statusStrings
{
   { cbBase::cbStatus::UNKNOWN, "UNKNOWN"}
  ,{ cbBase::cbStatus::EMPTY,   "EMPTY"}
  ,{ cbBase::cbStatus::ADDED,   "ADDED"}
  ,{ cbBase::cbStatus::REMOVED, "REMOVED"}
  ,{ cbBase::cbStatus::FULL,    "FULL"}
};
#pragma clang diagnostic pop
// END: ignore the warnings when compiled with clang up to here

cbBase::cbBase(const unsigned long cbSize) noexcept(false)
:
m_cbSize(cbSize)
{
  if ( 0 == m_cbSize )
  {
    throw std::invalid_argument("ERROR: The size of the circular buffer must not be zero");
  }
}

size_t
cbBase::size() const noexcept
{
  return m_cbSize;
}

bool
cbBase::_isEmpty() const noexcept
{
  return (0 == m_numElements);
}

bool
cbBase::_isFull() const noexcept
{
  return (m_cbSize == m_numElements);
}

unsigned long
cbBase::getNumElements() const noexcept
{
  std::lock_guard<std::mutex> mlg(m_mx);

  return m_numElements;
}

bool
cbBase::isEmpty() const noexcept
{
  std::lock_guard<std::mutex> mlg(m_mx);

  return (0 == m_numElements);
}

bool
cbBase::isFull() const noexcept
{
  std::lock_guard<std::mutex> mlg(m_mx);

  return (m_cbSize == m_numElements);
}

bool
cbBase::isPopulated() const noexcept
{
  std::lock_guard<std::mutex> mlg(m_mx);

  return (m_numElements > 0);
}

const
std::string&
cbBase::cbStatusString(const cbBase::cbStatus cbs) const noexcept(false)
{
  // at() returns a reference to the character at specified location pos.
  // Bounds checking is performed, exception of type std::out_of_range will be
  // thrown on invalid access.
  return m_statusStrings.at(cbs);
}
}  // namespace circular_buffer
////////////////////////////////////////////////////////////////////////////////
