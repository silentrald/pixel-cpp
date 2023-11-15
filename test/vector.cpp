/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-25
 *==========================*/

#include "core/ds/vector.hpp"
#include "catch2/catch_test_macros.hpp"
#include "core/logger/logger.hpp"
#include "types.hpp"

struct Test {
  i32 i = 0;
};

TEST_CASE("vector: init", "[ds]") {
  ds::vector<Test> v{};

  REQUIRE(v.get_size() == 0);
  REQUIRE(v.get_capacity() == 0);

  TRY_ABORT(v.push_back(Test{}), "Push back error");

  REQUIRE(v.get_size() == 1);
  REQUIRE(v.get_capacity() > 0);
  REQUIRE(v[0].i == 0);
}
