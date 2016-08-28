#include <algorithm>
#include <bitset>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <stations/algorithm.hpp>
#include <stations/join.hpp>
#include <stations/split.hpp>
#include <stations/station.hpp>
#include <stations/worker_queue.hpp>

void
get_random_ints(std::vector<int> & ints, std::size_t const & num_ints)
{
  // Random number between (and including) -1,000,000 and 1,000,000
  for (std::size_t i = 0; i < num_ints; ++i)
  {
    ints.push_back(rand() % 20000001 - 10000000);
  }
}


int
main(int argc, char ** argv)
{
  srand(42);  // Seed is 42

  if (argc != 3)
  {
    std::cout << "Usage: " << argv[0] << " <NUM_INTS> <NUM_THREADS>" << std::endl;
    std::exit(1);
  }


  int const num_ints = std::stoi(argv[1]);
  int const num_threads = std::stoi(argv[2]);

  std::vector<int> ints;
  get_random_ints(ints, num_ints);
  auto start = std::chrono::system_clock::now();
  stations::sort(ints.begin(), ints.end(), num_threads);
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cout << "Total number of ints sorted are " << ints.size() << std::endl;
  std::cout << "Sorting duration was " << diff.count() << " seconds." << std::endl;

  // Make sure the vector is sorted
  for (auto it = ints.begin() + 1; it != ints.end(); ++it)
  {
    if (*it < *(it - 1))
    {
      std::cout << "NOT SORTED" << std::endl;
    }
  }
}
