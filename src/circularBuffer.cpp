/* 
 * File:   circularBuffer.cpp
 */
#include "circularBuffer.h"
////////////////////////////////////////////////////////////////////////////////
namespace circular_buffer
{
cbBase::statusStringMap const cbBase::m_statusStrings
{
   { cbBase::cbStatus::UNKNOWN, "UNKNOWN"}
  ,{ cbBase::cbStatus::EMPTY,   "EMPTY"}
  ,{ cbBase::cbStatus::ADDED,   "ADDED"}
  ,{ cbBase::cbStatus::REMOVED, "REMOVED"}
  ,{ cbBase::cbStatus::FULL,    "FULL"}
};

cbBase::cbBase(const unsigned long cbSize) noexcept(false)
:
m_cbSize(cbSize)
{
  if ( 0 == m_cbSize )
  {
    throw std::invalid_argument("ERROR: The size of the circular buffer must not be zero");
  }
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
}  // namespace circular_buffer
////////////////////////////////////////////////////////////////////////////////

