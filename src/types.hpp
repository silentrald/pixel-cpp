/*==========================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-10-05
 *==========================*/

#ifndef PXL_TYPES_HPP
#define PXL_TYPES_HPP

#include "tl/expected.hpp"
#include <cstdint>
#include <string>

// === Main Types === //

using i8 = int8_t;
using i16 = short;
using i32 = int;
using i64 = long long;

using u8 = uint8_t;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using f32 = float;
using f64 = double;

using c8 = char;

// === Color Types === //

template <typename Type> struct rgba {
  union {
    struct {
      Type r{};
      Type g{};
      Type b{};
      Type a{};
    };
    Type colors[4]; // NOLINT
  };

  [[nodiscard]] bool operator==(rgba<Type> rhs) const noexcept {
    return *(i32*)this == *(i32*)&rhs;
  }

  [[nodiscard]] bool operator!=(rgba<Type> rhs) const noexcept {
    return *(i32*)this != *(i32*)&rhs;
  }
};

using rgba8 = rgba<u8>;
using rgba16 = rgba<u16>;

namespace color {

const rgba8 TRANSPARENT_COLOR{0x00, 0x00, 0x00, 0x00};

[[nodiscard]] std::string to_hex_string(rgba8 color) noexcept;
[[nodiscard]] rgba8 parse_hex_string(const c8* hex) noexcept;

} // namespace color

// === Geom Types === //

template <typename Type> struct vec {
  Type x{};
  Type y{};

  /**
   * Refer: https://en.wikipedia.org/wiki/Chebyshev_distance
   **/
  [[nodiscard]] i32 chebyshev_distance(vec<Type> other) noexcept {
    return std::max(std::abs(this->x - other.x), std::abs(this->y - other.y));
  }

  [[nodiscard]] vec<Type> operator-(vec<Type> rhs) {
    return {.x = this->x - rhs.x, .y = this->y - rhs.y};
  }

  [[nodiscard]] bool operator==(vec<Type> rhs) {
    return this->x == rhs.x && this->y == rhs.y;
  }

  [[nodiscard]] bool operator!=(vec<Type> rhs) {
    return this->x != rhs.x || this->y != rhs.y;
  }
};

template <typename Type> struct rect {
  union {
    struct {
      Type x;
      Type y;
      Type w;
      Type h;
    };
    struct {
      vec<Type> pos;
      vec<Type> size;
    };
  };

  /**
   * Check if the point is in the rect
   **/
  [[nodiscard]] inline bool has_point(const vec<Type>& point) const noexcept {
    return point.x >= this->x && point.y >= this->y && //
           point.x <= this->x + this->w && point.y <= this->y + this->h;
  }
};

// Int Vector
using ivec = vec<i32>;

// Float Vector
using fvec = vec<f32>;

// Int Rect
using irect = rect<i32>;

// Float Rect
using frect = rect<f32>;

// === Error Types === //

enum class error_code : u32 {
  // Generic
  OK = 0U,
  BAD_ALLOC,

  FILE_NOT_FOUND,
  FILE_SEEK,
  FILE_WRITE,
  FILE_READ
};

// Simplify expected with using error codes
template <typename T> using expected = tl::expected<T, error_code>;
using unexpected = tl::unexpected<error_code>;

inline bool is_error(error_code code) noexcept {
  return code != error_code::OK;
}

template <typename T> inline bool is_error(const expected<T>& exp) noexcept {
  return !exp;
}

inline unexpected to_unexpected(error_code code) noexcept {
  return unexpected{code};
}

template <typename T>
inline unexpected to_unexpected(const expected<T>& exp) noexcept {
  return unexpected{exp.error()};
}

inline error_code to_error_code(const unexpected& unexp) noexcept {
  return unexp.value();
}

template <typename T>
inline error_code to_error_code(const expected<T>& exp) noexcept {
  return exp.error();
}

inline void to_void(error_code code) noexcept {}
inline void to_void(const unexpected& unexp) noexcept {}
template <typename T> inline void to_void(const expected<T>& exp) noexcept {}

inline error_code get_code(error_code code) noexcept {
  return code;
}

inline error_code get_code(const unexpected& unexp) noexcept {
  return unexp.value();
}

template <typename T>
inline error_code get_code(const expected<T>& exp) noexcept {
  return exp.error();
}

#define TRY_1(code)                                                            \
  {                                                                            \
    auto val = code;                                                           \
    if (is_error(val)) {                                                       \
      return val;                                                              \
    }                                                                          \
  }

#define TRY_2(code, handle)                                                    \
  {                                                                            \
    auto val = code;                                                           \
    if (is_error(val)) {                                                       \
      handle;                                                                  \
      return val;                                                              \
    }                                                                          \
  }

#define TRY_3(code, handle, transformer)                                       \
  {                                                                            \
    auto val = code;                                                           \
    if (is_error(val)) {                                                       \
      handle;                                                                  \
      return transformer(val);                                                 \
    }                                                                          \
  }

#define TRY_X(x, a, b, c, func, ...) func

#define TRY(...)                                                               \
  TRY_X(                                                                       \
      , ##__VA_ARGS__, TRY_3(__VA_ARGS__), TRY_2(__VA_ARGS__),                 \
      TRY_1(__VA_ARGS__)                                                       \
  )

#define TRY_RET_1(code)                                                        \
  ({                                                                           \
    auto val = code;                                                           \
    if (is_error(val)) {                                                       \
      return val;                                                              \
    }                                                                          \
    std::move(val);                                                            \
  })

#define TRY_RET_2(code, handle)                                                \
  ({                                                                           \
    auto val = code;                                                           \
    if (is_error(val)) {                                                       \
      handle;                                                                  \
      return val;                                                              \
    }                                                                          \
    std::move(val);                                                            \
  })

#define TRY_RET_3(code, handle, transformer)                                   \
  ({                                                                           \
    auto val = code;                                                           \
    if (is_error(val)) {                                                       \
      handle;                                                                  \
      return transformer(val);                                                 \
    }                                                                          \
    std::move(val);                                                            \
  })

#define TRY_RET_X(x, a, b, c, func, ...) func

#define TRY_RET(...)                                                           \
  TRY_RET_X(                                                                   \
      , ##__VA_ARGS__, TRY_RET_3(__VA_ARGS__), TRY_RET_2(__VA_ARGS__),         \
      TRY_RET_1(__VA_ARGS__)                                                   \
  )

#define TRY_ABORT(code, msg)                                                   \
  {                                                                            \
    auto val = code;                                                           \
    if (is_error(val)) {                                                       \
      logger::fatal("ERRCODE: %u ; " msg, get_code(val));                      \
      std::abort();                                                            \
    }                                                                          \
  }

#define TRY_ABORT_RET(code, msg)                                               \
  ({                                                                           \
    auto val = code;                                                           \
    if (is_error(val)) {                                                       \
      logger::fatal("ERRCODE: %u ; " msg, get_code(val));                      \
      std::abort();                                                            \
    }                                                                          \
    std::move(val);                                                            \
  })

#define TRY_IGNORE(code, msg)                                                  \
  {                                                                            \
    auto val = code;                                                           \
    if (is_error(val)) {                                                       \
      logger::error("ERRCODE: %u ; " msg, get_code(val));                      \
    }                                                                          \
  }

#endif

