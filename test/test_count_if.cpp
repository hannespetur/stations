#include <catch.hpp>

#include <deque> // std::deque
#include <list> // std::list
#include <vector> // std::vector

#include <stations/algorithm.hpp> // stations::count

/*****************************
 * Counting empty containers *
 *****************************/
template<typename T>
void
check_empty_ints()
{
  T ints;
  REQUIRE(stations::count_if(ints.begin(), ints.end(), [](int i){return i <= 5;}) == 0);
}

TEST_CASE("Counting empty containers using count_if")
{
  SECTION("Empty vector")
    check_empty_ints<std::vector<int> >();

  SECTION("Empty list")
    check_empty_ints<std::list<int> >();

  SECTION("Empty deque")
    check_empty_ints<std::deque<int> >();
}


/*********************
 * Counting elements *
 *********************/
template<typename T>
void
check_int_count_if_case1()
{
  T ints = {0, 2, 4, 6, 0, 78, 8, 1, 2, 4, 6, 5, 7, 8, 554, 345, 23, 53, 64, 0, 65, 13123, 4444, 0};
  REQUIRE(stations::count_if(ints.begin(), ints.end(), [](int i){return i <= 5;}) == 10);
}


TEST_CASE("Counting elements in a vector case 1 using count_if")
{
  SECTION("Empty vector")
    check_int_count_if_case1<std::vector<int> >();

  SECTION("Empty list")
    check_int_count_if_case1<std::list<int> >();

  SECTION("Empty deque")
    check_int_count_if_case1<std::deque<int> >();
}


template<typename T>
void
check_int_count_if_case2()
{
  T ints = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  ints.resize(10000000, 0); // 10000000 - 5 = 9999995 less or equal to 5
  REQUIRE(stations::count_if(ints.begin(), ints.end(), [](int i){return i <= 5;}) == 9999995);
}


TEST_CASE("Counting elements in a vector case 2 using count_if")
{
  SECTION("Empty vector")
    check_int_count_if_case2<std::vector<int> >();
}
