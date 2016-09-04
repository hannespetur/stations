#include <algorithm>
#include <bitset>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <stations/internal/data_simulation.hpp>

#include <stations/algorithm.hpp>
#include <stations/join.hpp>
#include <stations/split.hpp>
#include <stations/station.hpp>
#include <stations/worker_queue.hpp>


int
main(int argc, char ** argv)
{
  srand(42);  // Seed is 42

  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " <NUM_INTS> <NUM_THREADS>" << std::endl;
    std::exit(1);
  }

  int const num_ints = std::stoi(argv[1]);
  int const num_threads = std::stoi(argv[2]);

  std::vector<int> ints = stations_internal::get_random_ints<std::vector<int> >(num_ints);
  auto start = std::chrono::system_clock::now();
  stations::sort(num_threads, ints.begin(), ints.end());
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cout << "Total number of ints sorted are " << ints.size() << std::endl;
  std::cout << "Sorting duration was " << diff.count() << " seconds." << std::endl;

  // Make sure the vector is sorted
  for (auto it = ints.begin() + 1; it != ints.end(); ++it)
  {
    if (*it < *(it - 1))
      std::cout << "NOT SORTED" << std::endl;
  }
}
