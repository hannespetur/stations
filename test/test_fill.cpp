#include <catch.hpp>

#include <deque> // std::deque
#include <list> // std::list
#include <vector> // std::vector

#include <stations/algorithm.hpp> // stations::all_of


/***********************
 * Filling empty range *
 ***********************/
template<typename T>
void
check_empty_ints()
{
  T ints;
  stations::fill(ints.begin(), ints.end(), 1);
  REQUIRE(ints.size() == 0);
}

TEST_CASE("Running fill with empty container")
{
  SECTION("Empty vector")
    check_empty_ints<std::vector<int> >();
}


template<typename T>
void
check_few_ints()
{
  T ints = {1, 2, 3, 4, 5};
  stations::fill(ints.begin(), ints.begin() + 4, 1);
  REQUIRE(ints.size() == 5);
  REQUIRE(ints[0] == 1);
  REQUIRE(ints[1] == 1);
  REQUIRE(ints[2] == 1);
  REQUIRE(ints[3] == 1);
  REQUIRE(ints[4] == 5);
}


TEST_CASE("Running fill with a small container")
{
  SECTION("Small vector")
    check_few_ints<std::vector<int> >();
}
