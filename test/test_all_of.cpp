#include <catch.hpp>

#include <deque> // std::deque
#include <list> // std::list
#include <vector> // std::vector

#include <stations/algorithm.hpp> // stations::all_of


/*****************************
 * Counting empty containers *
 *****************************/
template<typename T>
void
check_empty_ints()
{
  T ints;
  REQUIRE(stations::all_of(ints.begin(), ints.end(), [](int i){return i == 999999;}) == true); // Returns true if the range is empty
}

TEST_CASE("Running all_of with empty containers")
{
  SECTION("Empty vector")
    check_empty_ints<std::vector<int> >();
}


/*************************************
 * All of with a non-empty container *
 *************************************/
template<typename T>
void
check_for_all_with_a_non_empty_container_false()
{
  T ints = {1, 2, 3, 6, 4, 1, 2, 5, 2, 10};
  stations::all_of(ints.begin(), ints.end(), [](int i){return i < 10;}) == false;
}


template<typename T>
void
check_for_all_with_a_non_empty_container_true()
{
  T ints = {1, 2, 3, 6, 4, 1, 2, 5, 2, 10};
  stations::all_of(ints.begin(), ints.end(), [](int i){return i <= 10;}) == true;
}


TEST_CASE("Check if all_of works for a small set of integers")
{
  SECTION("Vector")
  {
    check_for_all_with_a_non_empty_container_false<std::vector<int> >();
    check_for_all_with_a_non_empty_container_true<std::vector<int> >();
  }
}
