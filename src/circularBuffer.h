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
 * File:   circularBuffer.h
 * Author: massimo
 *
 * Created on April 12, 2017, 8:17 PM
 */

#ifndef CCIRCULARBUFFER_H
#define CCIRCULARBUFFER_H

#include <iostream>
#include <mutex>
#include <string>
#include <map>
#include <tuple>
#include <memory>
#include <stdexcept>
////////////////////////////////////////////////////////////////////////////////
namespace circular_buffer
{
// Non-template base class: parts of the template class that do not depend on
// the template parameters
class cbBase
{
 public:
  cbBase() = delete;
  ~cbBase() = default;
  cbBase(const cbBase&) = delete;
  cbBase& operator= (const cbBase&) = delete;

  explicit cbBase(const unsigned int cbSize = cbBase::m_defaultSize)
  :
  m_cbSize(cbSize)
  {
    if ( 0 == m_cbSize )
    {
      throw std::invalid_argument("ERROR: The size of the circular buffer must not be zero");
    }
  }

  enum class cbStatus: uint8_t {EMPTY, ADDED, REMOVED, FULL};

 public:
  unsigned int getNumElements() const noexcept;
  bool isEmpty() const noexcept;
  bool isFull() const noexcept;
  bool isPopulated() const noexcept;
  const std::string& cbStatusString(const cbBase::cbStatus cbs) const noexcept;

 protected:
  using cbaddret = std::tuple<cbBase::cbStatus, size_t>;
  static const unsigned int m_defaultSize;
  using statusStringMap = std::map<cbStatus, std::string>;
  static statusStringMap const m_statusStrings;  
  const size_t m_cbSize {m_defaultSize};
  // mutables needed since this is a const class: mutable members of const class
  // instances are modifiable
  mutable std::mutex m_mx {};
  mutable unsigned int m_readIndex {0};
  mutable unsigned int m_numElements {0};
};  // class cbBase

// Template class
template <typename T = int>
class cb: public cbBase
{
 private:
  using cbremret = std::tuple<cbBase::cbStatus, T, size_t>;

 public:
  cb() = delete;
  ~cb() = default;
  cb(const cb&) = delete;
  cb& operator= (const cb&) = delete;

  explicit cb(const unsigned int cbSize = cbBase::m_defaultSize)
  :
  cbBase(cbSize),
  // allocate an array of T's having size cbSize and store the pointer to it in
  // the unique pointer
  m_pData ( std::make_unique<T[]>( cbSize ))
  {}

  // construct a unique pointer that will point to the data of the circular buffer
  std::unique_ptr<T[]> m_pData{};

  size_t size() const noexcept
  {
    return m_cbSize;
  }

  void printData(const std::string& caller) const noexcept
  {
    std::lock_guard<std::mutex> lg(m_mx);

    unsigned int ui {0};

    std::cout << "[" << __func__ << "] "
              << "[" << caller << "] "
              << "---data start---\n";

    for (unsigned int i = 0; i < m_cbSize; ++i)
    {
      std::cout << "[" << __func__ << "] "
                << "[" << caller << "] "
                << ui
                << ": '"
                << m_pData.get()[i]
                << "'";
      if ( m_readIndex != ui )
      {
        std::cout << "\n";
      } else {
        std::cout << "  <--- Head\n";
      }
      ++ui;
    }
    std::cout << "[" << __func__ << "] "
              << "[" << caller << "] "
              << "---data end---\n"
              << std::endl;
  }

  cbaddret add(const T& item) const noexcept
  {
    if ( true == isFull() )
    {
      // until C++17
      return std::make_tuple(cbBase::cbStatus::FULL, m_cbSize);
    }

    std::lock_guard<std::mutex> lg(m_mx);

    m_pData.get()[(m_readIndex + m_numElements) % m_cbSize] = item;
    ++m_numElements;

    // until C++17
    return std::make_tuple(cbBase::cbStatus::ADDED, m_numElements);
  }
  
  cbremret remove() const noexcept
  {
    constexpr T noItem {};

    if ( true == isEmpty() )
    {
      // until C++17
      return std::make_tuple(cbBase::cbStatus::EMPTY, noItem, 0);
    }

    std::lock_guard<std::mutex> lg(m_mx);

    // until C++17
    auto t = std::make_tuple(cbBase::cbStatus::REMOVED, m_pData.get()[m_readIndex], --m_numElements);

    m_pData.get()[m_readIndex] = noItem;
    m_readIndex = (m_readIndex + 1) % m_cbSize;

    return t;
  }
};  // class cb
}  // namespace circular_buffer

#endif  // CCIRCULARBUFFER_H
