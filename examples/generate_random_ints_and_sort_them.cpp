#include <algorithm>
#include <bitset>
#include <iostream>
#include <stdlib.h>
#include <vector>

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


void
sort_ints(std::vector<int> & ints)
{
  std::sort(ints.begin(), ints.end());
}


void
merge_two_sorted_vectors(std::vector<int> & merged, std::vector<int> const & i1, std::vector<int> const & i2)
{
  merged.resize(i1.size() + i2.size());
  std::size_t i = 0;
  std::size_t j = 0;
  std::size_t k = 0;

  while (i < i1.size() && j < i2.size())
  {
    if (i1[i] <= i2[j])
    {
      merged[k++] = i1[i++];
    }
    else
    {
      merged[k++] = i2[j++];
    }
  }

  while (i < i1.size())
  {
    merged[k++] = i1[i++];
  }

  while (j < i2.size())
  {
    merged[k++] = i2[j++];
  }
}


int main (int argc, char** argv)
{
  srand (42); // Seed is 42

  if (argc != 3)
  {
    std::cout << "Usage: " << argv[0] << " <NUM_INTS> <NUM_THREADS>" << std::endl;
    std::exit(1);
  }


  int const num_ints = std::stoi(argv[1]);
  int const num_threads = std::stoi(argv[2]);

  {
    std::bitset<64> bit_test(std::stoi(argv[2]));

    if (bit_test.count() > 1)
    {
      std::cout << "NUM_THREADS needs have only 2 as its prime factors." << std::endl;
      std::exit(1);
    }
  }

  auto start = std::chrono::system_clock::now();

  std::vector<int> ints;
  get_random_ints(ints, num_ints);

  // Split the ints into num_threads parts
  auto split_ints = stations::split(ints, num_threads);

  {
    stations::Station sort_station(num_threads, 1);

    for (std::size_t i = 0; i < static_cast<std::size_t>(num_threads); ++i)
    {
      sort_station.add_to_thread(i /*thread_id*/,
                       sort_ints /*function*/,
                       split_ints[i] /*data*/
                      );
    }
  }

  std::size_t d = 2;

  // Merge sort the split ints
  while (split_ints.size() > 1)
  {
    std::vector<int> merged_ints;
    auto new_split_ints = stations::split(merged_ints, num_threads / d);

    {
      stations::Station merge_sort_station(num_threads / d, 1);

      for (std::size_t i = 0; i < static_cast<std::size_t>(num_threads / d); ++i)
      {
        merge_sort_station.add_to_thread(i, merge_two_sorted_vectors, new_split_ints[i], split_ints[2*i], split_ints[2*i+1]);
      }
    }

    split_ints = new_split_ints;
    d *= 2;
  }

  // std::vector<int> merged;
  // merge_two_sorted_vectors(merged, *split_ints[0], *split_ints[1]);

  ints = *split_ints.at(0);

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end-start;
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
