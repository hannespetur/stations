#pragma once

#include <iterator> // std::next
#include <vector> // std::vector

namespace stations
{

template<typename TContainer> inline
std::vector<typename TContainer::iterator>
split_iterators(TContainer & container, std::size_t const PARTS)
{
  std::vector<typename TContainer::iterator> split_iterators;
  split_iterators.reserve(PARTS + 1);
  split_iterators.push_back(container.begin());
  std::size_t const container_original_size = container.size();

  for (std::size_t i = 0; i < PARTS - 1; ++i)
  {
    std::size_t const part_size = container_original_size / PARTS + (i < container_original_size % PARTS);
    split_iterators.push_back(std::next(split_iterators.back(), part_size));
  }

  split_iterators.push_back(container.end());
  return split_iterators;
}


template <typename BidirectionalIterator> inline
std::vector<BidirectionalIterator>
split_iterators(BidirectionalIterator first, BidirectionalIterator last, std::size_t const PARTS)
{
  std::vector<BidirectionalIterator> split_iterators;
  split_iterators.reserve(PARTS + 1);
  split_iterators.push_back(first);
  std::size_t const container_original_size = std::distance(first, last);

  for (std::size_t i = 0; i < PARTS - 1; ++i)
  {
    std::size_t const part_size = container_original_size / PARTS + (i < container_original_size % PARTS);
    split_iterators.push_back(std::next(split_iterators.back(), part_size));
  }

  split_iterators.push_back(last);
  return split_iterators;
}

} // namespace stations
