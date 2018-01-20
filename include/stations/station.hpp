#pragma once
#include <algorithm> // std::all_of, std::min_element
#include <iostream> // std::cout
#include <thread> // std::thread
#include <utility> // std::forward

#include <stations/station_options.hpp> // stations::WorkerQueue
#include <stations/partition_iterator.hpp> // stations::get_partition_iterators
#include <stations/worker_queue.hpp> // stations::WorkerQueue


namespace stations
{

class Station
{
public:
  /* aliases */
  using Queues = std::vector<std::unique_ptr<WorkerQueue> >;

private:
  /*********************
  * STATION STATISTICS *
  *********************/
  bool joined = false;
  std::size_t main_thread_work_count = 0; /** Number of jobs the main thread has run. */
  std::vector<std::thread> workers; /** List of threads. */
  Queues queues; /** Each thread has a unique worker queue. */

public:
  StationOptions options; /** Options and policies this station will follow */

  Station(StationOptions _options);
  Station(std::size_t const num_threads = 1, std::size_t const max_queue_size = 2);
  ~Station();

  /***************************
  * GENERAL MEMBER FUNCTIONS *
  ***************************/
  Queues::const_iterator find_smallest_queue(std::size_t & smallest_size);

  template <typename TWork, typename ... Args>
  void inline
  add_work(TWork && work, Args ... args)
  {
    if (workers.size() == 0)
    {
      work(args ...);
      ++main_thread_work_count;
    }
    else
    {
      // We have some workers, so let's assign the work to the smallest worker queue
      std::size_t smallest_size = -1;

      // If we have any worker threads, check who has the smallest queue
      auto min_queue_it = find_smallest_queue(smallest_size);

      if (smallest_size < options.max_queue_size)
      {
        (*min_queue_it)->add_work_to_queue([&work, args ...] {work(args ...);});
      }
      else
      {
        work(args ...); // If all queues are of maximum size, use the boss thread instead
        ++main_thread_work_count;
      }
    }
  }


  template <typename TWork, typename ... Args>
  add(TWork && work, Args ... args)
  {
    // For backwards compability
    this->add_work(std::forward<TWork>(work), args ...);
  }


  template <typename TWork, typename ... Args>
  void inline
  add_to_thread(std::size_t const thread_id, TWork && work, Args ... args)
  {
    std::size_t const thread_count = options.num_threads;

    if (thread_id % thread_count == thread_count - 1)
    {
      work(args ...);
      ++main_thread_work_count;
    }
    else
    {
      queues[thread_id % thread_count]->add_work_to_queue([&work, args ...] {work(args ...);});
    }
  }


  void inline
  join()
  {
    if (options.verbosity >= 2)
    {
      std::cout << "[stations] Main thread processed "
                << main_thread_work_count
                << " chunks.\n";
    }

    for (int i = 0; i < static_cast<int>(options.num_threads) - 1; ++i)
    {
      queues[i]->finished = true;
      workers[i].join();

      if (options.verbosity >= 2)
      {
        std::cout << "[stations] Thread " << (i + 1)
                  << " processed "
                  << queues[i]->get_number_of_completed_items()
                  << " chunks.\n";
      }
    }

    joined = true;
  }


  /***************************
  * PRIVATE MEMBER FUNCTIONS *
  ****************************/
private:
  void inline
  resize_queues_and_workers(std::size_t const new_size)
  {
    for (std::size_t i = 0; i < new_size; ++i)
    {
      queues.push_back(std::unique_ptr<WorkerQueue>(new WorkerQueue()));
      workers.push_back(std::thread(std::ref(*queues[i])));
    }
  }


};


} // namespace stations


/* IMPLEMENTATION */


namespace stations
{


inline
Station::Station(StationOptions _options)
  : options(_options)
{
  resize_queues_and_workers(options.num_threads - 1);
}


inline
Station::Station(std::size_t const num_threads, std::size_t const max_queue_size)
{
  options.num_threads = num_threads;
  options.max_queue_size = max_queue_size;
  resize_queues_and_workers(options.num_threads - 1);
}


inline
Station::~Station()
{
  if (not joined)
    join();
}


Station::Queues::const_iterator inline
Station::find_smallest_queue(std::size_t & smallest_size)
{
  Queues::const_iterator smallest_q_it;

  for (auto q_it = queues.cbegin(); q_it != queues.cend(); ++q_it)
  {
    std::size_t const current_queue_size = (*q_it)->get_number_of_items_in_queue();

    if (current_queue_size < smallest_size)
    {
      smallest_size = current_queue_size;
      smallest_q_it = q_it;

      // Check if any queue is empty, and if so then we don't need to look any further
      if (smallest_size == 0)
        break;
    }
  }

  return smallest_q_it;
}


} // namespace stations
