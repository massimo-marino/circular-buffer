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
 * File:   circular-buffer-example.cpp
 * Author: massimo
 * 
 * Created on April 12, 2017, 8:17 PM
 */

#include "circularBuffer.h"
#include <cstdint>
#include <iostream>
#include <thread>
#include <future>
#include <exception>
#include <algorithm>
////////////////////////////////////////////////////////////////////////////////
// Producer/Consumer example
// This example runs with numerical types only: bool, char, short int, int, ...
// The type of data stored in the circular buffer
using cbtype = uint16_t;

// Alias for the circular buffer used in the example
using cb = circular_buffer::cb<cbtype>;
// Size of the circular buffer used in the example
static constexpr unsigned int CBSIZE {100};

// Number of items produced by the producer thread
static constexpr cbtype MAX_LIMIT_DEFAULT{static_cast<cbtype>(1'000'000)};
// in order to avoid overflow, take the min between the max limit default
// value and the max value for the type cbtype
static constexpr cbtype LIMIT {std::min(MAX_LIMIT_DEFAULT, std::numeric_limits<cbtype>::max())};

static void allow (const int64_t&& d = 0) noexcept
{
  std::this_thread::yield();
  std::this_thread::sleep_for(std::chrono::nanoseconds(d));
}

#ifdef DO_LOGS
static void printCBStatus (const std::string& callerFun,
                          const cb& aCircularBuffer,
                          std::mutex& printMx) noexcept
{
  std::lock_guard<std::mutex> mlg(printMx);

  std::clog << "[" << callerFun << "] "
            << "aCircularBuffer size: "
            << aCircularBuffer.size()
            << std::endl;

  std::clog << "[" << callerFun << "] "
            << "aCircularBuffer num of elements: "
            << aCircularBuffer.getNumElements()
            << std::endl;

  std::clog << "[" << callerFun << "] "
            << "aCircularBuffer is empty: "
            << (aCircularBuffer.isEmpty() ? "TRUE" : "FALSE")
            << std::endl;

  std::clog << "[" << callerFun << "] "
            << "aCircularBuffer is full: "
            << (aCircularBuffer.isFull() ? "TRUE" : "FALSE")
            << std::endl;

  std::clog << "[" << callerFun << "] "
            << "aCircularBuffer is populated: "
            << (aCircularBuffer.isPopulated() ? "TRUE" : "FALSE")
            << std::endl;

  aCircularBuffer.printData(static_cast<std::string>(callerFun));
}  // printCBStatus
#endif

static auto producerExample(cb& aCircularBuffer, std::mutex& printMx) noexcept -> cbtype
{
#ifdef DO_LOGS
  printCBStatus(__func__, aCircularBuffer, printMx);
  circular_buffer::cbBase::cbStatus cbS {};
#endif
  
  cbtype item {};
  size_t numElements {};

  while ( item < LIMIT)
  {
    if ( aCircularBuffer.isFull() )
    {
#ifdef DO_LOGS
      // buffer is full: yield the cpu
      {
        std::lock_guard<std::mutex> mlg(printMx);
        std::clog << "[" << __func__ << "] "
                  << "FULL - num of elements: "
                  << aCircularBuffer.size()
                  << std::endl;
      }
#endif
      allow(4);
    }
    else
    {
      // buffer is not full
      // generate a new item and add it to the circular buffer
      ++item;
#ifdef DO_LOGS
      std::tie(cbS, numElements) = aCircularBuffer.add(item);
      std::lock_guard<std::mutex> mlg(printMx);
      std::clog << "[" << __func__ << "] "
                << aCircularBuffer.cbStatusString(cbS)
                << " - item: "
                << item
                << " - num of elements: "
                << numElements
                << std::endl;
#else
      std::tie(std::ignore, numElements) = aCircularBuffer.add(item);
#endif
      allow(1);
    }
  }
  std::lock_guard<std::mutex> mlg(printMx);
  std::clog << "[" << __func__ << "] "
            << "TERMINATED"
            << std::endl;
  return item;
}  // producerExample

static auto consumerExample(cb& aCircularBuffer, std::mutex& printMx) noexcept -> cbtype
{
#ifdef DO_LOGS
  printCBStatus(__func__, aCircularBuffer, printMx);
  circular_buffer::cbBase::cbStatus cbS {};
#endif
  
  cbtype item {};
  size_t numElements {};

  while ( LIMIT != item )
  {
    if ( aCircularBuffer.isPopulated() )
    {
      // buffer contains some elements: remove an item from the circular buffer
#ifdef DO_LOGS
      std::tie(cbS, item, numElements) = aCircularBuffer.remove();
      std::lock_guard<std::mutex> mlg(printMx);
      std::clog << "[" << __func__ << "] "
                << aCircularBuffer.cbStatusString(cbS)
                << " - item removed: "
                << item
                << " - num of elements: "
                << numElements
                << std::endl;
#else
      std::tie(std::ignore, item, numElements) = aCircularBuffer.remove();
#endif
    }
    else if ( aCircularBuffer.isEmpty() )
    {
#ifdef DO_LOGS
      // buffer is empty: yield the cpu
      {
        std::lock_guard<std::mutex> mlg(printMx);
        std::clog << "[" << __func__ << "] "
                  << "EMPTY - num of elements: 0"
                  << std::endl;
      }
#endif
      allow(4);
    }
  }
  std::lock_guard<std::mutex> mlg(printMx);
  std::clog << "[" << __func__ << "] "
            << "TERMINATED"
            << std::endl;
  return item;
}  // consumerExample

static void consumerThreadedExample(cb& aCircularBuffer, std::mutex& printMx) noexcept
{
  consumerExample(aCircularBuffer, printMx);
}  // consumerThreadedExample

static void producerThreadedExample(cb& aCircularBuffer, std::mutex& printMx) noexcept
{
  producerExample(aCircularBuffer, printMx);
}  // producerThreadedExample

static void taskExample ()
{
  std::mutex printMx {};
  constexpr unsigned int cbsize {CBSIZE};
  cb aCircularBuffer(cbsize);

  auto num_cpus = std::thread::hardware_concurrency();
  std::clog << "[" << __func__ << "] "
            << "There are "
            << num_cpus
            << " cores"
            << std::endl;

  // tasks launched asynchronously
  std::future<cbtype> cf = std::async(std::launch::async,
                                      consumerExample,
                                      std::ref(aCircularBuffer),
                                      std::ref(printMx));
  std::future<cbtype> pf = std::async(std::launch::async,
                                      producerExample,
                                      std::ref(aCircularBuffer),
                                      std::ref(printMx));
  
  // if at least one of the background tasks is running
//  if ( cf.wait_for(std::chrono::seconds(0)) != std::future_status::deferred ||
//       pf.wait_for(std::chrono::seconds(0)) != std::future_status::deferred
//     )
//  {
//    // poll until at least one of the loops ﬁnished
//    while ( cf.wait_for(std::chrono::seconds(0)) != std::future_status::ready &&
//            pf.wait_for(std::chrono::seconds(0)) != std::future_status::ready
//          )
//    {
//      std::this_thread::yield(); // hint to reschedule to the next thread
//    }
//  }
  
  // wait for all loops to be finished and process any exception
  try
  {
    auto pr = pf.get();
    auto cr = cf.get();
    std::clog << "[" << __func__ << "] "
              << "producer result: " << pr << std::endl;
    std::clog << "[" << __func__ << "] "
              << "consumer result: " << cr << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << "EXCEPTION: "
              << e.what()
              << std::endl;
  }

  std::clog << "[" << __func__ << "] "
            << "TERMINATED"
            << std::endl;
}  // taskExample

static void threadedExample()
{
  std::mutex printMx {};
  constexpr unsigned int cbsize {CBSIZE};
  cb aCircularBuffer(cbsize);

  auto numCPUs = std::thread::hardware_concurrency();
  auto producerCPU {1};
  auto consumerCPU {1};
  if ( numCPUs > 1 )
  {
    ++consumerCPU;
  }
  std::clog << "[" << __func__ << "] "
            << "There are "
            << numCPUs
            << " cores - Using 2 cores: core "
            << producerCPU
            << " for Producer thread, core "
            << consumerCPU
            << " for Consumer thread"
            << std::endl;

  try
  {
    std::thread cthrd(consumerThreadedExample,
                      std::ref(aCircularBuffer),
                      std::ref(printMx));
    // Create a cpu_set_t object representing a set of CPUs.
    // Clear it and mark only a CPU as set.
    cpu_set_t consumer_cpuset;
    CPU_ZERO(&consumer_cpuset);
    CPU_SET(consumerCPU, &consumer_cpuset);
    int crc = pthread_setaffinity_np(cthrd.native_handle(),
                                     sizeof(cpu_set_t), &consumer_cpuset);
    if (crc != 0)
    {
      std::cerr << "Error calling pthread_setaffinity_np: " << crc << "\n";
    }

    std::thread pthrd(producerThreadedExample,
                      std::ref(aCircularBuffer),
                      std::ref(printMx));
    // Create a cpu_set_t object representing a set of CPUs.
    // Clear it and mark only a CPU as set.
    cpu_set_t producer_cpuset;
    CPU_ZERO(&producer_cpuset);
    CPU_SET(producerCPU, &producer_cpuset);
    int prc = pthread_setaffinity_np(pthrd.native_handle(),
                                     sizeof(cpu_set_t), &producer_cpuset);
    if (prc != 0)
    {
      std::cerr << "Error calling pthread_setaffinity_np: " << prc << "\n";
    }

    cthrd.join();
    pthrd.join();
  }
  catch( const std::exception& e )
  {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cerr << "EXCEPTION" << std::endl;
  }

  std::clog << "[" << __func__ << "] "
            << "TERMINATED"
            << std::endl;
}  // threadedExample

auto main() -> int
{
  // this example only deals with integral or floating points types
  static_assert( (   (false == std::is_floating_point<cbtype>::value)
                  || (false == std::numeric_limits<cbtype>::is_integer)),
                 "expected integral or floating point types");
  
  std::clog << "\n\n[" << __func__ << "] "
            << "Using a circular buffer of "
            << CBSIZE
            << " elements. Producing "
            << LIMIT
            << " elements\n"
            << std::endl;

  threadedExample();

  std::clog << "\n------------------------------------\n"
            << std::endl;

  taskExample();

  std::clog << "\n[" << __func__ << "] "
            << "TERMINATED"
            << std::endl;
}  // main