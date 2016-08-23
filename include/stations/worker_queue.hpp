#pragma once

#include <atomic> // std::atomic
#include <iostream> // std::cout, std::endl
#include <thread> // std::this_thread::sleep_for
#include <list> // std::list


namespace stations
{

class WorkerQueue
{
public:
  std::list<std::function<void()> > function_queue;
  std::list<std::function<void()> >::iterator queue_it;
  bool finished = false;
  std::atomic<std::size_t> queue_size;
  std::size_t completed_items = 0;

  WorkerQueue()
    : queue_it(function_queue.end())
  {
    queue_size = 0;
  }


  void inline
  add_work_to_queue(std::function<void()> work)
  {
    function_queue.push_back(work); // Pushing back to lists does not invalidate iterators(!)

    // If this is the first element in the queue,
    if (queue_size == 0)
      queue_it = function_queue.begin();

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
        (*queue_it)();
        ++queue_it;
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
