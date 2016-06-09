#pragma once
#include <assert.h>
#include <algorithm>
#include <atomic>
#include <iostream>
#include <iterator>
#include <memory>
#include <thread>
#include <vector>



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


} // namespace stations


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
    // WorkerQueue new_queue;
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
    {
      join();
    }
  }


  template <typename TWork, typename ... Args>
  void inline
  add(TWork && work, Args ... args)
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
        (*min_queue_it)->add_work_to_queue(std::bind(std::forward<TWork>(work), std::ref(*args) ...));
        return;
      }
    }

    work(std::ref(*args) ...);
    ++main_thread_work_count;
  }


  template <typename TWork, typename ... Args>
  void inline
  add_to_thread(std::size_t const thread_id, TWork && work, Args ... args)
  {
    if (thread_id % thread_count == thread_count - 1)
    {
      work(std::ref(*args) ...);
      ++main_thread_work_count;
    }
    else
    {
      queues[thread_id % thread_count]->add_work_to_queue(std::bind(std::forward<TWork>(work), std::ref(*args) ...));
    }
  }


  void inline
  join()
  {
    std::cout << "Main thread processed " << main_thread_work_count << " chunks." << std::endl;

    for (long i = 0; i < static_cast<long>(thread_count) - 1; ++i)
    {
      queues[i]->finished = true;
      workers[i].join();
      std::cout << "Thread " << (i + 1) << " processed " << queues[i]->get_number_of_completed_items() << " chunks." << std::endl;
    }

    joined = true;
  }


};

} // namespace stations


namespace stations
{

template <typename TContainer>
inline
std::vector<std::shared_ptr<TContainer> >
split(TContainer & container, std::size_t const PARTS)
{
  assert(PARTS > 0);
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


} // namespace stations
