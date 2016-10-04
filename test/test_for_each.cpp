#include <catch.hpp>

#include <deque> // std::deque
#include <list> // std::list
#include <mutex> // std::mutex
#include <vector> // std::vector

#include <stations/algorithm.hpp> // stations::count

/********************************
 * For each in empty containers *
 ********************************/
template<typename T>
void
check_empty_ints()
{
  T ints;
  std::vector<int> v;
  stations::for_each(ints.begin(), ints.end(), [&](int i){v.push_back(2*i);});
  REQUIRE(v.size() == 0);
}

TEST_CASE("Running for_each on an empty container")
{
  SECTION("Empty vector")
    check_empty_ints<std::vector<int> >();

  SECTION("Empty list")
    check_empty_ints<std::list<int> >();

  SECTION("Empty deque")
    check_empty_ints<std::deque<int> >();
}

/*************************************
 * For each in a non-empty container *
 *************************************/
template<typename T>
void
check_non_empty_ints()
{
  // This example is stupid, this will always run faster serialized, but whatever
  int half_sum = 0;
  std::mutex m;
  T ints = {1, 2, 3, 6, 4, 1, 2, 5, 2, 10}; // 0 + 1 + 1 + 3 + 2 + 0 + 1 + 2 + 1 + 5 == 16
  stations::for_each(ints.begin(), ints.end(), [&](int i){std::lock_guard<std::mutex> guard(m); half_sum += i / 2;});
  REQUIRE(half_sum == 16);
}


TEST_CASE("Check if for_each work in for a small set of integers")
{
  SECTION("Vector")
    check_non_empty_ints<std::vector<int> >();

  SECTION("List")
    check_non_empty_ints<std::list<int> >();

  SECTION("Deque")
    check_non_empty_ints<std::deque<int> >();
}
