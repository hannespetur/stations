#pragma once

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

template<typename InputIt, typename UnaryPredicate>
bool inline
all_of(StationOptions && options, InputIt first, InputIt last, UnaryPredicate f)
{
  std::atomic<bool> false_found{false}; // Needs to be atomic for thread safety
  stations::Station all_of_station(options);
  std::vector<InputIt> partition_iterators = stations::get_partition_iterators(first, last, options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
  {
    // If some expression have found to be false we can safely skip the rest of the work
    if (false_found)
      break;

    all_of_station.add_work([&false_found, f](InputIt first, InputIt last)
                            {
                              if (!std::all_of(first, last, f))
                                false_found = true;
                            }, /*function*/
                            partition_iterators[i], /*first*/
                            partition_iterators[i+1] /*last*/
                            );
  }

  all_of_station.join();
  return !false_found;
}


template<typename InputIt, typename UnaryPredicate>
bool inline
all_of(InputIt first, InputIt last, UnaryPredicate f)
{
  StationOptions options;
  options.chunk_size = 1;
  return all_of(std::move(options), first, last, f);
}


template<typename InputIt, typename UnaryPredicate>
bool inline
any_of(StationOptions && options, InputIt first, InputIt last, UnaryPredicate f)
{
  std::atomic<bool> true_found{false}; // Needs to be atomic for thread safety
  std::vector<InputIt> partition_iterators = stations::get_partition_iterators(first, last, options);
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
                            partition_iterators[i+1] /*last*/
                            );
  }

  any_of_station.join();
  return true_found;
}


template<typename InputIt, typename UnaryPredicate>
bool inline
any_of(InputIt first, InputIt last, UnaryPredicate f)
{
  StationOptions options;
  options.chunk_size = 1;
  return any_of(std::move(options), first, last, f);
}


template<typename InputIt, typename T>
T inline
count(StationOptions && options, InputIt first, InputIt last, T const & value)
{
  std::vector<InputIt> partition_iterators = stations::get_partition_iterators(first, last, options);
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
                           partition_iterators[i+1], /*last*/
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


template<typename InputIt, typename T>
T inline
count(InputIt first, InputIt last, T const & value)
{
  StationOptions options;
  options.chunk_size = 0; // Partition the container evenly
  return stations::count(std::move(options), first, last, value);
}


template<typename InputIt, typename UnaryPredicate>
typename std::iterator_traits<InputIt>::difference_type inline
count_if(StationOptions && options, InputIt first, InputIt last, UnaryPredicate p)
{
  using T = typename std::iterator_traits<InputIt>::difference_type;
  std::vector<InputIt> partition_iterators = stations::get_partition_iterators(first, last, options);
  std::vector<std::shared_ptr<T> > counts;
  stations::Station count_if_station(options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
  {
    counts.push_back(std::make_shared<T>(0));
    count_if_station.add_work([p](InputIt first, InputIt last, std::shared_ptr<T> ret)
                              {
                                *ret = std::count_if(first, last, p);
                              } /*function*/,
                              partition_iterators[i], /*first*/
                              partition_iterators[i+1], /*last*/
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


template<typename InputIt, typename UnaryPredicate>
typename std::iterator_traits<InputIt>::difference_type inline
count_if(InputIt first, InputIt last, UnaryPredicate p)
{
  StationOptions options;
  options.chunk_size = 0; // Partition evenly
  return count_if(std::move(options), first, last, p);
}


template<typename InputIt, typename T>
void inline
fill(StationOptions && options, InputIt first, InputIt last, T const & value)
{
  std::vector<InputIt> partition_iterators = stations::get_partition_iterators(first, last, options);
  std::vector<std::shared_ptr<T> > counts;
  stations::Station fill_station(options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
  {
    fill_station.add_work([value](InputIt first, InputIt last)
                          {
                            std::fill(first, last, value);
                          } /*function*/,
                          partition_iterators[i], /*first*/
                          partition_iterators[i+1] /*last*/
                          );
  }

  fill_station.join();
}


template<typename InputIt, typename T>
void inline
fill(InputIt first, InputIt last, T const & value)
{
  StationOptions options;
  options.chunk_size = 0; // Partition evenly
  fill(std::move(options), first, last, value);
}


template<typename InputIt, typename UnaryFunction>
UnaryFunction inline
for_each(StationOptions && options, InputIt first, InputIt last, UnaryFunction f)
{
  std::vector<InputIt> partition_iterators = stations::get_partition_iterators(first, last, options);
  stations::Station for_each_station(options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
    for_each_station.add_work([f](InputIt it){f(*it);} /*function*/, partition_iterators[i] /*it*/);

  for_each_station.join();
  return f;
}


template<typename InputIt, typename UnaryFunction>
UnaryFunction inline
for_each(InputIt first, InputIt last, UnaryFunction f)
{
  StationOptions options;
  options.chunk_size = 1;
  return for_each(std::move(options), first, last, f);
}


template<typename InputIt, typename UnaryPredicate>
bool inline
none_of(StationOptions && options, InputIt first, InputIt last, UnaryPredicate f)
{
  return !stations::any_of(options, first, last, f);
}


template<typename InputIt, typename UnaryPredicate>
bool inline
none_of(InputIt first, InputIt last, UnaryPredicate f)
{
  return !stations::any_of(first, last, f);
}


template <typename InputIt>
void inline
sort(InputIt first, InputIt last)
{
  using TValue = typename std::iterator_traits<InputIt>::value_type;
  using TVector = std::vector<TValue>;

  StationOptions options;
  std::vector<InputIt> partition_iterators = stations::get_partition_iterators(first, last, options);
  stations::Station sort_station(options);

  for (long i = 0; i < static_cast<long>(partition_iterators.size()) - 1; ++i)
  {
    sort_station.add_work([](InputIt first, InputIt last){std::sort(first, last);} /*function*/,
                          partition_iterators[i], /*first*/
                          partition_iterators[i+1] /*last*/
                          );
  }

  sort_station.join(); // After this join, all partitions are sorted, then we need to merge the sorted partition

  auto merge_fun = [](InputIt first, InputIt middle, InputIt last)
  {
    TVector merged_vector(std::distance(first, last));
    std::merge(first, middle, middle, last, merged_vector.begin());
    std::move(merged_vector.begin(), merged_vector.end(), first);
  };

  std::size_t d = 2;

  while(d <= options.get_num_threads())
  {
    {
      auto new_first = first;
      while (new_first != last)
      {
        std::cout << *new_first << std::endl;
        ++new_first;
      }
      std::cout << std::endl;
    }

    {
      stations::Station merge_station(options);



      std::size_t i;

      for (i = 0; static_cast<long>(i*d) < static_cast<long>(partition_iterators.size() - d); ++i)
      {
        merge_station.add_work(merge_fun,
                               partition_iterators[i*d],
                               partition_iterators[i*d + d / 2],
                               partition_iterators[i*d + d]
                               );
      }

      merge_station.join();
    }

    d *= 2;
  }

  merge_fun(partition_iterators[0], partition_iterators[d / 2], partition_iterators.back());

  // auto new_first = first;
  //
  // while (new_first != last)
  // {
  //   std::cout << *new_first << std::endl;
  //   ++new_first;
  // }
  // std::cout << std::endl;

  // {
  //   std::size_t d = 2;
  //
  //   // Merge sort the split ints
  //   while (split_ints.size() > 1)
  //   {
  //     TVector merged_ints;
  //     auto new_split_ints = stations::split(merged_ints.begin(), merged_ints.end(), num_threads / d);
  //
  //     {
  //       stations::StationOptions options;
  //       options.set_num_threads(num_threads / d);
  //       stations::Station merge_sort_station(options);
  //
  //       for (std::size_t i = 0; i < static_cast<std::size_t>(num_threads / d); ++i)
  //         merge_sort_station.add_work(stations_internal::merge_two_sorted_vectors<TVector>, new_split_ints[i], split_ints[2 * i], split_ints[2 * i + 1]);
  //     }
  //
  //     split_ints = new_split_ints;
  //     d *= 2;
  //   }
  // }
  //
  // for (std::size_t i = 0; i < split_ints[0]->size(); ++i)
  // {
  //   *first = (*split_ints[0])[i];
  //   ++first;
  // }
}


} // namespace stations
