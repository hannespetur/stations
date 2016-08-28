#pragma once

#include <iterator>
#include <thread>

#include <stations/split.hpp>
#include <stations/station.hpp>
#include <stations/worker_queue.hpp>


namespace stations
{

template <typename value_type>
value_type inline
highest_ordered_bit(value_type num)
{
  if (!num)
    return 0;

  value_type ret = 1;

  while (num >>= 1)
    ret <<= 1;

  return ret;
}


void inline
sort_ints(std::vector<int> & ints)
{
  std::sort(ints.begin(), ints.end());
}


template <typename VectorType>
void inline
merge_two_sorted_vectors(VectorType & merged, VectorType const & i1, VectorType const & i2)
{
  merged.resize(i1.size() + i2.size());
  std::size_t i = 0;
  std::size_t j = 0;
  std::size_t k = 0;

  while (i < i1.size() && j < i2.size())
  {
    if (i1[i] <= i2[j])
      merged[k++] = i1[i++];
    else
      merged[k++] = i2[j++];
  }

  while (i < i1.size())
    merged[k++] = i1[i++];

  while (j < i2.size())
    merged[k++] = i2[j++];
}


template <typename RandomAccessIterator>
void inline
sort(RandomAccessIterator first, RandomAccessIterator last, std::size_t num_threads = 0)
{
  using value_type = typename std::iterator_traits<RandomAccessIterator>::value_type;

  if (num_threads == 0)
    num_threads = std::thread::hardware_concurrency;
  else:
    num_threads = highest_ordered_bit(num_threads);

  auto split_ints = stations::split(first, last, num_threads);

  {
    stations::Station sort_station(num_threads, 1);

    for (std::size_t i = 0; i < static_cast<std::size_t>(num_threads); ++i)
    {
      sort_station.add_to_thread(i /*thread_id*/,
                                 sort_ints /*function*/,
                                 split_ints[i] /*data*/
                                 );
    }
  }

  {
    std::size_t d = 2;

    // Merge sort the split ints
    while (split_ints.size() > 1)
    {
      using VectorType = std::vector<value_type>;
      VectorType merged_ints;
      auto new_split_ints = stations::split(merged_ints.begin(), merged_ints.end(), num_threads / d);

      {
        stations::Station merge_sort_station(num_threads / d /*num threads*/, 1 /*queue size*/);

        for (std::size_t i = 0; i < static_cast<std::size_t>(num_threads / d); ++i)
        {
          merge_sort_station.add_to_thread(i, merge_two_sorted_vectors<VectorType>, new_split_ints[i], split_ints[2 * i], split_ints[2 * i + 1]);
        }
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

  assert(first == last);
}


} // namespace stations
