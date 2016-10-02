#include <catch.hpp>

#include <algorithm> // std::is_sorted
#include <array> // std::array
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
  stations::sort(ints.begin(), ints.end());
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
 stations::sort(ints.begin(), ints.end());
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
 * Sorting simple container *
 ****************************/
template<typename T>
void
check_simple_floats()
{
  std::array<float, 7> const initial_floats  = {0.5f, 0.2f, 0.1f, 100.0f, -0.2f, 0.4f, 0.9f};
  std::array<float, 7> const expected_floats = {-0.2f, 0.1f, 0.2f, 0.4f, 0.5f, 0.9f, 100.0f};
  T floats;

  for (auto it = initial_floats.cbegin(); it != initial_floats.cend(); ++it)
    floats.push_back(*it);

  REQUIRE(floats.size() == 7);
  REQUIRE(!std::is_sorted(floats.begin(), floats.end()));
  stations::sort(floats.begin(), floats.end());
  REQUIRE(std::is_sorted(floats.begin(), floats.end()));

  // Iterator over both the array and the container
  auto it1 = expected_floats.cbegin();
  auto it2 = floats.cbegin();

  while (it1 != expected_floats.cend())
  {
    REQUIRE(*it1 == *it2);
    ++it1;
    ++it2;
  }
}


TEST_CASE("Sorting simple container")
{
  SECTION("Simple vector of integers")
    check_simple_floats<std::vector<float> >();

  SECTION("Simple list of integers")
    check_simple_floats<std::list<float> >();

  SECTION("Simple deque of integers")
    check_simple_floats<std::deque<float> >();
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
  stations::sort(ints.begin(), ints.end());
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
