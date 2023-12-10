/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-07
 *==========================*/

#include "catch2/catch_test_macros.hpp"
#include "core/draw/types.hpp"
#include "core/logger/logger.hpp"
#include "types.hpp"
#include <cstring>
#include <utility>

/* #define private public */
#include "core/draw/anim.hpp"

inline const ivec SIZE{2, 2};
inline const usize ISIZE = SIZE.x * SIZE.y;

using namespace draw;

void test_color(rgba8 c1, rgba8 c2) {
  REQUIRE(std::memcmp(&c1, &c2, 4) == 0);
}

void test_empty_layer(const Image& image, u32 id) noexcept {
  REQUIRE(image.get_id() == id);
  REQUIRE(image.get_size().x == SIZE.x);
  REQUIRE(image.get_width() == SIZE.x);
  REQUIRE(image.get_size().y == SIZE.y);
  REQUIRE(image.get_height() == SIZE.y);
  REQUIRE(image.get_pixels() != nullptr);

  for (i32 i = 0; i < ISIZE; ++i) {
    test_color(*(rgba8*)image.get_pixel(i), {});
  }
}

void test_init_anim(const Anim& anim) noexcept {
  REQUIRE(anim.get_size().x == SIZE.x);
  REQUIRE(anim.get_width() == SIZE.x);
  REQUIRE(anim.get_size().y == SIZE.y);
  REQUIRE(anim.get_height() == SIZE.y);
  REQUIRE(anim.get_layer_count() == 1);

  auto image = anim.get_image_fast(1U);
  test_empty_layer(image, 1U);
}

TEST_CASE("Anim: layers modification", "[draw]") {
  Anim anim{};

  TRY_ABORT(anim.init(SIZE, ColorType::RGBA8), "Could not init anim");
  test_init_anim(anim);

  usize id = 0U;
  Image image{};

  const usize INSERT_COUNT = 6U;

  for (usize i = 2U; i <= INSERT_COUNT; ++i) {
    TRY_ABORT(anim.insert_layer(0U), "Could not update anim");
    id = *TRY_ABORT_RET(anim.create_image(0U, 0U), "Could not update anim");

    REQUIRE(id == i);
    REQUIRE(anim.get_layer_count() == i);

    image = *TRY_ABORT_RET(anim.get_image(id), "Could not read anim");
    test_empty_layer(image, id);
  }

  // Check ordering
  for (usize i = 0; i < INSERT_COUNT; ++i) {
    id = anim.get_image_id(0U, i);
    REQUIRE(i + id == 6U);
  }

  // Edit the layers
  const rgba8 expected_colors[] = {
      {0x11, 0x11, 0x11, 0x11}, {0x22, 0x22, 0x22, 0x22},
      {0x33, 0x33, 0x33, 0x33}, {0x44, 0x44, 0x44, 0x44},
      {0x55, 0x55, 0x55, 0x55}, {0x66, 0x66, 0x66, 0x66}};
  for (id = 1U; id <= INSERT_COUNT; ++id) {
    image = *TRY_ABORT_RET(anim.get_image(id), "Could not read anim");
    for (i32 i = 0; i < ISIZE; ++i) {
      image.paint(i, expected_colors[id - 1]);
    }
  }

  for (id = 1U; id <= INSERT_COUNT; ++id) {
    image = *TRY_ABORT_RET(anim.get_image(id), "Could not read anim");
    for (usize i = 0; i < ISIZE; ++i) {
      test_color(*(rgba8*)image.get_pixel(i), expected_colors[id - 1]);
    }
  }
}

