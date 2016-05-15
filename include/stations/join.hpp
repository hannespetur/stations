#pragma once

namespace stations
{

template<typename TContainer> inline
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

} // namespace stations
