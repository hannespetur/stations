#pragma once

#include <iterator> // std::iterator_traits
#include <thread> // std::thread::hardware_concurrency

#include <stations/internal/algorithm_help_functions.hpp>

#include <stations/split.hpp>
#include <stations/split_iterator.hpp>
#include <stations/station.hpp>
#include <stations/worker_queue.hpp>


namespace stations
{

template<typename InputIt, typename T>
T
count(InputIt first, InputIt last, T const & value)
{
  std::size_t num_threads = std::thread::hardware_concurrency();
  std::vector<InputIt> split_iterators = stations::split_iterators(first, last, num_threads);
  std::vector<std::shared_ptr<T> > counts;

  // Parallel region
  {
    stations::Station count_station(num_threads, 1);

    for (std::size_t i = 0; i < num_threads; ++i)
    {
      counts.push_back(std::make_shared<T>(0));
      count_station.add_to_thread(i /*thread_id*/,
                                 [value](InputIt first, InputIt last, std::shared_ptr<T> ret)
                                 {
                                   *ret = std::count(first, last, value);
                                 } /*function*/,
                                 split_iterators[i], /*first*/
                                 split_iterators[i+1], /*last*/
                                 counts.back()
                                 );
    }
  }

  // Merge region
  T sum = 0;

  for (auto const & count : counts)
    sum += *count;

  return sum;
}


template<typename InputIt, typename UnaryPredicate>
typename std::iterator_traits<InputIt>::difference_type
count_if(InputIt first, InputIt last, UnaryPredicate p)
{
  using T = typename std::iterator_traits<InputIt>::difference_type;

  std::size_t num_threads = std::thread::hardware_concurrency();
  std::vector<InputIt> split_iterators = stations::split_iterators(first, last, num_threads);
  std::vector<std::shared_ptr<T> > counts;

  // Parallel region
  {
    stations::Station count_if_station(num_threads, 1);

    for (std::size_t i = 0; i < num_threads; ++i)
    {
      counts.push_back(std::make_shared<T>(0));
      count_if_station.add_to_thread(i /*thread_id*/,
                                     [p](InputIt first, InputIt last, std::shared_ptr<T> ret)
                                     {
                                       *ret = std::count_if(first, last, p);
                                     } /*function*/,
                                     split_iterators[i], /*first*/
                                     split_iterators[i+1], /*last*/
                                     counts.back()
                                     );
    }
  }

  // Merge region
  T sum = 0;

  for (auto const & count : counts)
    sum += *count;

  return sum;
}


template<typename InputIt, typename UnaryFunction>
UnaryFunction
for_each(InputIt first, InputIt last, UnaryFunction f)
{
  std::size_t num_threads = std::thread::hardware_concurrency();

  // Parallel region
  {
    stations::Station for_each_station(num_threads, 1);

    while (first != last)
    {
      for_each_station.add([f](InputIt it)
                           {
                             f(*it);
                           } /*function*/,
                           first++ /*it*/
                           );
    }
  }

  return f;
}


template <typename TIter>
void inline
sort(TIter first, TIter last)
{
  using TValue = typename std::iterator_traits<TIter>::value_type;
  using TVector = std::vector<TValue>;
  std::size_t num_threads = std::thread::hardware_concurrency();

  if (num_threads == 0)
    num_threads = 1;
  else
    num_threads = stations_internal::highest_ordered_bit(num_threads);

  std::vector<std::shared_ptr<TVector> > split_ints = stations::split(first, last, num_threads);

  {
    stations::Station sort_station(num_threads, 1);

    for (std::size_t i = 0; i < static_cast<std::size_t>(num_threads); ++i)
    {
      sort_station.add_to_thread(i /*thread_id*/,
                                 [](std::shared_ptr<TVector> ints) {std::sort(ints->begin(), ints->end());} /*function*/,
                                 split_ints[i] /*data*/
                                 );
    }
  }

  {
    std::size_t d = 2;

    // Merge sort the split ints
    while (split_ints.size() > 1)
    {
      TVector merged_ints;
      auto new_split_ints = stations::split(merged_ints.begin(), merged_ints.end(), num_threads / d);

      {
        stations::Station merge_sort_station(num_threads / d /*num threads*/, 1 /*queue size*/);

        for (std::size_t i = 0; i < static_cast<std::size_t>(num_threads / d); ++i)
          merge_sort_station.add_to_thread(i, stations_internal::merge_two_sorted_vectors<TVector>, new_split_ints[i], split_ints[2 * i], split_ints[2 * i + 1]);
      }

      split_ints = new_split_ints;
      d *= 2;
    }
  }

  for (std::size_t i = 0; i < split_ints[0]->size(); ++i)
  {
    *first = (*split_ints[0])[i];
    ++first;
  }
}


} // namespace stations
