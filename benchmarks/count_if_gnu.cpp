#include <chrono> // std::chrono::system_clock::now
#include <iostream> // std::cout, std::endl;

#include <stations/internal/data_simulation.hpp> // stations_internal::get_random_ints
#include <parallel/algorithm> // __gnu_parallel::count_if

#include "help_functions.hpp"


int
main()
{
  // Parameters
  std::size_t SEED = 42;
  std::size_t const N = 10000000;

  // Setup
  srand(SEED);
  std::vector<int> ints = stations_internal::get_random_ints<std::vector<int> >(N);

  // Benchmark starts here
  auto t1 = std::chrono::system_clock::now();
  // std::size_t const COUNT = __gnu_parallel::count_if(ints.begin(), ints.end(), [](int n){return is_prime(std::abs(n));});
  std::size_t const COUNT = __gnu_parallel::count_if(ints.begin(), ints.end(), [](int n){return n < 0;});
  auto t2 = std::chrono::system_clock::now();
  std::cout << "Total: " << static_cast<std::chrono::duration<double> >(t2 - t1).count() << " with count " << COUNT << std::endl;
}
