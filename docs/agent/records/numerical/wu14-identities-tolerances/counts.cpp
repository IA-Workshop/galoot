// Tracked-oracle counts for the floating identities of WU14
// (wu14-identities-tolerances.md). Runs every computation over
// test_support::tracked only, with no double run, and prints to stdout, per
// identity: whether the two exact sides agree, the range of K of each side,
// the largest tolerance gamma_KL M_L + gamma_KR M_R in units of u and
// relative to the largest |exact| of its sample, how many exact
// coefficients need more than 53 bits (so that double must round), and the
// largest bits of the magnitudes' numerators and denominators (the int64
// headroom). On stderr: the name of each identity as it starts, and the
// largest magnitude of conformal(3)'s compose(q2, q1), which must stay far
// below 2^53 for the double compose to be exact.
//
// Build and run from the repository root (a single compile, not a pool step):
//   c++ -std=c++23 -O1 -ffp-contract=off -I include -I test \
//     docs/agent/records/numerical/wu14-identities-tolerances/counts.cpp \
//     -o counts && ./counts > counts.out 2> counts.err
// Seed 20261108. std::uniform_int_distribution is not specified across
// standard libraries; counts.out is from libstdc++ (GCC 16.2.1).
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <map>
#include <random>
#include <string>

#include <galoot/core.hpp>
#include <support/tracked.hpp>

namespace core = galoot::core;
using test_support::rational;
using test_support::tracked;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout =
    core::blades<rga3, "e423 e431 e412 e321">;
inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
inline constexpr auto flector_layout =
    core::blades<rga3, "e1 e2 e3 e4 e423 e431 e412 e321">;

template <class Alg, auto L>
using T = core::mv<Alg, L, tracked>;

std::mt19937_64 source(20261108);

rational draw(int bits, std::int64_t bound) {
  std::uniform_int_distribution<std::int64_t> p(-(bound - 1), bound - 1);
  return rational{p(source), test_support::power_of_two(bits)};
}

template <class X>
X random_mv(int bits, std::int64_t bound) {
  X x{};
  for (std::size_t k = 0; k < X::component_count; ++k) {
    x.coefficients.at(k) = tracked::input(draw(bits, bound));
  }
  return x;
}

// A tracked result turned into an input: same exact values, K = 0.
template <class X>
X as_input(const X& y) {
  X x{};
  for (std::size_t k = 0; k < X::component_count; ++k) {
    x.coefficients.at(k) = tracked::input(y.coefficients.at(k).value);
  }
  return x;
}

int bits_of(std::int64_t v) {
  const auto m = static_cast<std::uint64_t>(v < 0 ? -v : v);
  return 64 - std::countl_zero(m);
}

// Significant bits of a dyadic rational: of the odd numerator.
int significand_bits(const rational& r) {
  std::int64_t n = r.numerator;
  if (n == 0) return 0;
  while (n % 2 == 0) n /= 2;
  return bits_of(n);
}

double to_double(const rational& r) {
  return static_cast<double>(r.numerator) / static_cast<double>(r.denominator);
}

struct stats {
  int samples = 0;
  int coefficients = 0;
  int disagree = 0;
  int kl_max = 0, kr_max = 0, kl_min = 1 << 30, kr_min = 1 << 30;
  double tol_u_max = 0;     // max (gamma_KL M_L + gamma_KR M_R) / u
  double tol_rel_max = 0;   // max over coefficients of that tol / max|e| in the sample
  double tol_rel_med_acc = 0;
  int must_round = 0;       // exact value needs more than 53 bits
  int mag_num_bits = 0, mag_den_bits = 0, val_den_bits = 0;
  std::map<std::pair<int, int>, int> k_pairs;
};

std::map<std::string, stats> all;

struct entry {
  tracked t;
  bool present = false;
};

template <class X>
std::map<unsigned, entry> by_mask(const X& x) {
  std::map<unsigned, entry> m;
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const auto blade = X::layout.blades.at(k);
    const tracked t = blade.sign < 0 ? -x.coefficients.at(k) : x.coefficients.at(k);
    m[static_cast<unsigned>(blade.mask)] = {t, true};
  }
  return m;
}

double gamma_over_u(int k) {
  const double u = 0x1p-53;
  return k / (1.0 - k * u);
}

template <class L, class R>
void note(const std::string& name, const L& l, const R& r) {
  if (all.find(name) == all.end()) std::fprintf(stderr, "start %s\n", name.c_str());
  stats& s = all[name];
  ++s.samples;
  auto ml = by_mask(l);
  auto mr = by_mask(r);
  std::map<unsigned, std::pair<entry, entry>> joined;
  for (auto& [m, e] : ml) joined[m].first = e;
  for (auto& [m, e] : mr) joined[m].second = e;
  double emax = 0;
  for (auto& [m, p] : joined) {
    const rational el = p.first.present ? p.first.t.value : rational{};
    emax = std::max(emax, std::abs(to_double(el)));
  }
  for (auto& [m, p] : joined) {
    const tracked a = p.first.present ? p.first.t : tracked{};
    const tracked b = p.second.present ? p.second.t : tracked{};
    ++s.coefficients;
    if (a.value != b.value) ++s.disagree;
    s.kl_max = std::max(s.kl_max, a.roundings);
    s.kr_max = std::max(s.kr_max, b.roundings);
    s.kl_min = std::min(s.kl_min, a.roundings);
    s.kr_min = std::min(s.kr_min, b.roundings);
    s.k_pairs[{a.roundings, b.roundings}]++;
    const double tol_u = gamma_over_u(a.roundings) * to_double(a.magnitude) +
                         gamma_over_u(b.roundings) * to_double(b.magnitude);
    s.tol_u_max = std::max(s.tol_u_max, tol_u);
    if (emax > 0) s.tol_rel_max = std::max(s.tol_rel_max, tol_u / emax);
    if (significand_bits(a.value) > 53) ++s.must_round;
    for (const tracked* t : {&a, &b}) {
      s.mag_num_bits = std::max(s.mag_num_bits, bits_of(t->magnitude.numerator));
      s.mag_den_bits = std::max(s.mag_den_bits, bits_of(t->magnitude.denominator) - 1);
      s.val_den_bits = std::max(s.val_den_bits, bits_of(t->value.denominator) - 1);
    }
  }
}

template <class Alg, auto Layout>
void associativity(const std::string& where, int samples, int bits) {
  using X = T<Alg, Layout>;
  for (int i = 0; i < samples; ++i) {
    const X a = random_mv<X>(bits, std::int64_t{1} << bits);
    const X b = random_mv<X>(bits, std::int64_t{1} << bits);
    const X c = random_mv<X>(bits, std::int64_t{1} << bits);
    note(where + " (a*b)*c vs a*(b*c)", (a * b) * c, a * (b * c));
    note(where + " (a%b)%c vs a%(b%c)", (a % b) % c, a % (b % c));
  }
}

template <class Alg, auto Layout>
void de_morgan(const std::string& where, int samples) {
  using X = T<Alg, Layout>;
  using core::left_complement;
  using core::right_complement;
  for (int i = 0; i < samples; ++i) {
    const X a = random_mv<X>(27, 1 << 27);
    const X b = random_mv<X>(27, 1 << 27);
    note(where + " right(a*b) vs right(a)%right(b)", right_complement(a * b),
         right_complement(a) % right_complement(b));
    note(where + " right(a^b) vs right(a)&right(b)", right_complement(a ^ b),
         right_complement(a) & right_complement(b));
    note(where + " left(a*b) vs left(a)%left(b)", left_complement(a * b),
         left_complement(a) % left_complement(b));
    note(where + " left(a^b) vs left(a)&left(b)", left_complement(a ^ b),
         left_complement(a) & left_complement(b));
    note(where + " right(a%b) vs right(a)*right(b)", right_complement(a % b),
         right_complement(a) * right_complement(b));
    note(where + " right(a&b) vs right(a)^right(b)", right_complement(a & b),
         right_complement(a) ^ right_complement(b));
    note(where + " left(a%b) vs left(a)*left(b)", left_complement(a % b),
         left_complement(a) * left_complement(b));
    note(where + " left(a&b) vs left(a)^left(b)", left_complement(a & b),
         left_complement(a) ^ left_complement(b));
  }
}

// rigid(3): every motor and flector layout acts with a dropped part that is
// zero as a polynomial, so any coefficients compose exactly.
template <auto Q2L, auto Q1L, auto XL>
void rigid_composition(const std::string& where, int samples, int qbits,
                       int xbits) {
  using Q2 = T<rga3, Q2L>;
  using Q1 = T<rga3, Q1L>;
  using X = T<rga3, XL>;
  for (int i = 0; i < samples; ++i) {
    const Q2 q2 = random_mv<Q2>(qbits, std::int64_t{1} << qbits);
    const Q1 q1 = random_mv<Q1>(qbits, std::int64_t{1} << qbits);
    const X x = random_mv<X>(xbits, std::int64_t{1} << xbits);
    note(where, core::transform(x, core::compose(q2, q1)),
         core::transform(core::transform(x, q1), q2));
  }
}

// conformal(3): versors as antiproducts of two integer hyperplanes, made
// exact inputs, so that the double run holds the same exact versors.
template <auto XL>
void conformal_composition(const std::string& where, int samples, int hrange,
                           int xbits) {
  using H = T<cga3, core::of_grade<5, 4>>;
  using X = T<cga3, XL>;
  std::uniform_int_distribution<std::int64_t> hcoef(-hrange, hrange);
  auto hyperplane = [&] {
    H h{};
    for (std::size_t k = 0; k < H::component_count; ++k) {
      h.coefficients.at(k) = tracked::input(rational{hcoef(source)});
    }
    return h;
  };
  for (int i = 0; i < samples; ++i) {
    const auto q1 = as_input(hyperplane() % hyperplane());
    const auto q2 = as_input(hyperplane() % hyperplane());
    const X x = random_mv<X>(xbits, std::int64_t{1} << xbits);
    static int compose_bits = 0;
    const auto v = core::compose(q2, q1);
    for (const auto& t : v.coefficients) {
      if (bits_of(t.magnitude.numerator) > compose_bits) {
        compose_bits = bits_of(t.magnitude.numerator);
        std::fprintf(stderr, "compose magnitude bits %d (den %lld)\n", compose_bits,
                     (long long)t.magnitude.denominator);
      }
    }
    note(where, core::transform(x, core::compose(q2, q1)),
         core::transform(core::transform(x, q1), q2));
  }
}

int main() {
  associativity<rga3, core::full<4>>("rigid(3) full", 200, 18);
  associativity<cga3, core::full<5>>("conformal(3) full", 40, 18);
  associativity<cga3, core::full<5>>("conformal(3) full 2^-17", 200, 17);
  de_morgan<rga3, core::full<4>>("rigid(3) full", 200);
  de_morgan<cga3, core::full<5>>("conformal(3) full", 40);
  rigid_composition<motor_layout, motor_layout, point_layout>(
      "rigid(3) motor.motor on point", 200, 7, 26);
  rigid_composition<motor_layout, motor_layout, line_layout>(
      "rigid(3) motor.motor on line", 200, 7, 26);
  rigid_composition<motor_layout, motor_layout, plane_layout>(
      "rigid(3) motor.motor on plane", 200, 7, 26);
  rigid_composition<flector_layout, flector_layout, point_layout>(
      "rigid(3) flector.flector on point", 200, 7, 26);
  rigid_composition<motor_layout, flector_layout, point_layout>(
      "rigid(3) motor.flector on point", 200, 7, 26);
  rigid_composition<flector_layout, motor_layout, line_layout>(
      "rigid(3) flector.motor on line", 200, 7, 26);
  conformal_composition<core::of_grade<5, 1>>(
      "conformal(3) (h.h).(h.h) on vector", 100, 2, 30);
  conformal_composition<core::of_grade<5, 1>>(
      "conformal(3) (h.h).(h.h) on vector x 2^-42", 300, 2, 42);
  conformal_composition<core::of_grade<5, 2>>(
      "conformal(3) (h.h).(h.h) on bivector x 2^-42", 300, 2, 42);
  conformal_composition<core::of_grade<5, 2>>(
      "conformal(3) (h.h).(h.h) on bivector", 100, 2, 30);
  for (auto& [name, s] : all) {
    std::printf("%s\n  samples %d coefficients %d disagree %d\n", name.c_str(),
                s.samples, s.coefficients, s.disagree);
    std::printf("  K left %d..%d right %d..%d\n", s.kl_min, s.kl_max, s.kr_min,
                s.kr_max);
    std::printf("  max tol/u %.4g  max tol/(u max|e|) %.4g\n", s.tol_u_max,
                s.tol_rel_max);
    std::printf("  exact values needing >53 bits: %d of %d\n", s.must_round,
                s.coefficients);
    std::printf("  max bits: magnitude numerator %d, magnitude denominator 2^%d, "
                "value denominator 2^%d\n",
                s.mag_num_bits, s.mag_den_bits, s.val_den_bits);
    std::printf("  (K_L, K_R) pairs:");
    int shown = 0;
    for (auto& [k, n] : s.k_pairs) {
      if (shown++ < 12) std::printf(" (%d,%d)x%d", k.first, k.second, n);
    }
    std::printf("%s\n", shown > 12 ? " ..." : "");
  }
}
