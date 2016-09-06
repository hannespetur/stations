#include <catch.hpp>

#include <algorithm> // std::is_sorted
#include <deque> // std::deque
#include <list> // std::list
#include <vector> // std::vector

#include <stations/internal/data_simulation.hpp> // stations_internal::get_random_ints

#include <stations/algorithm.hpp> // stations::sort


/****************************
 * Sorting empty containers *
 ****************************/
template<typename T>
void
check_empty_ints()
{
  T ints;
  REQUIRE(ints.size() == 0);
  stations::sort(8, ints.begin(), ints.end());
  REQUIRE(ints.size() == 0);
}


TEST_CASE("Sorting empty containers")
{
  SECTION("Empty vector of integers")
    check_empty_ints<std::vector<int> >();

  SECTION("Empty list of integers")
    check_empty_ints<std::list<int> >();

  SECTION("Empty deque of integers")
    check_empty_ints<std::deque<int> >();
}


/********************************
 * Sorting containers of size 1 *
 ********************************/
template<typename T>
void
check_small_ints()
{
 T ints = stations_internal::get_random_ints<T>(1);
 REQUIRE(ints.size() == 1);
 REQUIRE(std::is_sorted(ints.begin(), ints.end()));
 stations::sort(8, ints.begin(), ints.end());
 REQUIRE(ints.size() == 1);
 REQUIRE(std::is_sorted(ints.begin(), ints.end()));
}


TEST_CASE("Sorting containers of size 1")
{
 SECTION("Large vector of integers")
   check_small_ints<std::vector<int> >();

 SECTION("Large list of integers")
   check_small_ints<std::list<int> >();

 SECTION("Large deque of integers")
   check_small_ints<std::deque<int> >();
}


/****************************
 * Sorting large containers *
 ****************************/
template<typename T>
void
check_large_ints()
{
  std::size_t const N = 10000;
  T ints = stations_internal::get_random_ints<T>(N);
  REQUIRE(ints.size() == N);
  REQUIRE(!std::is_sorted(ints.begin(), ints.end())); // Well, at least it is extremely unlikely to be sorted :)
  stations::sort(8, ints.begin(), ints.end());
  REQUIRE(ints.size() == N);
  REQUIRE(std::is_sorted(ints.begin(), ints.end()));
}


TEST_CASE("Sorting large containers")
{
  SECTION("Large vector of integers")
    check_large_ints<std::vector<int> >();

  SECTION("Large list of integers")
    check_large_ints<std::list<int> >();

  SECTION("Large deque of integers")
    check_large_ints<std::deque<int> >();
}
