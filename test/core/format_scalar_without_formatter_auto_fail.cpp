// A line over a scalar type that has no std::formatter, formatted into auto s.
// This file must not compile.
#include <cstdint>
#include <format>
#include <string>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/format.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

// A ring scalar with no std::formatter.
struct plain_number {
  std::int64_t value = 0;

  constexpr plain_number() = default;
  explicit constexpr plain_number(std::int64_t v) noexcept : value(v) {}

  [[maybe_unused]] friend constexpr plain_number
  operator+(plain_number a, plain_number b) noexcept {
    return plain_number{a.value + b.value};
  }
  [[maybe_unused]] friend constexpr plain_number
  operator-(plain_number a, plain_number b) noexcept {
    return plain_number{a.value - b.value};
  }
  [[maybe_unused]] friend constexpr plain_number
  operator*(plain_number a, plain_number b) noexcept {
    return plain_number{a.value * b.value};
  }
  [[maybe_unused]] friend constexpr plain_number
  operator-(plain_number a) noexcept {
    return plain_number{-a.value};
  }
};

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, plain_number>;

[[maybe_unused]] void use(const Line& x) { auto s = std::format("{}", x); }

} // namespace
