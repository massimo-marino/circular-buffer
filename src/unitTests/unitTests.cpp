/* 
 * File:   unitTests.cpp
 * Author: massimo
 * 
 * Created on April 12, 2017, 8:17 PM
 */
#include "../circularBuffer.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;
////////////////////////////////////////////////////////////////////////////////
// These tests run with numerical types only: bool, char, short int, int, ...
// The type of data stored in the circular buffer
using cbtype = uint16_t;

// Alias for the circular buffer used in the example
using cb_t = circular_buffer::cb<cbtype>;

// Tests
TEST(circularBuffer, test_1)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {0};

  EXPECT_THROW(cb_t aCircularBuffer(cbsize), std::invalid_argument);
}

TEST(circularBuffer, test_2)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {100};

  EXPECT_NO_THROW(cb_t aCircularBuffer(cbsize));
}

TEST(circularBuffer, test_3)
{
  // default size set
  EXPECT_NO_THROW(cb_t aCircularBuffer {});
}

TEST(circularBuffer, test_3A)
{
  // default size set
  cb_t aCircularBuffer{};

  // the default size is a private attribute of class cbBase set to 3
  ASSERT_EQ(3, aCircularBuffer.size());
}

TEST(circularBuffer, test_3B)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {1230};
  auto shptr {std::make_shared<cb_t>(cbsize)};

  EXPECT_EQ(cbsize, shptr->size());
}

TEST(circularBuffer, test_4)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {100};
  cb_t aCircularBuffer{cbsize};

  ASSERT_EQ(cbsize, aCircularBuffer.size());
  ASSERT_EQ(cbtype{}, aCircularBuffer.getFront());
}

TEST(circularBuffer, test_4A)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {100};
  cb_t aCircularBuffer(cbsize);

  ASSERT_EQ(cbsize, aCircularBuffer.size());
  ASSERT_EQ(cbtype{}, aCircularBuffer.getFront());
}

TEST(circularBuffer, test_5)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {100};
  cb_t aCircularBuffer(cbsize);

  ASSERT_EQ(true, aCircularBuffer.isEmpty());
}

TEST(circularBuffer, test_6)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {100};
  cb_t aCircularBuffer(cbsize);

  ASSERT_EQ(false, aCircularBuffer.isFull());
}

TEST(circularBuffer, test_7)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {100};
  cb_t aCircularBuffer(cbsize);

  ASSERT_EQ(false, aCircularBuffer.isPopulated());
}

TEST(circularBuffer, test_8)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {100};
  cb_t aCircularBuffer(cbsize);

  ASSERT_EQ(0, aCircularBuffer.getNumElements());
}

TEST(circularBuffer, test_9)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {100};
  cb_t aCircularBuffer(cbsize);

  circular_buffer::cbBase::cbStatus cbS {};
  cbtype item {};
  size_t numElements {};

  ASSERT_EQ(true, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(false, aCircularBuffer.isPopulated());

  // try to remove from an empty circular buffer
  std::tie(cbS, item, numElements) = aCircularBuffer.remove();

  ASSERT_EQ(true, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(false, aCircularBuffer.isPopulated());
  ASSERT_EQ(circular_buffer::cbBase::cbStatus::EMPTY, cbS);
  ASSERT_EQ(cbtype {}, item);
  ASSERT_EQ(0, numElements);
}

TEST(circularBuffer, test_10)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {1};
  cb_t aCircularBuffer(cbsize);

  circular_buffer::cbBase::cbStatus cbS {};
  size_t numElements {};

  ASSERT_EQ(true, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(false, aCircularBuffer.isPopulated());

  // try to add an item to an empty circular buffer of size 1
  std::tie(cbS, numElements) = aCircularBuffer.add(123);

  ASSERT_EQ(false, aCircularBuffer.isEmpty());
  ASSERT_EQ(true, aCircularBuffer.isFull());
  ASSERT_EQ(true, aCircularBuffer.isPopulated());
  ASSERT_EQ(circular_buffer::cbBase::cbStatus::ADDED, cbS);
  ASSERT_EQ(1, numElements);
}

TEST(circularBuffer, test_11)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {2};
  cb_t aCircularBuffer(cbsize);

  circular_buffer::cbBase::cbStatus cbS {};
  cbtype item {123};
  size_t numElements {};

  ASSERT_EQ(true, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(false, aCircularBuffer.isPopulated());

  // try to add an item to an empty circular buffer of size 2
  std::tie(cbS, numElements) = aCircularBuffer.add(item);

  ASSERT_EQ(false, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(true, aCircularBuffer.isPopulated());
  ASSERT_EQ(circular_buffer::cbBase::cbStatus::ADDED, cbS);
  ASSERT_EQ(1, numElements);
}

TEST(circularBuffer, test_12)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {2};
  cb_t aCircularBuffer(cbsize);

  circular_buffer::cbBase::cbStatus cbS {};
  size_t numElements {};

  ASSERT_EQ(true, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(false, aCircularBuffer.isPopulated());

  // try to add two items to an empty circular buffer of size 2
  aCircularBuffer.add(123);
  std::tie(cbS, numElements) = aCircularBuffer.add(456);

  ASSERT_EQ(false, aCircularBuffer.isEmpty());
  ASSERT_EQ(true, aCircularBuffer.isFull());
  ASSERT_EQ(true, aCircularBuffer.isPopulated());
  ASSERT_EQ(circular_buffer::cbBase::cbStatus::ADDED, cbS);
  ASSERT_EQ(2, numElements);
}

TEST(circularBuffer, test_13)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {2};
  cb_t aCircularBuffer(cbsize);

  circular_buffer::cbBase::cbStatus cbS {};
  size_t numElements {};

  ASSERT_EQ(true, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(false, aCircularBuffer.isPopulated());

  // try to add three items to an empty circular buffer of size 2
  aCircularBuffer.add(123);
  aCircularBuffer.add(456);
  std::tie(cbS, numElements) = aCircularBuffer.add(789);

  ASSERT_EQ(false, aCircularBuffer.isEmpty());
  ASSERT_EQ(true, aCircularBuffer.isFull());
  ASSERT_EQ(true, aCircularBuffer.isPopulated());
  ASSERT_EQ(circular_buffer::cbBase::cbStatus::FULL, cbS);
  ASSERT_EQ(2, numElements);
}

TEST(circularBuffer, test_14)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {2};
  cb_t aCircularBuffer(cbsize);

  circular_buffer::cbBase::cbStatus cbS {};
  cbtype item {456};
  size_t numElements {};

  ASSERT_EQ(true, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(false, aCircularBuffer.isPopulated());

  // try to add two items to an empty circular buffer of size 2
  aCircularBuffer.add(123);
  aCircularBuffer.add(item);

  ASSERT_EQ(false, aCircularBuffer.isEmpty());
  ASSERT_EQ(true, aCircularBuffer.isFull());
  ASSERT_EQ(true, aCircularBuffer.isPopulated());

  // remove an item
  std::tie(cbS, item, numElements) = aCircularBuffer.remove();
  
  ASSERT_EQ(false, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(true, aCircularBuffer.isPopulated());
  ASSERT_EQ(circular_buffer::cbBase::cbStatus::REMOVED, cbS);
  ASSERT_EQ(cbtype {123}, item);
  ASSERT_EQ(1, numElements);

  // remove an item
  std::tie(cbS, item, numElements) = aCircularBuffer.remove();
  
  ASSERT_EQ(true, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(false, aCircularBuffer.isPopulated());
  ASSERT_EQ(circular_buffer::cbBase::cbStatus::REMOVED, cbS);
  ASSERT_EQ(cbtype {456}, item);
  ASSERT_EQ(0, numElements);
}

TEST(circularBuffer, test_15)
{
  // Size of the circular buffer used in the test
  constexpr unsigned int cbsize {3};
  cb_t aCircularBuffer(cbsize);

  circular_buffer::cbBase::cbStatus cbS {};
  cbtype item {1};
  size_t numElements {};

  ASSERT_EQ(true, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(false, aCircularBuffer.isPopulated());

  // try to add more items than capacity to an empty circular buffer
  aCircularBuffer.add(item++);
  aCircularBuffer.add(item++);
  aCircularBuffer.add(item++);
  aCircularBuffer.add(item++);
  aCircularBuffer.add(item);

  ASSERT_EQ(false, aCircularBuffer.isEmpty());
  ASSERT_EQ(true, aCircularBuffer.isFull());
  ASSERT_EQ(true, aCircularBuffer.isPopulated());
  ASSERT_EQ(cbsize, aCircularBuffer.getNumElements());

  // remove an item
  std::tie(cbS, item, numElements) = aCircularBuffer.remove();
  
  ASSERT_EQ(false, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(true, aCircularBuffer.isPopulated());
  ASSERT_EQ(circular_buffer::cbBase::cbStatus::REMOVED, cbS);
  ASSERT_EQ(cbtype {1}, item);
  ASSERT_EQ(2, numElements);

  // remove an item
  std::tie(cbS, item, numElements) = aCircularBuffer.remove();
  
  ASSERT_EQ(false, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(true, aCircularBuffer.isPopulated());
  ASSERT_EQ(circular_buffer::cbBase::cbStatus::REMOVED, cbS);
  ASSERT_EQ(cbtype {2}, item);
  ASSERT_EQ(1, numElements);

  // remove an item
  std::tie(cbS, item, numElements) = aCircularBuffer.remove();
  
  ASSERT_EQ(true, aCircularBuffer.isEmpty());
  ASSERT_EQ(false, aCircularBuffer.isFull());
  ASSERT_EQ(false, aCircularBuffer.isPopulated());
  ASSERT_EQ(circular_buffer::cbBase::cbStatus::REMOVED, cbS);
  ASSERT_EQ(cbtype {3}, item);
  ASSERT_EQ(0, numElements);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
