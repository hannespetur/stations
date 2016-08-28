#pragma once
#include <assert.h>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>


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


template <typename BidirectionalIterator>
inline
std::vector<std::shared_ptr<std::vector<typename BidirectionalIterator::value_type> > >
split(BidirectionalIterator first, BidirectionalIterator last, std::size_t const PARTS)
{
  using TContainer = std::vector<typename BidirectionalIterator::value_type>;
  assert(PARTS > 0);
  std::vector<std::shared_ptr<TContainer> > split_container;
  split_container.resize(PARTS);
  std::size_t const container_original_size = std::distance(first, last);

  for (long i = static_cast<long>(PARTS) - 1; i >= 0; --i)
  {
    std::size_t const part_size = container_original_size / PARTS + (container_original_size % PARTS > static_cast<std::size_t>(i));
    split_container[i] =
      std::make_shared<TContainer>(std::make_move_iterator(std::next(last, -part_size)),
                                   std::make_move_iterator(last)
                                   );

    last = std::next(last, -part_size);
  }

  return split_container;
}


} // namespace stations
