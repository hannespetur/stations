#pragma once

#include <iterator> // std::iterator_traits
#include <thread> // std::thread::hardware_concurrency

#include <stations/internal/algorithm_help_functions.hpp>

#include <stations/split.hpp>
#include <stations/station.hpp>
#include <stations/worker_queue.hpp>


namespace stations
{

template <typename TIter>
void inline
sort(std::size_t num_threads, TIter first, TIter last)
{
  using TValue = typename std::iterator_traits<TIter>::value_type;
  using TVector = std::vector<TValue>;

  if (num_threads == 0)
  {
    num_threads = std::thread::hardware_concurrency(); // If the value is not well defined or not computable, returns ​0​. If this happens, use 1 thread by default.

    if (num_threads == 0)
      num_threads = 1;
  }
  else
  {
    num_threads = stations_internal::highest_ordered_bit(num_threads);
  }

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
