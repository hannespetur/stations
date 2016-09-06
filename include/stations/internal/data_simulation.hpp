#pragma once

#include <vector> // std::vector

namespace stations_internal
{

template<typename TContainer>
TContainer
get_random_ints(std::size_t const N)
{
  TContainer ints;

  for (std::size_t i = 0; i < N; ++i)
    ints.push_back(rand() % 20000001 - 10000000); // Random number between (and including) -1,000,000 and 1,000,000

  return ints;
}

// std::vector specialization
template<>
std::vector<int>
get_random_ints(std::size_t const N)
{
  std::vector<int> ints;
  ints.reserve(N);

  for (std::size_t i = 0; i < N; ++i)
    ints.push_back(rand() % 20000001 - 10000000); // Random number between (and including) -1,000,000 and 1,000,000

  return ints;
}

} // namespace stations_internal
