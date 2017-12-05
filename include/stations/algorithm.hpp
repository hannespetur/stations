#pragma once

#include <chrono>
#include <iterator> // std::iterator_traits
#include <thread> // std::thread::hardware_concurrency

#include <stations/internal/algorithm_help_functions.hpp>

#include <stations/partition_iterator.hpp>
#include <stations/split.hpp>
#include <stations/station.hpp>
#include <stations/station_options.hpp> // stations::StationOptions
#include <stations/worker_queue.hpp>


namespace stations
{

template <typename InputIt, typename UnaryPredicate>
bool inline
all_of(StationOptions && options, InputIt first, InputIt last, UnaryPredicate f)
{
  std::atomic<bool> false_found{false}; // Needs to be atomic for thread safety
  stations::Station all_of_station(options);
  std::vector<InputIt> partition_iterators =
    stations::get_partition_iterators(first, last, options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
  {
    // If some expression have found to be false we can safely skip the rest of the work
    if (false_found)
      break;

    all_of_station.add_work([&false_found, f](InputIt first, InputIt last)
      {
        if (!std::all_of(first, last, f))
          false_found = true;
      },                       /*function*/
                            partition_iterators[i], /*first*/
                            partition_iterators[i + 1] /*last*/
                            );
  }

  all_of_station.join();
  return !false_found;
}


template <typename InputIt, typename UnaryPredicate>
bool inline
all_of(InputIt first, InputIt last, UnaryPredicate f)
{
  StationOptions options;
  options.chunk_size = 1;
  return all_of(std::move(options), first, last, f);
}


template <typename InputIt, typename UnaryPredicate>
bool inline
any_of(StationOptions && options, InputIt first, InputIt last, UnaryPredicate f)
{
  std::atomic<bool> true_found{false}; // Needs to be atomic for thread safety
  std::vector<InputIt> partition_iterators =
    stations::get_partition_iterators(first, last, options);
  stations::Station any_of_station(options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
  {
    if (true_found)
      break;

    any_of_station.add_work([&true_found, f](InputIt first, InputIt last)
      {
        if (std::any_of(first, last, f))
          true_found = true;
      } /*function*/,
                            partition_iterators[i], /*first*/
                            partition_iterators[i + 1] /*last*/
                            );
  }

  any_of_station.join();
  return true_found;
}


template <typename InputIt, typename UnaryPredicate>
bool inline
any_of(InputIt first, InputIt last, UnaryPredicate f)
{
  StationOptions options;
  options.chunk_size = 1;
  return any_of(std::move(options), first, last, f);
}


template <typename InputIt, typename T>
T inline
count(StationOptions && options, InputIt first, InputIt last, T const & value)
{
  std::vector<InputIt> partition_iterators =
    stations::get_partition_iterators(first, last, options);
  std::vector<std::shared_ptr<T> > counts;
  stations::Station count_station(options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
  {
    counts.push_back(std::make_shared<T>(0));
    count_station.add_work([value](InputIt first, InputIt last, std::shared_ptr<T> ret)
      {
        *ret = std::count(first, last, value);
      } /*function*/,
                           partition_iterators[i], /*first*/
                           partition_iterators[i + 1], /*last*/
                           counts.back()
                           );
  }

  // Merge region
  T sum = 0;
  count_station.join(); // Join all threads

  for (auto const & count : counts)
    sum += *count;

  return sum;
}


template <typename InputIt, typename T>
T inline
count(InputIt first, InputIt last, T const & value)
{
  StationOptions options;
  options.chunk_size = 0; // Partition the container evenly
  return stations::count(std::move(options), first, last, value);
}


template <typename InputIt, typename UnaryPredicate>
typename std::iterator_traits<InputIt>::difference_type inline
count_if(StationOptions && options, InputIt first, InputIt last, UnaryPredicate p)
{
  using T = typename std::iterator_traits<InputIt>::difference_type;
  std::vector<InputIt> partition_iterators =
    stations::get_partition_iterators(first, last, options);
  std::vector<std::shared_ptr<T> > counts;
  stations::Station count_if_station(options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
  {
    counts.push_back(std::make_shared<T>(0));
    count_if_station.add_work([p](InputIt first, InputIt last, std::shared_ptr<T> ret)
      {
#ifdef D_GLIBCXX_PARALLEL
        *ret = std::count_if(first, last, p, __gnu_parallel::sequential_tag());
#else
        *ret = std::count_if(first, last, p);
#endif
      } /*function*/,
                              partition_iterators[i], /*first*/
                              partition_iterators[i + 1], /*last*/
                              counts.back()
                              );
  }

  // Merge region
  T sum = 0;
  count_if_station.join();

  for (auto const & count : counts)
    sum += *count;

  return sum;
}


template <typename InputIt, typename UnaryPredicate>
typename std::iterator_traits<InputIt>::difference_type inline
count_if(InputIt first, InputIt last, UnaryPredicate p)
{
  return stations::count_if(StationOptions(), first, last, p);
}


template <typename InputIt, typename T>
void inline
fill(StationOptions && options, InputIt first, InputIt last, T const & value)
{
  std::vector<InputIt> partition_iterators =
    stations::get_partition_iterators(first, last, options);
  std::vector<std::shared_ptr<T> > counts;
  stations::Station fill_station(options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
  {
    fill_station.add_work([value](InputIt first, InputIt last)
      {
        std::fill(first, last, value);
      } /*function*/,
                          partition_iterators[i], /*first*/
                          partition_iterators[i + 1] /*last*/
                          );
  }

  fill_station.join();
}


template <typename InputIt, typename T>
void inline
fill(InputIt first, InputIt last, T const & value)
{
  StationOptions options;
  options.chunk_size = 0; // Partition evenly
  fill(std::move(options), first, last, value);
}


template <typename InputIt, typename UnaryFunction>
UnaryFunction inline
for_each(StationOptions && options, InputIt first, InputIt last, UnaryFunction f)
{
  std::vector<InputIt> partition_iterators =
    stations::get_partition_iterators(first, last, options);
  stations::Station for_each_station(options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
    for_each_station.add_work([f](InputIt it){
        f(*it);
      } /*function*/, partition_iterators[i] /*it*/);

  for_each_station.join();
  return f;
}


template <typename InputIt, typename UnaryFunction>
UnaryFunction inline
for_each(InputIt first, InputIt last, UnaryFunction f)
{
  StationOptions options;
  options.chunk_size = 1;
  return for_each(std::move(options), first, last, f);
}


template <typename InputIt, typename UnaryPredicate>
bool inline
none_of(StationOptions && options, InputIt first, InputIt last, UnaryPredicate f)
{
  return !stations::any_of(options, first, last, f);
}


template <typename InputIt, typename UnaryPredicate>
bool inline
none_of(InputIt first, InputIt last, UnaryPredicate f)
{
  return !stations::any_of(first, last, f);
}


template <typename InputIt>
void inline
sort(StationOptions && options, InputIt first, InputIt last)
{
  std::vector<InputIt> partition_iterators =
    stations::get_partition_iterators(first, last, options);

  stations::Station sort_station(options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
  {
    sort_station.add_work([](InputIt first, InputIt last){
        std::sort(first, last);
      } /*function*/,
                          partition_iterators[i], /*first*/
                          partition_iterators[i + 1] /*last*/
                          );
  }

  // After this join, all partitions are sorted, then we need to merge the sorted partition
  sort_station.join();

  // I have tried to implement a multi-threaded merge, but actually the single threaded version
  // was always faster! x(
  for (std::size_t d = 2; d < partition_iterators.size(); ++d)
  {
    std::inplace_merge(partition_iterators[0], partition_iterators[d - 1], partition_iterators[d]);
  }
}


template <typename InputIt>
void inline
sort(InputIt first, InputIt last)
{
  StationOptions options;
  std::size_t const container_size = std::distance(first, last);

  if (options.num_threads > 2 && container_size >= 1000 && container_size <= 10000)
  {
    options.set_num_threads(2);
  }
  else if (options.num_threads > 4 && container_size >= 10000 && container_size <= 100000)
  {
    options.set_num_threads(4);
  }

  stations::sort(std::move(options), first, last);
}


} // namespace stations
