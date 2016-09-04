#pragma once

/**
THIS IS AN AUTO-GENERATED FILE. DO NOT EDIT THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING.

You can include this file to use Stations. However, if you would like to make changes, please only change to the original source files.
*/


#include <vector> // std::vector

namespace stations_internal
{

template<typename TContainer>
std::vector<int>
get_random_ints(std::size_t const N)
{
  std::vector<int> ints;
  ints.reserve(N);

  for (std::size_t i = 0; i < N; ++i)
    ints.push_back(rand() % 20000001 - 10000000); // Random number between (and including) -1,000,000 and 1,000,000

  return ints;
}

} // namespace stations_internal

#include <algorithm> // std::sort
#include <memory> // std::shared_ptr


namespace stations_internal
{

/** Returns a number with a single bit set, the bit set is the most significant bit of the input. If the input is 0, 1 is returned. Examples:
 * highest_ordered_bit(0) = 1
 * highest_ordered_bit(1) = 1
 * highest_ordered_bit(2) = 2
 * highest_ordered_bit(3) = 2
 * highest_ordered_bit(4) = 4
 * highest_ordered_bit(7) = 4
 * highest_ordered_bit(9) = 8
 */
template <typename T>
T inline
highest_ordered_bit(T num)
{
  T ret = 1;

  while (num >>= 1)
    ret <<= 1;

  return ret;
}


template <typename TContainer>
void inline
sort_ints(std::shared_ptr<TContainer> ints)
{
  std::sort(ints->begin(), ints->end());
}


template <typename TVector>
void inline
merge_two_sorted_vectors(std::shared_ptr<TVector> merged, std::shared_ptr<TVector> i1, std::shared_ptr<TVector> i2)
{
  // Make sure merged is empty initially
  merged->clear();

  // If either sorted vector is empty, the other is the merged vector.
  if (i1->size() == 0)
  {
    std::swap(*merged, *i2);
    return;
  }
  else if (i2->size() == 0)
  {
    std::swap(*merged, *i1);
    return;
  }

  merged->reserve(i1->size() + i2->size());
  std::size_t i = 0;
  std::size_t j = 0;

  while (true)
  {
    if ((*i1)[i] <= (*i2)[j])
    {
      merged->push_back((*i1)[i++]);

      if (i == i1->size())
      {
        while (j < i2->size())
          merged->push_back((*i2)[j++]);

        break;
      }
    }
    else
    {
      merged->push_back((*i2)[j++]);

      if (j == i2->size())
      {
        while (i < i1->size())
          merged->push_back((*i1)[i++]);

        break;
      }
    }
  }
}

} // namespace stations_internal

#include <atomic> // std::atomic
#include <iostream> // std::cout, std::endl
#include <thread> // std::this_thread::sleep_for
#include <list> // std::list


namespace stations
{

class WorkerQueue
{
public:
  std::list<std::function<void()> > function_queue; // Use std::list because pushing back to lists does not invalidate previous iterators
  std::list<std::function<void()> >::iterator item_to_run;
  bool finished = false;
  std::atomic<std::size_t> queue_size;
  std::size_t completed_items = 0;

  WorkerQueue()
  {
    queue_size = 0;
  }


  void inline
  add_work_to_queue(std::function<void()> work)
  {
    function_queue.push_back(work);

    if (queue_size == 0 && completed_items == 0)
      item_to_run = function_queue.begin();

    ++queue_size;
  }


  std::size_t inline
  get_number_of_items_in_queue() const
  {
    return queue_size;
  }


  std::size_t inline
  get_number_of_completed_items() const
  {
    return completed_items;
  }

  void inline
  operator()()
  {
    while (true)
    {
      if (queue_size > 0)
      {
        if (completed_items != 0)
          ++item_to_run;

        (*item_to_run)();
        ++completed_items;
        --queue_size;
      }
      else if (finished)
      {
        return;
      }
      else
      {
        std::this_thread::sleep_for(std::chrono::microseconds(100)); // 0.1 ms
      }
    }
  }


};

} // namespace stations

#include <iterator>
#include <thread>

#include <stations/internal/_algorithm.hpp>

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

#include <memory> // std::shared_ptr
#include <vector> // std::vector

namespace stations
{

template <typename TContainer>
inline
void
join(TContainer & container, std::vector<std::shared_ptr<TContainer> > & split_container)
{
  std::size_t const PARTS = split_container.size();

  for (std::size_t i = 0; i < PARTS; ++i)
  {
    std::move(split_container[i]->begin(), split_container[i]->end(), std::back_inserter(container));
    split_container[i]->clear();
  }
}


template <typename TContainer, typename Function>
inline
void
join(TContainer & map, std::vector<std::shared_ptr<TContainer> > & split_map, Function merge_fun)
{
  std::size_t const PARTS = split_map.size();

  for (auto & a_map : split_map)
  {
    for (typename TContainer::iterator it = a_map->begin(); it != a_map->end(); ++it)
      merge_fun(map, it);

    a_map->clear(); // Free memory
  }
}


} // namespace stations
#include <algorithm> // std::min_element
#include <iostream> // std::cout, std::endl
#include <thread> // std::thread

#include <stations/worker_queue.hpp> // stations::WorkerQueue

namespace stations
{

class Station
{
private:
  std::size_t const thread_count;
  std::size_t main_thread_work_count = 0;
  std::size_t const max_queue_size;
  std::vector<std::thread> workers;
  std::vector<std::unique_ptr<WorkerQueue> > queues;
  bool joined = false;


public:
  Station(std::size_t const _thread_count = 1, std::size_t const _max_queue_size = 2)
    : thread_count(_thread_count)
    , max_queue_size(_max_queue_size)
  {
    // If thread_count == 0 then thread_count == 1 is used (only boss thread will be used)
    for (long i = 0; i < static_cast<long>(thread_count) - 1; ++i)
    {
      std::unique_ptr<WorkerQueue> new_ptr(new WorkerQueue());
      queues.push_back(std::move(new_ptr));
      workers.push_back(std::thread(std::ref(*queues[i])));
    }
  }


  ~Station()
  {
    if (not joined)
      join();
  }


  template <typename TWork, typename... Args>
  void inline
  add(TWork && work, Args... args)
  {
    if (thread_count > 1)
    {
      auto min_queue_it = std::min_element(queues.begin(),
                                           queues.end(),
                                           [](std::unique_ptr<WorkerQueue> const & q1, std::unique_ptr<WorkerQueue> const & q2)
        {
          return q1->get_number_of_items_in_queue() < q2->get_number_of_items_in_queue();
        }
                                           );

      if ((*min_queue_it)->queue_size < max_queue_size)
      {
        (*min_queue_it)->add_work_to_queue(std::bind(std::forward<TWork>(work), args...));
        return;
      }
    }

    work(args...);
    ++main_thread_work_count;
  }


  template <typename TWork, typename ... Args>
  void inline
  add_to_thread(std::size_t const thread_id, TWork && work, Args... args)
  {
    if (thread_id % thread_count == thread_count - 1)
    {
      work(args...);
      ++main_thread_work_count;
    }
    else
    {
      queues[thread_id % thread_count]->add_work_to_queue(std::bind(std::forward<TWork>(work), args...));
    }
  }


  void inline
  join(bool const VERBOSE = true)
  {
    if (VERBOSE)
      std::cout << "Main thread processed " << main_thread_work_count << " chunks." << std::endl;

    for (long i = 0; i < static_cast<long>(thread_count) - 1; ++i)
    {
      queues[i]->finished = true;
      workers[i].join();

      if (VERBOSE)
        std::cout << "Thread " << (i + 1) << " processed " << queues[i]->get_number_of_completed_items() << " chunks." << std::endl;
    }

    joined = true;
  }


};

} // namespace stations
#include <iterator> // std::next, std::make_move_iterator
#include <memory> // std::shared_ptr
#include <vector> // std::vector


namespace stations
{

template <typename TContainer>
inline
std::vector<std::shared_ptr<TContainer> >
split(TContainer & container, std::size_t const PARTS)
{
  std::vector<std::shared_ptr<TContainer> > split_container;
  split_container.resize(PARTS);
  std::size_t const container_original_size = container.size();

  for (long i = static_cast<long>(PARTS) - 1; i >= 0; --i)
  {
    std::size_t const part_size = container_original_size / PARTS + (container_original_size % PARTS > static_cast<std::size_t>(i));
    split_container[i] =
      std::make_shared<TContainer>(std::make_move_iterator(std::next(container.end(), -part_size)),
                                   std::make_move_iterator(container.end())
                                   );

    // Make sure the container is smaller now
    if (container.size() > i * (container_original_size / PARTS + 1))
    {
      container.resize(i * (container_original_size / PARTS + 1));
    }
  }

  return split_container;
}


template <typename BidirectionalIterator>
inline
std::vector<std::shared_ptr<std::vector<typename BidirectionalIterator::value_type> > >
split(BidirectionalIterator first, BidirectionalIterator last, std::size_t const PARTS)
{
  using TContainer = std::vector<typename BidirectionalIterator::value_type>;
  std::vector<std::shared_ptr<TContainer> > split_container;
  split_container.resize(PARTS);
  std::size_t const container_original_size = std::distance(first, last);

  for (long i = static_cast<long>(PARTS) - 1; i >= 0; --i)
  {
    std::size_t const part_size = container_original_size / PARTS + (container_original_size % PARTS > static_cast<std::size_t>(i));
    split_container[i] =
      std::make_shared<TContainer>(std::make_move_iterator(std::next(last, -part_size)),
                                   std::make_move_iterator(last)
                                   );

    last = std::next(last, -part_size);
  }

  return split_container;
}


} // namespace stations
