/* 
 * File:   circular-buffer-example.cpp
 * Author: massimo
 * 
 * Created on April 12, 2017, 8:17 PM
 */
#include "../circularBuffer.h"
#include <future>
#include <sstream>
#include <chrono>
#include <thread>
////////////////////////////////////////////////////////////////////////////////
// Multi-threaded Producer/Consumer example
// This example runs with numerical types only: bool, char, short int, int, ...

// The data type stored in the circular buffer
using cbtype = uint32_t;

// Alias for the circular buffer templated type used in the example
using cb_t = circular_buffer::cb<cbtype>;
using cb_shptr_t = std::shared_ptr<cb_t>;
// Size of the circular buffer used in the example
static constexpr unsigned int CBSIZE {50};

// Number of items produced by the producer thread
static constexpr cbtype MAX_LIMIT_DEFAULT {static_cast<cbtype>(200'000)};
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
              const cb_t& aCircularBuffer) noexcept
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
          << "\n\n";

  aCircularBuffer.printData(std::forward<std::string>(callerFun));
}  // printCBStatus
#endif

static
auto
producerExample(const cb_shptr_t& cb_shptr) -> cbtype
{
#ifdef DO_LOGS
  circular_buffer::cbBase::cbStatus previousCBS {circular_buffer::cbBase::cbStatus::UNKNOWN};

  printCBStatus(__func__, *cb_shptr);
#endif

  circular_buffer::cbBase::cbStatus cbS {circular_buffer::cbBase::cbStatus::UNKNOWN};
  cbtype item {0};
  size_t numElements {0};

  while ( item <= LIMIT )
  {
    // try to add an item; get the result and the number of items in the circular
    // buffer after the action
    std::tie(cbS, numElements) = cb_shptr->add(item);

    switch (cbS)
    {
      case circular_buffer::cbBase::cbStatus::ADDED:
      {
#ifdef DO_LOGS
        pclog{} << "[" << __func__ << "] "
                << cb_shptr->cbStatusString(cbS)
                << " - item: "
                << item
                << " - num of elements: "
                << numElements
                << "\n";
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
                  << cb_shptr->cbStatusString(cbS)
                  << " - num of elements: "
                  << numElements
                  << "\n";
          previousCBS = cbS;
        }
#endif
        allow(3);
      }
      break;
    }
  }
#ifdef DO_LOGS
  pclog{} << "[" << __func__ << "] "
          << "TERMINATED\n";
#endif
  return (item - static_cast<cbtype>(1));
}  // producerExample

static
auto
consumerExample(const cb_shptr_t& cb_shptr) -> cbtype
{
#ifdef DO_LOGS
  circular_buffer::cbBase::cbStatus previousCBS {circular_buffer::cbBase::cbStatus::UNKNOWN};
#endif

  circular_buffer::cbBase::cbStatus cbS {circular_buffer::cbBase::cbStatus::UNKNOWN};
  cbtype item {0};
  size_t numElements {0};
  
  while ( item != LIMIT )
  {
    // try to remove an item; get the result, the item, and the number of items
    // in the circular buffer after the action
    std::tie(cbS, item, numElements) = cb_shptr->remove();

    switch (cbS)
    {
      case circular_buffer::cbBase::cbStatus::REMOVED:
      {
#ifdef DO_LOGS
        pclog{} << "[" << __func__ << "] "
                << cb_shptr->cbStatusString(cbS)
                << " - item removed: "
                << item
                << " - num of elements: "
                << numElements
                << "\n";
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
                  << cb_shptr->cbStatusString(cbS)
                  << " - num of elements: "
                  << numElements
                  << "\n";
          previousCBS = cbS;
        }
#endif
        allow(5);
      }
      break;
    }
  }
#ifdef DO_LOGS
  pclog{} << "[" << __func__ << "] "
          << "TERMINATED\n";
#endif
  return item;
}  // consumerExample

static
void
consumerThreadedExample(const cb_shptr_t& cb_shptr)
{
  consumerExample(cb_shptr);
}  // consumerThreadedExample

static
void
producerThreadedExample(const cb_shptr_t& cb_shptr)
{
  producerExample(cb_shptr);
}  // producerThreadedExample

static
void
taskExample () noexcept
{
  std::cout << "[" << __func__ << "] STARTING\n\n";

  constexpr unsigned int cbsize {CBSIZE};
  cb_shptr_t aCircularBuffer_sp = std::make_shared<cb_t>(cbsize);

  auto num_cpus = std::thread::hardware_concurrency();
#ifdef DO_LOGS
  pclog{} << "[" << __func__ << "] "
          << "There are "
          << num_cpus
          << " cores\n";
#endif

  // tasks launched asynchronously
  std::future<cbtype> cf = std::async(std::launch::async,
                                      consumerExample,
                                      aCircularBuffer_sp);
  std::future<cbtype> pf = std::async(std::launch::async,
                                      producerExample,
                                      aCircularBuffer_sp);

  // wait for all loops to be finished and process any exception
  try
  {
    auto pr = pf.get();
    auto cr = cf.get();
#ifdef DO_LOGS
    pclog{} << "[" << __func__ << "] "
            << "producer result: " << pr << "\n";
    pclog{} << "[" << __func__ << "] "
            << "consumer result: " << cr << "\n";
#endif
  }
  catch( const std::exception& e )
  {
#ifdef DO_LOGS
    pclog{} << "EXCEPTION: "
            << e.what()
            << "\n";
#endif
  }

#ifdef DO_LOGS
  printCBStatus(__func__, *aCircularBuffer_sp);
#else
  aCircularBuffer_sp->printData(__func__);
#endif

  std::cout << "[" << __func__ << "] "
            << "TERMINATED\n";
}  // taskExample

static
void
threadedExample() noexcept
{
  std::cout << "[" << __func__ << "] STARTING\n\n";

  constexpr unsigned int cbsize {CBSIZE};
  cb_shptr_t aCircularBuffer_sp = std::make_shared<cb_t>(cbsize);

  auto numCPUs {std::thread::hardware_concurrency()};
  auto producerCPU {1};
  auto consumerCPU {1};
  if ( numCPUs > 1 )
  {
    ++consumerCPU;
  }

  std::cout << "[" << __func__ << "] "
            << "There are "
            << numCPUs
            << " cores - Using 2 cores: core "
            << producerCPU
            << " for Producer thread, core "
            << consumerCPU
            << " for Consumer thread\n\n";

  try
  {
    std::thread cthrd(consumerThreadedExample, aCircularBuffer_sp);
    // Create a cpu_set_t object representing a set of CPUs.
    // Clear it and mark only a CPU as set.
    cpu_set_t consumer_cpuset;
    CPU_ZERO(&consumer_cpuset);
    CPU_SET(consumerCPU, &consumer_cpuset);
    int crc {pthread_setaffinity_np(cthrd.native_handle(),
                                    sizeof(cpu_set_t), &consumer_cpuset)};
    if ( 0 != crc )
    {
#ifdef DO_LOGS
      pclog{} << "Error calling pthread_setaffinity_np: " << crc << "\n";
#endif
    }

    std::thread pthrd(producerThreadedExample, aCircularBuffer_sp);
    // Create a cpu_set_t object representing a set of CPUs.
    // Clear it and mark only a CPU as set.
    cpu_set_t producer_cpuset;
    CPU_ZERO(&producer_cpuset);
    CPU_SET(producerCPU, &producer_cpuset);
    int prc {pthread_setaffinity_np(pthrd.native_handle(),
                                    sizeof(cpu_set_t), &producer_cpuset)};
    if ( 0 != prc )
    {
#ifdef DO_LOGS
      pclog{} << "Error calling pthread_setaffinity_np: " << prc << "\n";
#endif
    }

    cthrd.join();
    pthrd.join();
  }
  catch( const std::exception& e )
  {
#ifdef DO_LOGS
    pclog{} << "EXCEPTION: " << e.what() << "\n";
#endif
  }
  catch( ... )
  {
#ifdef DO_LOGS
    pclog{} << "EXCEPTION\n";
#endif
  }

#ifdef DO_LOGS
  printCBStatus(__func__, *aCircularBuffer_sp);
#else
  aCircularBuffer_sp->printData(__func__);
#endif
  std::cout << "[" << __func__ << "] "
            << "TERMINATED\n";
}  // threadedExample

auto
main() -> int
{
  std::cout << "\n[" << __func__ << "] STARTING\n";

  // this example only deals with integral or floating points types
  static_assert( (   (false == std::is_floating_point<cbtype>::value)
                  || (false == std::is_integral<cbtype>::value)),
                 "expected integral or floating point types");

  std::cout << "\n[" << __func__ << "] "
            << "Using a circular buffer of "
            << CBSIZE
            << " elements. Producing "
            << LIMIT
            << " elements\n\n";

  threadedExample();

  std::cout << "\n------------------------------------\n\n";

  taskExample();

  std::cout << "\n[" << __func__ << "] "
            << "TERMINATED\n\n";
}  // main

