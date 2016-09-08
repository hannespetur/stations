#include <catch.hpp>

#include <deque> // std::deque
#include <list> // std::list
#include <vector> // std::vector

#include <stations/split.hpp>


TEST_CASE("Split an empty vector")
{
  std::vector<double> doubles;
  auto find_non_zero_lambda = [](std::shared_ptr<std::vector<double> > dd){return dd->size() != 0;};

  SECTION("One part")
  {
    auto split_doubles = stations::split(doubles, 1 /*PARTS*/);
    REQUIRE(split_doubles.size() == 1);
    REQUIRE(split_doubles[0]->size() == 0);
  }

  SECTION("Two parts")
  {
    std::vector<std::shared_ptr<std::vector<double> > > split_doubles = stations::split(doubles, 2 /*PARTS*/);
    REQUIRE(split_doubles.size() == 2);
    auto find_it = std::find_if(split_doubles.begin(), split_doubles.end(), find_non_zero_lambda);
    REQUIRE(find_it == split_doubles.end());
  }

  SECTION("322 parts")
  {
    auto split_doubles = stations::split(doubles, 322 /*PARTS*/);
    REQUIRE(split_doubles.size() == 322);
    auto find_it = std::find_if(split_doubles.begin(), split_doubles.end(), find_non_zero_lambda);
    REQUIRE(find_it == split_doubles.end());
  }
}


TEST_CASE("Split some vector")
{
  std::vector<int> ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // Size is 10

  SECTION("One part")
  {
    auto split_ints = stations::split(ints, 1 /*PARTS*/);
    REQUIRE(split_ints.size() == 1);
    REQUIRE(split_ints[0]->size() == 10);
  }

  SECTION("Two parts")
  {
    auto split_ints = stations::split(ints, 2 /*PARTS*/);
    REQUIRE(split_ints.size() == 2);
    REQUIRE(split_ints[0]->size() == 5);
    REQUIRE(split_ints[1]->size() == 5);
  }

  SECTION("Three parts")
  {
    auto split_ints = stations::split(ints, 3 /*PARTS*/);
    REQUIRE(split_ints.size() == 3);
    REQUIRE(split_ints[0]->size() == 4); // The first one should be the largest one (because that one will be run first)
    REQUIRE(split_ints[1]->size() == 3);
    REQUIRE(split_ints[2]->size() == 3);
  }

  SECTION("Ten parts")
  {
    auto split_ints = stations::split(ints, 10 /*PARTS*/);
    REQUIRE(split_ints.size() == 10);

    for (unsigned i = 0; i < 10; ++i)
      REQUIRE(split_ints[i]->size() == 1);
  }
}


TEST_CASE("Split a list")
{
  std::list<double> floats = {0, 1, 2}; // Size is 3

  SECTION("Two parts")
  {
    auto split_floats = stations::split(floats, 2 /*PARTS*/);
    REQUIRE(split_floats.size() == 2);
    REQUIRE(split_floats[0]->size() == 2);
    REQUIRE(split_floats[1]->size() == 1);
  }
}


TEST_CASE("Split a deque")
{
  std::deque<unsigned> unsigneds = {0};

  SECTION("Three parts")
  {
    auto split_unsigneds = stations::split(unsigneds, 3 /*PARTS*/);
    REQUIRE(split_unsigneds.size() == 3);
    REQUIRE(split_unsigneds[0]->size() == 1);
    REQUIRE(split_unsigneds[1]->size() == 0);
    REQUIRE(split_unsigneds[1]->size() == 0);
  }
}
