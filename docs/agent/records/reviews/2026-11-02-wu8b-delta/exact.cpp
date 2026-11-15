// Compares detail::holds_exactly<T>(c) with an independent reference for
// edge and random coefficients: for integer types std::in_range (with the
// header's stated modular rule for unsigned types), for binary floating
// types a round trip through long double, which holds every int64 exactly,
// and for a limits-only half type the bound (2^p - 1) 2^(emax - p) with
// divisibility.
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <random>
#include <utility>
#include <vector>
#if defined(PROBE_STDFLOAT)
#include <stdfloat>
#endif

#include <galoot/core/mv.hpp>

namespace core = galoot::core;

struct half_limits_only {
  float v = 0;
};
template <> struct std::numeric_limits<half_limits_only> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_exact = false;
  static constexpr int radix = 2;
  static constexpr int digits = 11;
  static constexpr int max_exponent = 16;
};

static std::vector<std::int64_t> values() {
  std::vector<std::int64_t> v{0, INT64_MIN, INT64_MAX, INT64_MIN + 1,
                              INT64_MAX - 1};
  for (int k = 0; k < 64; ++k) {
    const std::uint64_t p = std::uint64_t{1} << k;
    for (std::uint64_t d : {p - 1, p, p + 1}) {
      if (d <= static_cast<std::uint64_t>(INT64_MAX)) {
        v.push_back(static_cast<std::int64_t>(d));
        v.push_back(-static_cast<std::int64_t>(d));
      }
    }
    // odd parts of every width w times 2^k
    for (int w = 1; w + k < 64; ++w) {
      const std::uint64_t odd_lo = (std::uint64_t{1} << (w - 1)) | 1;
      const std::uint64_t odd_hi = (std::uint64_t{1} << w) - 1;
      for (std::uint64_t o : {odd_lo, odd_hi}) {
        const std::uint64_t d = o << k;
        if (d <= static_cast<std::uint64_t>(INT64_MAX)) {
          v.push_back(static_cast<std::int64_t>(d));
          v.push_back(-static_cast<std::int64_t>(d));
        }
      }
    }
  }
  const std::uint64_t seed = 20261101;
  std::printf("seed %" PRIu64 "\n", seed);
  std::mt19937_64 rng(seed);
  for (int i = 0; i < 400000; ++i) {
    const int bits = static_cast<int>(rng() % 64);
    std::uint64_t m = bits == 0 ? 0 : (rng() >> (64 - bits));
    const int tz = static_cast<int>(rng() % 64);
    m = tz < 64 ? (m << tz) : m;
    m &= static_cast<std::uint64_t>(INT64_MAX);
    const auto s = static_cast<std::int64_t>(m);
    v.push_back((rng() & 1) ? s : -s);
  }
  return v;
}

static long failures = 0;
template <class T, class Ref>
void check(const char* name, const std::vector<std::int64_t>& vs, Ref ref) {
  long accepted = 0, n = 0;
  for (std::int64_t c : vs) {
    const bool got = core::detail::holds_exactly<T>(c);
    const bool want = ref(c);
    ++n;
    accepted += got;
    if (got != want) {
      if (++failures < 40) {
        std::printf("MISMATCH %s c=%" PRId64 " got %d want %d\n", name, c, got,
                    want);
      }
    }
  }
  std::printf("%-20s %ld values, %ld accepted\n", name, n, accepted);
}

template <class T> bool int_ref(std::int64_t c) {
  if constexpr (std::numeric_limits<T>::is_signed) {
    // round trip: conversion to a narrower signed type is modular (C++20)
    return static_cast<std::int64_t>(static_cast<T>(c)) == c;
  } else {
    // stated rule: |c| <= max()
    const std::uint64_t mag =
        c < 0 ? std::uint64_t{0} - static_cast<std::uint64_t>(c)
              : static_cast<std::uint64_t>(c);
    return mag <= static_cast<std::uint64_t>(std::numeric_limits<T>::max());
  }
}
template <class T> bool float_ref(std::int64_t c) {
  const T t = static_cast<T>(c);
  const long double back = static_cast<long double>(t);
  return back == static_cast<long double>(c);
}
bool half_ref(std::int64_t c) {
  const std::uint64_t mag =
      c < 0 ? std::uint64_t{0} - static_cast<std::uint64_t>(c)
            : static_cast<std::uint64_t>(c);
  const int p = 11, emax = 16;
  const std::uint64_t maxfinite = ((std::uint64_t{1} << p) - 1)
                                  << (emax - p);  // 65504
  if (mag > maxfinite) return false;
  int bw = 0;
  for (std::uint64_t x = mag; x; x >>= 1) ++bw;
  if (bw <= p) return true;
  const std::uint64_t unit = std::uint64_t{1} << (bw - p);
  return mag % unit == 0;
}

int main() {
  const auto vs = values();
  check<signed char>("signed char", vs, int_ref<signed char>);
  check<char>("char", vs, int_ref<char>);
  check<short>("short", vs, int_ref<short>);
  check<int>("int", vs, int_ref<int>);
  check<long>("long", vs, int_ref<long>);
  check<long long>("long long", vs, int_ref<long long>);
  check<std::int64_t>("int64", vs, int_ref<std::int64_t>);
  check<std::int32_t>("int32", vs, int_ref<std::int32_t>);
  check<wchar_t>("wchar_t", vs, int_ref<wchar_t>);
  check<unsigned char>("unsigned char", vs, int_ref<unsigned char>);
  check<unsigned short>("unsigned short", vs, int_ref<unsigned short>);
  check<unsigned>("unsigned", vs, int_ref<unsigned>);
  check<std::uint64_t>("uint64", vs, int_ref<std::uint64_t>);
  check<char16_t>("char16_t", vs, int_ref<char16_t>);
  check<char32_t>("char32_t", vs, int_ref<char32_t>);
  check<float>("float", vs, float_ref<float>);
  check<double>("double", vs, float_ref<double>);
  check<long double>("long double", vs, float_ref<long double>);
  check<half_limits_only>("half (limits only)", vs, half_ref);
#if defined(PROBE_STDFLOAT)
  check<std::float16_t>("float16_t", vs, float_ref<std::float16_t>);
  check<std::bfloat16_t>("bfloat16_t", vs, float_ref<std::bfloat16_t>);
  check<std::float32_t>("float32_t", vs, float_ref<std::float32_t>);
  check<std::float64_t>("float64_t", vs, float_ref<std::float64_t>);
  check<std::float128_t>("float128_t", vs, float_ref<std::float128_t>);
#endif
  std::printf("failures %ld\n", failures);
  return failures == 0 ? 0 : 1;
}
