#pragma once
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
  /**
   * Instance variables
   */
  bool verbose = false; // Set to true to output statistics about the thread distribution at the end of the run.

  /**
   * Class member functions
   */
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
  join()
  {
    if (verbose)
      std::cout << "Main thread processed " << main_thread_work_count << " chunks." << std::endl;

    for (long i = 0; i < static_cast<long>(thread_count) - 1; ++i)
    {
      queues[i]->finished = true;
      workers[i].join();

      if (verbose)
        std::cout << "Thread " << (i + 1) << " processed " << queues[i]->get_number_of_completed_items() << " chunks." << std::endl;
    }

    joined = true;
  }
};

} // namespace stations
