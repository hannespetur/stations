#include <catch.hpp>

#include <list> // std::list
#include <vector> // std::vector

#include <stations/internal/data_simulation.hpp> // stations_internal::get_random_ints

#include <stations/algorithm.hpp> // stations::sort


TEST_CASE("Sorting empty containers")
{
  SECTION("Vector of integers")
  {
    std::vector<int> ints;
    stations::sort(4, ints.begin(), ints.end());
    REQUIRE(ints.size() == 0);
  }

  SECTION("List of integers")
  {
    std::list<int> ints;
    stations::sort(4, ints.begin(), ints.end());
    REQUIRE(ints.size() == 0);
  }
}


TEST_CASE("Sorting large containers")
{
  std::vector<int> ints = stations_internal::get_random_ints<std::vector<int> >(100000);
  stations::sort(4, ints.begin(), ints.end());
}
