#include <catch.hpp>

#include <algorithm> // std::sort
#include <deque> // std::deque
#include <list> // std::list
#include <vector> // std::vector

#include <stations/split_iterator.hpp>


TEST_CASE("Get split iterators for an empty vector")
{
  std::vector<double> doubles;

  SECTION("One part")
  {
    std::vector<std::vector<double>::iterator> split_iters = stations::split_iterators(doubles.begin(), doubles.end(), 1 /*PARTS*/);
    REQUIRE(split_iters.size() == 2);
    REQUIRE(split_iters[0] == doubles.begin());
    REQUIRE(split_iters[1] == doubles.begin()); // Note: Here, doubles.begin() == doubles.end().
  }

  SECTION("Two parts")
  {
    std::vector<std::vector<double>::iterator> split_iters = stations::split_iterators(doubles.begin(), doubles.end(), 2 /*PARTS*/);
    REQUIRE(split_iters.size() == 3);
    REQUIRE(split_iters[0] == doubles.begin());
    REQUIRE(split_iters[1] == doubles.begin());
    REQUIRE(split_iters[2] == doubles.begin());
  }
}


TEST_CASE("Get split iterators for some vector")
{
  std::vector<int> ints = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

  SECTION("One part")
  {
    std::vector<std::vector<int>::iterator> split_iters = stations::split_iterators(ints.begin(), ints.end(), 1 /*PARTS*/);
    REQUIRE(split_iters.size() == 2);
    REQUIRE(split_iters[0] == ints.begin());
    REQUIRE(split_iters[1] == ints.begin() + 10);
    REQUIRE(split_iters[1] == ints.end());
  }

  SECTION("Two parts")
  {
    std::vector<std::vector<int>::iterator> split_iters = stations::split_iterators(ints.begin(), ints.end(), 2 /*PARTS*/);
    REQUIRE(split_iters.size() == 3);
    REQUIRE(split_iters[0] == ints.begin());
    REQUIRE(split_iters[1] == ints.begin() + 5);
    REQUIRE(split_iters[2] == ints.begin() + 10);
    REQUIRE(split_iters[2] == ints.end());
  }

  SECTION("Three parts")
  {
    std::vector<std::vector<int>::iterator> split_iters = stations::split_iterators(ints.begin(), ints.end(), 3 /*PARTS*/);
    REQUIRE(split_iters.size() == 4);
    REQUIRE(split_iters[0] == ints.begin());
    REQUIRE(split_iters[1] == ints.begin() + 4);
    REQUIRE(split_iters[2] == ints.begin() + 7);
    REQUIRE(split_iters[3] == ints.begin() + 10);
    REQUIRE(split_iters[3] == ints.end());

    // Also make sure the iterators are valid after sorting
    std::sort(split_iters[0], split_iters[1]);
    std::sort(split_iters[1], split_iters[2]);
    std::sort(split_iters[2], split_iters[3]);

    REQUIRE(split_iters[0] == ints.begin());
    REQUIRE(split_iters[1] == ints.begin() + 4);
    REQUIRE(split_iters[2] == ints.begin() + 7);
    REQUIRE(split_iters[3] == ints.begin() + 10);

    // Make sure the partitions are sorted
    REQUIRE(*split_iters[0] == 6);
    REQUIRE(*split_iters[1] == 3);
    REQUIRE(*split_iters[2] == 0);
  }

  SECTION("Ten parts")
  {
    std::vector<std::vector<int>::iterator> split_iters = stations::split_iterators(ints.begin(), ints.end(), 10 /*PARTS*/);
    REQUIRE(split_iters.size() == 11);

    for (unsigned i = 0; i < 11; ++i)
      REQUIRE(split_iters[i] == ints.begin() + i);

    REQUIRE(split_iters.back() == ints.end());
  }
}
