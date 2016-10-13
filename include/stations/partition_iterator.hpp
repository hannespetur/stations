#pragma once

#include <iterator> // std::next
#include <vector> // std::vector
#include <iostream>

#include <stations/station_options.hpp> // stations::StationOptions


namespace stations
{

template <typename BidirectionalIterator> inline
std::vector<BidirectionalIterator>
get_partition_iterators(BidirectionalIterator first, BidirectionalIterator last, StationOptions const & options)
{
  std::vector<BidirectionalIterator> partition_iterators;

  if (options.chunk_size == 0)
  {
    std::size_t const PARTS = options.get_num_threads() - (options.get_num_threads() > 1 && options.boss_thread_mode == PATIENT_BOSS);
    partition_iterators.reserve(PARTS + 1);
    partition_iterators.push_back(first);
    std::size_t const container_original_size = std::distance(first, last);

    for (std::size_t i = 0; i < PARTS - 1; ++i)
    {
      std::size_t const part_size = container_original_size / PARTS + (i < container_original_size % PARTS);
      partition_iterators.push_back(std::next(partition_iterators.back(), part_size));
    }
  }
  else
  {
    partition_iterators.reserve(std::distance(first, last) / options.chunk_size + options.chunk_size + 1);

    for (std::size_t i = 0; first != last; ++i, ++first)
    {
      if (i % options.chunk_size == 0)
        partition_iterators.push_back(first);
    }
  }

  partition_iterators.push_back(last);
  return partition_iterators;
}


template <typename BidirectionalIterator> inline
std::vector<BidirectionalIterator>
get_partition_iterators(BidirectionalIterator first, BidirectionalIterator last, std::size_t const num_threads)
{
  StationOptions options;
  options.set_num_threads(num_threads);
  return get_partition_iterators(first, last, options);
}

} // namespace stations
