#pragma once

#include <memory> // std::shared_ptr
#include <vector> // std::vector

namespace stations
{

template <typename TContainer>
inline
void
join(TContainer & container, std::vector<std::shared_ptr<TContainer> > & split_container)
{
  std::size_t const PARTS = split_container.size();

  for (std::size_t i = 0; i < PARTS; ++i)
  {
    std::move(split_container[i]->begin(), split_container[i]->end(), std::back_inserter(container));
    split_container[i]->clear();
  }
}


template <typename TContainer, typename Function>
inline
void
join(TContainer & map, std::vector<std::shared_ptr<TContainer> > & split_map, Function merge_fun)
{
  std::size_t const PARTS = split_map.size();

  for (auto & a_map : split_map)
  {
    for (typename TContainer::iterator it = a_map->begin(); it != a_map->end(); ++it)
      merge_fun(map, it);

    a_map->clear(); // Free memory
  }
}


} // namespace stations
