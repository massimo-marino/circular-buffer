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
#include <map>
#include <memory>
////////////////////////////////////////////////////////////////////////////////
namespace circular_buffer
{
// Non-template base class: parts of the template class that do not depend on
// the template parameters
class cbBase
{
 public:
  // delegating ctor: default ctor sets the circular buffer's size to the default size
  cbBase() : cbBase(m_defaultSize) {}
  ~cbBase() = default;
  cbBase(const cbBase&) = delete;
  cbBase& operator= (const cbBase&) = delete;
  cbBase(const cbBase&&) = delete;
  cbBase& operator= (const cbBase&&) = delete;
  explicit cbBase(const unsigned int cbSize) noexcept(false);

  enum class cbStatus: uint8_t {UNKNOWN, EMPTY, ADDED, REMOVED, FULL};

  unsigned int getNumElements() const noexcept;
  bool isEmpty() const noexcept;
  bool isFull() const noexcept;
  bool isPopulated() const noexcept;
  const std::string& cbStatusString(const cbBase::cbStatus cbs) const noexcept;
  size_t size() const noexcept;

 protected:
  using cbaddret = std::tuple<cbBase::cbStatus, size_t>;
  using statusStringMap = std::map<cbStatus, std::string>;
  static const unsigned int m_defaultSize;
  static statusStringMap const m_statusStrings;  
  const size_t m_cbSize {m_defaultSize};
  // mutables needed since this is a const class: mutable members of const class
  // instances are modifiable
  mutable std::mutex m_mx {};
  mutable unsigned int m_readIndex {0};
  mutable unsigned int m_numElements {0}; 
  bool _isEmpty() const noexcept;
  bool _isFull() const noexcept;
};  // class cbBase

// Template class
template <typename T = int>
class cb final : public cbBase
{
 private:
  using cbremret = std::tuple<cbBase::cbStatus, T, size_t>;
  const T m_noItem {};

 public:
  // delegating ctor: default ctor builds a circular buffer with the default size
  cb() : cb(m_defaultSize) {}
  ~cb() = default;
  cb(const cb&) = delete;
  cb& operator= (const cb&) = delete;
  cb(const cb&&) = delete;
  cb& operator= (const cb&&) = delete;

  // construct a unique pointer that will point to the data of the circular buffer
  std::unique_ptr<T[]> m_pData {};

  explicit cb(const unsigned int cbSize)
  :
  cbBase(cbSize),
  // allocate an array of T's having size cbSize and store the pointer to it in
  // the unique pointer
  m_pData (std::make_unique<T[]>(cbSize))
  {}

  void printData(const std::string&& caller) const noexcept
  {    
    std::lock_guard<std::mutex> lg(m_mx);

    std::cout << "[" << __func__ << "] "
              << "[" << caller << "] "
              << "---data start---\n";

    for (unsigned int i = 0; i < m_cbSize; ++i)
    {
      if ( m_noItem != m_pData.get()[i] || m_readIndex == i)
      {
        std::cout << "[" << __func__ << "] "
                  << "[" << caller << "] "
                  << i
                  << ": '"
                  << m_pData.get()[i]
                  << "'";
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
              << std::endl;
  }

  // add an item in the circular buffer, if not full
  cbaddret add(const T& item) const noexcept
  {
    std::lock_guard<std::mutex> lg(m_mx);

    if ( true == _isFull() )
    {
      // until C++17
      return std::make_tuple(cbBase::cbStatus::FULL, m_cbSize);
    }

    m_pData.get()[(m_readIndex + m_numElements) % m_cbSize] = item;

    // until C++17
    return std::make_tuple(cbBase::cbStatus::ADDED, ++m_numElements);
  }

  // return the first item in the circular buffer, no changes in it
  T getFront() const noexcept
  {
    return m_pData.get()[m_readIndex];
  }

  // remove the first item from the circular buffer, if not empty
  cbremret remove() const noexcept
  {
    std::lock_guard<std::mutex> lg(m_mx);

    if ( true == _isEmpty() )
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

#endif  // CCIRCULARBUFFER_H
