#include <catch.hpp>

#include <stations/internal/algorithm_help_functions.hpp> // stations_internal::get_random_ints


/********************************
 * highest_ordered_bit function *
 ********************************/
TEST_CASE("highest_ordered_bit function")
{
  /** Returns a number with a single bit set, the bit set is the most significant bit of the input. If the input is 0, then 1 is returned. */
  REQUIRE(stations_internal::highest_ordered_bit(0) == 1);
  REQUIRE(stations_internal::highest_ordered_bit(1) == 1);
  REQUIRE(stations_internal::highest_ordered_bit(2) == 2);
  REQUIRE(stations_internal::highest_ordered_bit(3) == 2);
  REQUIRE(stations_internal::highest_ordered_bit(4) == 4);
  REQUIRE(stations_internal::highest_ordered_bit(6) == 4);
  REQUIRE(stations_internal::highest_ordered_bit(7) == 4);
  REQUIRE(stations_internal::highest_ordered_bit(8) == 8);
  REQUIRE(stations_internal::highest_ordered_bit(65) == 64);
}
