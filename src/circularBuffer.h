/* 
 * File:   circularBuffer.h
 * Author: massimo
 *
 * Created on April 12, 2017, 8:17 PM
 */
#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <mutex>
#include <map>
#include <memory>
#include <tuple>
////////////////////////////////////////////////////////////////////////////////
namespace circular_buffer
{
// Non-template base class: parts of the template class that do not depend on
// the template parameters
class cbBase
{
 protected:
  // delegating ctor: default ctor sets the circular buffer's size to the default size
  cbBase() : cbBase(m_defaultSize) {}

  cbBase(const cbBase&) = delete;
  cbBase& operator= (const cbBase&) = delete;
  cbBase(const cbBase&&) = delete;
  cbBase& operator= (const cbBase&&) = delete;
  explicit cbBase(unsigned long cbSize) noexcept(false);

 public:
  enum class cbStatus : uint8_t {UNKNOWN, EMPTY, ADDED, REMOVED, FULL};

  unsigned long getNumElements() const noexcept;
  bool isEmpty() const noexcept;
  bool isFull() const noexcept;
  bool isPopulated() const noexcept;

  const
  std::string&
  cbStatusString(cbBase::cbStatus cbs) const noexcept(false)
  {
    // at() returns a reference to the character at specified location pos.
    // Bounds checking is performed, exception of type std::out_of_range will be
    // thrown on invalid access.
    return m_statusStrings.at(cbs);
  }

  constexpr
  size_t
  size() const noexcept
  {
    return m_cbSize;
  }

 protected:
  using cbaddret = std::tuple<cbBase::cbStatus, size_t>;
  using statusStringMap = std::map<cbStatus, std::string>;
  static inline const unsigned long m_defaultSize {3};
  static statusStringMap const m_statusStrings;
  const size_t m_cbSize {m_defaultSize};
  // mutables needed since this is a const class: mutable members of const class
  // instances are modifiable
  mutable std::mutex m_mx {};
  mutable unsigned long m_readIndex {0};
  mutable unsigned long m_numElements {0};

  constexpr
  bool
  _isEmpty() const noexcept
  {
    return (0 == m_numElements);
  }

  constexpr
  bool
  _isFull() const noexcept
  {
    return (m_cbSize == m_numElements);
  }
};  // class cbBase

// Template class
template <typename T = int>
class cb final : public cbBase
{
  using cbremret = std::tuple<cbBase::cbStatus, T, size_t>;

 private:
  constexpr static inline T m_noItem {};

 public:
  // we don't want these objects allocated on the heap
  void* operator new(std::size_t) = delete;
  void* operator new[](std::size_t) = delete;

  void operator delete(void*) = delete;
  void operator delete[](void*) = delete;

  // delegating ctor: default ctor builds a circular buffer with the default size
  cb() : cb(m_defaultSize) {}

  cb(const cb&) = delete;
  cb& operator= (const cb&) = delete;
  cb(const cb&&) = delete;
  cb& operator= (const cb&&) = delete;

  // construct a unique pointer that will point to the data of the circular buffer
  std::unique_ptr<T[]> m_pData {};

  explicit
  cb(const unsigned long cbSize)
  :
  cbBase(cbSize),
  // allocate an array of T's having size cbSize and store the pointer to it in
  // the unique pointer
  m_pData (std::make_unique<T[]>(cbSize))
  {}

  void
  printData(const std::string&& caller = "caller-unspecified") const noexcept
  {
    std::lock_guard<std::mutex> lg(m_mx);

    std::cout << "[" << __func__ << "] "
              << "[" << caller << "] "
              << "---data start---\n"
              << std::fixed;

    for (unsigned long i {0}; i < m_cbSize; ++i)
    {
      auto d {m_pData.get()[i]};

      if ( m_noItem != d || m_readIndex == i)
      {
        std::cout << "[" << __func__ << "] "
                  << "[" << caller << "] "
                  << i
                  << ": '";
        if ( 1 == sizeof(decltype(d)) )
        {
          if ( std::is_signed<decltype(d)>::value )
          {
            std::cout << static_cast<int16_t>(d);
          }
          else
          {
            std::cout << static_cast<uint16_t>(d);
          }
        }
        else
        {
          std::cout << d;
        }
        std::cout << "'";
        if ( m_readIndex != i )
        {
          std::cout << "\n";
        }
        else
        {
          std::cout << "  <--- Head\n";
        }
      }
    }
    std::cout << "[" << __func__ << "] "
              << "[" << caller << "] "
              << "---data end---\n"
              << "\n";
  }

  // add an item in the circular buffer, if not full
  cbaddret
  add(const T& item) const noexcept
  {
    std::lock_guard<std::mutex> lg(m_mx);

    if ( _isFull() )
    {
      // until C++17
      return std::make_tuple(cbBase::cbStatus::FULL, m_cbSize);
    }

    m_pData.get()[(m_readIndex + m_numElements) % m_cbSize] = item;

    // until C++17
    return std::make_tuple(cbBase::cbStatus::ADDED, ++m_numElements);
  }

  // return the first item in the circular buffer, no changes in it
  constexpr
  T
  getFront() const noexcept
  {
    return m_pData.get()[m_readIndex];
  }

  // remove the first item from the circular buffer, if not empty
  cbremret
  remove() const noexcept
  {
    std::lock_guard<std::mutex> lg(m_mx);

    if ( _isEmpty() )
    {
      // until C++17
      return std::make_tuple(cbBase::cbStatus::EMPTY, m_noItem, 0);
    }

    // until C++17
    auto t = std::make_tuple(cbBase::cbStatus::REMOVED, getFront(), --m_numElements);

    m_pData.get()[m_readIndex] = m_noItem;
    m_readIndex = (m_readIndex + 1) % m_cbSize;

    return t;
  }
};  // class cb
}  // namespace circular_buffer

