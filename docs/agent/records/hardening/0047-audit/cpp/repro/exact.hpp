// A small exact scalar for the reproducers: an integer wrapper whose
// constructor from std::int64_t is explicit, as a rational type's often is.
#ifndef REPRO_EXACT_HPP
#define REPRO_EXACT_HPP
#include <cstdint>
#include <limits>

struct Exact {
  std::int64_t v{};
  constexpr Exact() = default;
  constexpr explicit Exact(std::int64_t x) : v(x) {}
  friend constexpr Exact operator+(Exact a, Exact b) { return Exact{a.v + b.v}; }
  friend constexpr Exact operator-(Exact a, Exact b) { return Exact{a.v - b.v}; }
  friend constexpr Exact operator*(Exact a, Exact b) { return Exact{a.v * b.v}; }
  friend constexpr Exact operator-(Exact a) { return Exact{-a.v}; }
  friend constexpr bool operator==(Exact, Exact) = default;
};

template <>
struct std::numeric_limits<Exact> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_exact = true;
};
#endif
