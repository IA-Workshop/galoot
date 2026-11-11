// Transforms under the build's floating-point contraction, for comparing
// builds that differ in -ffp-contract and GALOOT_CHECKING bit for bit.
//
// Families, each in double and in float, from a seeded generator:
//   rga3-point, rga3-line, rga3-plane: a rigid(3) motor with every
//     coefficient random, acting on a random point, line, or plane;
//   cga3-vector-2flat: a conformal(3) motor, the antiproduct of two flat
//     planes with coefficients k/8 (|k| <= 8), which is computed exactly,
//     so it is a versor, acting on a random vector;
//   cga3-bivector-2flat: the same motor on a random bivector;
//   cga3-vector-1hyp: one random hyperplane (an odd versor) on a vector.
// Random coefficients are uniform in [-1, 1) with 53 bits, then rounded to
// the scalar. For each case it prints the family, the scalar, the case
// number, and the transform's coefficients as hex floats.
//
// It also evaluates the first CT cases of every family in a constant
// expression, where the check always runs and no compiler contracts, and
// prints how many of their coefficients differ in bits from the same cases
// at run time.
//
//   cprobe <cases> <seed>     (the constant cases use seed 20261110)
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;

struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

constexpr std::uint64_t ct_seed = 20261110;
constexpr int ct_cases = 32;

// splitmix64, usable in a constant expression.
struct source {
  std::uint64_t state;
  constexpr std::uint64_t next() {
    std::uint64_t z = (state += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30U)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27U)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31U);
  }
  constexpr double uniform() {
    return (static_cast<double>(next() >> 11U) * 0x1p-53 * 2.0) - 1.0;
  }
  constexpr int small() { return static_cast<int>(next() % 17) - 8; }
};

template <class M, class T>
constexpr M random_mv(source& s) {
  M m;
  for (T& c : m.coefficients) {
    c = static_cast<T>(s.uniform());
  }
  return m;
}

template <class M, class T>
constexpr M dyadic_mv(source& s) {
  M m;
  for (T& c : m.coefficients) {
    c = static_cast<T>(s.small()) / T{8};
  }
  return m;
}

constexpr auto flat_planes = core::blades<cga3, "e1235 e1245 e1345 e2345">;
constexpr auto hyperplanes = core::of_grade<5, 4>;
constexpr auto vectors = core::of_grade<5, 1>;
constexpr auto bivectors = core::of_grade<5, 2>;

// The transform of case `i` of family F (cases are drawn in sequence, so
// case i consumes the draws of the cases before it: the generator is
// stepped from the start).
template <int F, class T>
constexpr auto one_case(source& s) {
  if constexpr (F == 0) {
    const auto q = random_mv<rga3::Motor<T>, T>(s);
    const auto x = random_mv<rga3::Point<T>, T>(s);
    return core::transform(x, q).coefficients;
  } else if constexpr (F == 1) {
    const auto q = random_mv<rga3::Motor<T>, T>(s);
    const auto x = random_mv<rga3::Line<T>, T>(s);
    return core::transform(x, q).coefficients;
  } else if constexpr (F == 2) {
    const auto q = random_mv<rga3::Motor<T>, T>(s);
    const auto x = random_mv<rga3::Plane<T>, T>(s);
    return core::transform(x, q).coefficients;
  } else if constexpr (F == 3 || F == 4) {
    using H = core::mv<cga3, flat_planes, T>;
    const auto p1 = dyadic_mv<H, T>(s);
    const auto p2 = dyadic_mv<H, T>(s);
    const auto v = core::compose(p2, p1);
    if constexpr (F == 3) {
      const auto x = random_mv<core::mv<cga3, vectors, T>, T>(s);
      return core::transform(x, v).coefficients;
    } else {
      const auto x = random_mv<core::mv<cga3, bivectors, T>, T>(s);
      return core::transform(x, v).coefficients;
    }
  } else {
    const auto v = random_mv<core::mv<cga3, hyperplanes, T>, T>(s);
    const auto x = random_mv<core::mv<cga3, vectors, T>, T>(s);
    return core::transform(x, v).coefficients;
  }
}

template <int F, class T>
using result_t = decltype(one_case<F, T>(std::declval<source&>()));

template <int F, class T>
constexpr std::array<result_t<F, T>, ct_cases> constant_cases() {
  source s{ct_seed};
  std::array<result_t<F, T>, ct_cases> out{};
  for (auto& r : out) {
    r = one_case<F, T>(s);
  }
  return out;
}

constexpr const char* names[] = {
    "rga3-point",         "rga3-line",           "rga3-plane",
    "cga3-vector-2flat", "cga3-bivector-2flat", "cga3-vector-1hyp",
};

template <class T>
bool same_bits(T a, T b) {
  return std::memcmp(&a, &b, sizeof(T)) == 0;
}

template <int F, class T>
void family(long cases, std::uint64_t seed, const char* scalar) {
  source s{seed};
  for (long i = 0; i < cases; ++i) {
    const auto r = one_case<F, T>(s);
    std::printf("%s %s %ld", names[F], scalar, i);
    for (const T c : r) {
      std::printf(" %a", static_cast<double>(c));
    }
    std::printf("\n");
  }
  // The constant cases against the same cases at run time; the seed is read
  // through a volatile so that the run-time cases are not folded.
  static constexpr auto ct = constant_cases<F, T>();
  volatile std::uint64_t runtime_seed = ct_seed;
  source rs{runtime_seed};
  long differ = 0;
  long total = 0;
  for (int i = 0; i < ct_cases; ++i) {
    const auto r = one_case<F, T>(rs);
    for (std::size_t j = 0; j < r.size(); ++j) {
      differ += same_bits(r[j], ct[i][j]) ? 0 : 1;
      ++total;
    }
  }
  std::fprintf(stderr, "constant-vs-runtime %s %s %ld of %ld differ\n",
               names[F], scalar, differ, total);
}

template <class T>
void all(long cases, std::uint64_t seed, const char* scalar) {
  family<0, T>(cases, seed, scalar);
  family<1, T>(cases, seed + 1, scalar);
  family<2, T>(cases, seed + 2, scalar);
  family<3, T>(cases, seed + 3, scalar);
  family<4, T>(cases, seed + 4, scalar);
  family<5, T>(cases, seed + 5, scalar);
}

} // namespace

int main(int argc, char** argv) {
  if (argc != 3) {
    std::fprintf(stderr, "usage: cprobe <cases> <seed>\n");
    return 2;
  }
  const long cases = std::atol(argv[1]);
  const std::uint64_t seed = std::strtoull(argv[2], nullptr, 10);
  all<double>(cases, seed, "d");
  all<float>(cases, seed + 100, "f");
  return 0;
}
