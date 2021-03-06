#include <chrono> // std::chrono::system_clock::now
#include <iostream> // std::cout, std::endl;

#include <stations/internal/data_simulation.hpp> // stations_internal::get_random_ints
#include <stations/algorithm.hpp> // stations::count_if
#include <stations/station_options.hpp> // stations::count_if

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
  stations::StationOptions options;
  // ++options.num_threads;
  options.num_threads = 8;
  options.chunk_size = N / 100;
  //options.boss_thread_mode = stations::PATIENT_BOSS;

  // Benchmark starts here
  auto t1 = std::chrono::system_clock::now();
  std::size_t const COUNT =
    stations::count_if(std::move(options), ints.begin(), ints.end(), [](int n){
    return n < 0 && (-n % 2) == 0 && ((n * n / 2) % 2 == 0);
  });

  auto t2 = std::chrono::system_clock::now();

  std::cout << "Total: " << static_cast<std::chrono::duration<double> >(t2 - t1).count()
            << " with count " << COUNT << "\n";
}
