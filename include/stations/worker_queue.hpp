#pragma once

#include <atomic>
#include <iostream>
#include <thread>


namespace stations
{

class WorkerQueue
{
 public:
  std::vector<std::function<void()> > function_queue;
  bool finished = false;
  std::atomic<std::size_t> queue_size;
  std::size_t n = 0; // Index of next function to run

  WorkerQueue()
  {
    queue_size = 0;
  }

  void inline
  add_work_to_queue(std::function<void()> work)
  {
    function_queue.push_back(work);
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
    return n;
  }

  void inline
  operator()()
  {
    while (true)
    {
      if (queue_size > 0)
      {
        function_queue[n]();
        ++n;
        --queue_size;
      }
      else if (finished)
      {
        return;
      }
    }
  }
};

} // namespace stations
