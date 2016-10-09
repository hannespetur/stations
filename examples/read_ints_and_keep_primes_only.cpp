#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>


#include <stations/join.hpp>
#include <stations/split.hpp>
#include <stations/station.hpp>
#include <stations/worker_queue.hpp>

#include "read_integers_from_file.hpp"


bool
is_prime(int const number)
{
  if (number == 2)
    return true;

  if (number <= 1 or number % 2 == 0)
    return false;

  int i = 3;

  while (i*i <= number)
  {
    if (number % i == 0)
      return false;

    i += 2;
  }

  return true;
}


void
keep_primes(std::shared_ptr<std::vector<int> > numbers)
{
  numbers->erase(std::remove_if(numbers->begin(),
                                numbers->end(),
                                [](int const x){return !is_prime(x);}
                               ),
                 numbers->end()
                 );
}


int main (int argc, char** argv)
{
  if (argc != 3)
  {
    std::cout << "Usage: " << argv[0] << " <FILE_NAME> <NUM_THREADS>" << std::endl;
    std::exit(1);
  }

  // Read a file with positive integers. The format should be
  stations::ReadIntegersFromFile riff(argv[1]);
  std::size_t const num_threads = std::stoi(argv[2]);
  riff.set_chunk_size(4096);

  std::cout << "Number of threads are " << num_threads << "." << std::endl;
  std::cout << "Each chunk has " << riff.get_chunk_size() << " integers." << std::endl;

  {
    std::vector<std::shared_ptr<std::vector<int> > > all_primes;
    stations::Station keep_primes_station;

    for (auto ints = riff(); ints->size() > 0; ints = riff())
    {
      keep_primes_station.add_work(keep_primes, ints);
      all_primes.push_back(ints);
    }
  }
}
