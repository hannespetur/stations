#pragma once

#include <thread> // std::thread

namespace stations
{

/** Each boss thread mode defines how the boss thread will react to work if all worker queues are above the max_queue_size. */
enum BOSS_THREAD_MODE
{
  HARD_WORKING_BOSS, /** Boss will do the work. */
  PATIENT_BOSS, /** Boss will sleep until any worker queue is below the max_queue_size. */
  ORGANIZED_BOSS /** Boss will always add work to the smallest worker queue, disregarding the max_queue_size parameter. */
};

class StationOptions
{
  friend class Station; /** Allow stations to see your privates. */

private:
  std::size_t num_threads = std::thread::hardware_concurrency() + 1; /** Number of threads to use, including the main thread */

public:
  BOSS_THREAD_MODE boss_thread_mode = PATIENT_BOSS;
  std::size_t max_queue_size = 1; /** Maximum size of the worker queues, including jobs which are running */
  std::size_t chunk_size = 0; /** Number of items in each chunk of work to process. If 0, then the work will be evenly distributed among all threads. */
  std::size_t verbosity = 0; /** 0 is quite mode, 1 can output warnings, and 2 will output warnings and statistics to std::cout. */

  void inline
  set_num_threads(std::size_t const _num_threads)
  {
    if (_num_threads == 0)
      num_threads = std::max(static_cast<uint32_t>(1), std::thread::hardware_concurrency());
    else
      num_threads = _num_threads;
  }

  std::size_t inline
  get_num_threads() const
  {
    return num_threads;
  }
};

} // namespace stations
