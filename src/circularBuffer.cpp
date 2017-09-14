/*
 * Copyright (C) 2017 massimo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

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
constexpr unsigned int cbBase::m_defaultSize {3};

// BEGIN: ignore the warnings listed below when compiled with clang from here
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
cbBase::statusStringMap const cbBase::m_statusStrings
{
   { cbBase::cbStatus::EMPTY,   "EMPTY"}
  ,{ cbBase::cbStatus::ADDED,   "ADDED"}
  ,{ cbBase::cbStatus::REMOVED, "REMOVED"}
  ,{ cbBase::cbStatus::FULL,    "FULL"}
};
#pragma clang diagnostic pop
// END: ignore the warnings when compiled with clang up to here

unsigned int cbBase::getNumElements() const noexcept
{
  std::lock_guard<std::mutex> mlg(m_mx);

  return m_numElements;
}

bool cbBase::isEmpty() const noexcept
{
  std::lock_guard<std::mutex> mlg(m_mx);

  return (0 == m_numElements);
}

bool cbBase::isFull() const noexcept
{
  std::lock_guard<std::mutex> mlg(m_mx);

  return (m_cbSize == m_numElements);
}

bool cbBase::isPopulated() const noexcept
{
  std::lock_guard<std::mutex> mlg(m_mx);

  return (m_numElements > 0);
}

const std::string& cbBase::cbStatusString(const cbBase::cbStatus cbs) const noexcept
{
  return m_statusStrings.at(cbs);
}
}  // namespace circular_buffer
////////////////////////////////////////////////////////////////////////////////
