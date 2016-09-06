#pragma once

#include <algorithm> // std::sort
#include <memory> // std::shared_ptr


namespace stations_internal
{

/** Returns a number with a single bit set, the bit set is the most significant bit of the input. If the input is 0, then 1 is returned. Examples:
 * highest_ordered_bit(0) = 1
 * highest_ordered_bit(1) = 1
 * highest_ordered_bit(2) = 2
 * highest_ordered_bit(3) = 2
 * highest_ordered_bit(4) = 4
 * highest_ordered_bit(7) = 4
 * highest_ordered_bit(9) = 8
 */
template <typename T>
T inline
highest_ordered_bit(T num)
{
  T ret = 1;

  while (num >>= 1)
    ret <<= 1;

  return ret;
}


template <typename TVector>
void inline
merge_two_sorted_vectors(std::shared_ptr<TVector> merged, std::shared_ptr<TVector> i1, std::shared_ptr<TVector> i2)
{
  // Make sure merged vector is empty initially
  merged->clear();

  // If either sorted vector is empty, the other is the merged vector.
  if (i1->size() == 0)
  {
    std::swap(*merged, *i2);
    return;
  }
  else if (i2->size() == 0)
  {
    std::swap(*merged, *i1);
    return;
  }

  // merged->reserve(i1->size() + i2->size());
  std::size_t i = 0;
  std::size_t j = 0;

  while (true)
  {
    if ((*i1)[i] <= (*i2)[j])
    {
      merged->push_back((*i1)[i++]);

      if (i == i1->size())
      {
        while (j < i2->size())
          merged->push_back((*i2)[j++]);

        break;
      }
    }
    else
    {
      merged->push_back((*i2)[j++]);

      if (j == i2->size())
      {
        while (i < i1->size())
          merged->push_back((*i1)[i++]);

        break;
      }
    }
  }
}


} // namespace stations_internal
