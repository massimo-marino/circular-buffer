/* 
 * File:   circular-buffer-example.cpp
 * Author: massimo
 * 
 * Created on April 12, 2017, 8:17 PM
 */

#include "circularBuffer.h"
#include <future>
#include <sstream>
////////////////////////////////////////////////////////////////////////////////
// Multi-threaded Producer/Consumer example
// This example runs with numerical types only: bool, char, short int, int, ...

// The data type stored in the circular buffer
using cbtype = uint16_t;

// Alias for the circular buffer templated type used in the example
using cb = circular_buffer::cb<cbtype>;
// Size of the circular buffer used in the example
static constexpr unsigned int CBSIZE {50};

// Number of items produced by the producer thread
static constexpr cbtype MAX_LIMIT_DEFAULT {static_cast<cbtype>(20'000)};
// in order to avoid overflow, take the min between the max limit default
// value and the max value for the type cbtype
static constexpr cbtype LIMIT {std::min(MAX_LIMIT_DEFAULT, std::numeric_limits<cbtype>::max())};

static
void
allow(const int64_t& d = 0) noexcept
{
  std::this_thread::yield();
  if ( 0 == d )
  {
    return;
  }
  std::this_thread::sleep_for(std::chrono::nanoseconds(d));
}

#ifdef DO_LOGS
// helper for concurrent logging
struct pclog : public std::stringstream
{
  static inline std::mutex cout_mutex;
  ~pclog()
  {
    std::lock_guard<std::mutex> l {cout_mutex};
    std::clog << rdbuf();
  }
};

static
void
printCBStatus(std::string&& callerFun,
              const cb& aCircularBuffer) noexcept
{
  pclog{} << "[" << callerFun << "] "
          << "aCircularBuffer size: "
          << aCircularBuffer.size()
          << "\n"
          << "[" << callerFun << "] "
          << "aCircularBuffer num of elements: "
          << aCircularBuffer.getNumElements()
          << "\n"
          << "[" << callerFun << "] "
          << "aCircularBuffer is empty: "
          << std::boolalpha
          << aCircularBuffer.isEmpty()
          << "\n"
          << "[" << callerFun << "] "
          << "aCircularBuffer is full: "
          << aCircularBuffer.isFull()
          << "\n"
          << "[" << callerFun << "] "
          << "aCircularBuffer is populated: "
          << aCircularBuffer.isPopulated()
          << std::endl;

  aCircularBuffer.printData(std::forward<std::string>(callerFun));
}  // printCBStatus
#endif

static
auto
producerExample(cb& aCircularBuffer) -> cbtype
{
#ifdef DO_LOGS
  circular_buffer::cbBase::cbStatus previousCBS {circular_buffer::cbBase::cbStatus::UNKNOWN};

  printCBStatus(__func__, aCircularBuffer);
#endif

  circular_buffer::cbBase::cbStatus cbS {circular_buffer::cbBase::cbStatus::UNKNOWN};
  cbtype item {0};
  size_t numElements {};

  while ( item <= LIMIT )
  {
    // try to add an item; get the result and the number of items in the circular
    // buffer after the action
    std::tie(cbS, numElements) = aCircularBuffer.add(item);

    switch (cbS)
    {
      case circular_buffer::cbBase::cbStatus::ADDED:
      {
#ifdef DO_LOGS
        pclog{} << "[" << __func__ << "] "
                << aCircularBuffer.cbStatusString(cbS)
                << " - item: "
                << item
                << " - num of elements: "
                << numElements
                << std::endl;
        previousCBS = cbS;
#endif
        ++item;
        allow(0);
      }
      break;
      
      case circular_buffer::cbBase::cbStatus::FULL:
      {
#ifdef DO_LOGS
        if ( cbS != previousCBS )
        {
          pclog{} << "[" << __func__ << "] "
                  << aCircularBuffer.cbStatusString(cbS)
                  << " - num of elements: "
                  << numElements
                  << std::endl;
          previousCBS = cbS;
        }
#endif
        allow(3);
      }
      break;
    }
  }

  pclog{} << "[" << __func__ << "] "
          << "TERMINATED"
          << std::endl;
  return (item - 1);
}  // producerExample

static
auto
consumerExample(cb& aCircularBuffer) -> cbtype
{
#ifdef DO_LOGS
  circular_buffer::cbBase::cbStatus previousCBS {circular_buffer::cbBase::cbStatus::UNKNOWN};
#endif

  circular_buffer::cbBase::cbStatus cbS {circular_buffer::cbBase::cbStatus::UNKNOWN};
  cbtype item {};
  size_t numElements {};
  
  while ( item != LIMIT )
  {
    // try to remove an item; get the result, the item, and the number of items
    // in the circular buffer after the action
    std::tie(cbS, item, numElements) = aCircularBuffer.remove();

    switch (cbS)
    {
      case circular_buffer::cbBase::cbStatus::REMOVED:
      {
#ifdef DO_LOGS
        pclog{} << "[" << __func__ << "] "
                << aCircularBuffer.cbStatusString(cbS)
                << " - item removed: "
                << item
                << " - num of elements: "
                << numElements
                << std::endl;
        previousCBS = cbS;
#endif
        allow(0);
      }
      break;

      case circular_buffer::cbBase::cbStatus::EMPTY:
      {
#ifdef DO_LOGS
        if ( cbS != previousCBS )
        {
          pclog{} << "[" << __func__ << "] "
                  << aCircularBuffer.cbStatusString(cbS)
                  << " - num of elements: "
                  << numElements
                  << std::endl;
          previousCBS = cbS;
        }
#endif
        allow(5);
      }
      break;
    }
  }

  pclog{} << "[" << __func__ << "] "
          << "TERMINATED"
          << std::endl;
  return item;
}  // consumerExample

static
void
consumerThreadedExample(cb& aCircularBuffer)
{
  consumerExample(aCircularBuffer);
}  // consumerThreadedExample

static
void
producerThreadedExample(cb& aCircularBuffer)
{
  producerExample(aCircularBuffer);
}  // producerThreadedExample

static
void
taskExample () noexcept
{
  constexpr unsigned int cbsize {CBSIZE};
  cb aCircularBuffer(cbsize);

  auto num_cpus = std::thread::hardware_concurrency();
  pclog{} << "[" << __func__ << "] "
          << "There are "
          << num_cpus
          << " cores"
          << std::endl;

  // tasks launched asynchronously
  std::future<cbtype> cf = std::async(std::launch::async,
                                      consumerExample,
                                      std::ref(aCircularBuffer));
  std::future<cbtype> pf = std::async(std::launch::async,
                                      producerExample,
                                      std::ref(aCircularBuffer));

  // wait for all loops to be finished and process any exception
  try
  {
    auto pr = pf.get();
    auto cr = cf.get();
    pclog{} << "[" << __func__ << "] "
            << "producer result: " << pr << std::endl;
    pclog{} << "[" << __func__ << "] "
            << "consumer result: " << cr << std::endl;
    
    aCircularBuffer.printData(__func__);
  }
  catch( const std::exception& e )
  {
    pclog{} << "EXCEPTION: "
            << e.what()
            << std::endl;
  }

  pclog{} << "[" << __func__ << "] "
          << "TERMINATED"
          << std::endl;
}  // taskExample

static
void
threadedExample() noexcept
{
  constexpr unsigned int cbsize {CBSIZE};
  cb aCircularBuffer(cbsize);

  auto numCPUs {std::thread::hardware_concurrency()};
  auto producerCPU {1};
  auto consumerCPU {1};
  if ( numCPUs > 1 )
  {
    ++consumerCPU;
  }
  pclog{} << "[" << __func__ << "] "
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
                      std::ref(aCircularBuffer));
    // Create a cpu_set_t object representing a set of CPUs.
    // Clear it and mark only a CPU as set.
    cpu_set_t consumer_cpuset;
    CPU_ZERO(&consumer_cpuset);
    CPU_SET(consumerCPU, &consumer_cpuset);
    int crc {pthread_setaffinity_np(cthrd.native_handle(),
                                    sizeof(cpu_set_t), &consumer_cpuset)};
    if ( 0 != crc )
    {
      pclog{} << "Error calling pthread_setaffinity_np: " << crc << "\n";
    }

    std::thread pthrd(producerThreadedExample,
                      std::ref(aCircularBuffer));
    // Create a cpu_set_t object representing a set of CPUs.
    // Clear it and mark only a CPU as set.
    cpu_set_t producer_cpuset;
    CPU_ZERO(&producer_cpuset);
    CPU_SET(producerCPU, &producer_cpuset);
    int prc {pthread_setaffinity_np(pthrd.native_handle(),
                                    sizeof(cpu_set_t), &producer_cpuset)};
    if ( 0 != prc )
    {
      pclog{} << "Error calling pthread_setaffinity_np: " << prc << "\n";
    }

    cthrd.join();
    pthrd.join();
    aCircularBuffer.printData(__func__);
  }
  catch( const std::exception& e )
  {
    pclog{} << "EXCEPTION: " << e.what() << std::endl;
  }
  catch( ... )
  {
    pclog{} << "EXCEPTION" << std::endl;
  }

  pclog{} << "[" << __func__ << "] "
          << "TERMINATED"
          << std::endl;
}  // threadedExample

auto
main() -> int
{
  // this example only deals with integral or floating points types
  static_assert( (   (false == std::is_floating_point<cbtype>::value)
                  || (false == std::numeric_limits<cbtype>::is_integer)),
                 "expected integral or floating point types");
  
  pclog{} << "\n\n[" << __func__ << "] "
          << "Using a circular buffer of "
          << CBSIZE
          << " elements. Producing "
          << LIMIT
          << " elements\n"
          << std::endl;

  threadedExample();

  pclog{} << "\n------------------------------------\n"
          << std::endl;

  taskExample();

  pclog{} << "\n[" << __func__ << "] "
          << "TERMINATED"
          << std::endl;
}  // main
