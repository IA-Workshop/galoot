// Exact checks of the forms the benchmarks time, over the test rational:
//
// - for motors with random rational coefficients, which need not be
//   unitized, forms a, b and bp equal the sandwich Q v x v antireverse(Q),
//   computed from the library's two full antiproducts, on x's blades (the
//   sandwich has no part on other blades in rigid(3)), and equal
//   galoot::core::transform; form c differs, and the program counts how
//   often;
// - for unitized motors that satisfy the geometric constraint, built exactly
//   as a rotor composed with a translator, all four forms equal the
//   sandwich;
// - in conformal(3), for a motor on e415 e425 e435 e12345 e235 e315 e125 e5
//   with random coefficients and x of each grade 1 to 4, the collection
//   pass gives what galoot::core::transform gives.
//
// It prints each count and the collection pass's operation counts, and
// exits 1 on any failure.
#include <array>
#include <cstddef>
#include <cstdint>
#include <print>
#include <random>

#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>

#include <support/rational.hpp>

#include "../proto/collect.hpp"
#include "forms.hpp"

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using test_support::rational;
using Motor = rga3::Motor<rational>;

int failures = 0;

rational small(std::mt19937_64& rng, std::int64_t top, std::int64_t below) {
  const auto span = static_cast<std::uint64_t>((2 * top) + 1);
  const auto n = static_cast<std::int64_t>(rng() % span) - top;
  const auto d =
      static_cast<std::int64_t>(rng() % static_cast<std::uint64_t>(below)) + 1;
  return rational{n, d};
}

template <std::size_t N>
std::array<rational, N> random_array(std::mt19937_64& rng) {
  std::array<rational, N> a{};
  for (rational& c : a) {
    c = small(rng, 4, 3);
  }
  return a;
}

// A rotor on a rational point of the unit 3-sphere (the stereographic image
// of (a, b, c)), composed with a translator by half of (tx, ty, tz).
Motor unit_motor(std::mt19937_64& rng) {
  const rational a = small(rng, 2, 2);
  const rational b = small(rng, 2, 2);
  const rational c = small(rng, 2, 2);
  const rational one{1};
  const rational two{2};
  const rational n2 = (a * a) + (b * b) + (c * c);
  const rational k = one / (one + n2);
  const Motor rotor{two * a * k, two * b * k, two * c * k, (n2 - one) * k,
                    rational{}, rational{}, rational{}, rational{}};
  const Motor translator{rational{}, rational{},       rational{},
                         one,        small(rng, 3, 2), small(rng, 3, 2),
                         small(rng, 3, 2), rational{}};
  return Motor{core::compose(translator, rotor)};
}

// The sandwich read on X's blades, with X's orientations; `outside` is set
// when it has a nonzero coefficient on another blade.
template <class X, class V>
std::array<rational, X::component_count> sandwich(const X& x, const V& v,
                                                  bool& outside) {
  const auto whole = core::geometric_antiproduct(
      core::geometric_antiproduct(v, x), core::antireverse(v));
  using W = decltype(whole);
  std::array<rational, X::component_count> y{};
  outside = false;
  for (std::size_t k = 0; k < W::component_count; ++k) {
    const auto blade = W::layout.blades[k];
    bool found = false;
    for (std::size_t i = 0; i < X::component_count; ++i) {
      if (X::layout.blades[i].mask == blade.mask) {
        y[i] = whole.coefficients[k] *
               rational{blade.sign * X::layout.blades[i].sign};
        found = true;
      }
    }
    outside = outside || (!found && whole.coefficients[k] != rational{});
  }
  return y;
}

template <class Form, std::size_t N>
std::array<rational, N> run(const std::array<rational, 8>& q,
                            const std::array<rational, N>& x) {
  return Form::apply(Form::setup(q), q, x);
}

void expect(bool ok, const char* what) {
  if (!ok) {
    std::println("FAIL {}", what);
    ++failures;
  }
}

template <class X, class A, class B, class C>
void check_rigid(const char* name, std::mt19937_64& rng, int draws) {
  constexpr std::size_t n = X::component_count;
  int c_differs = 0;
  for (int i = 0; i < draws; ++i) {
    for (const bool unit : {false, true}) {
      Motor v;
      if (unit) {
        v = unit_motor(rng);
      } else {
        v.coefficients = random_array<8>(rng);
      }
      X x;
      x.coefficients = random_array<n>(rng);
      bool outside = false;
      const auto s = sandwich(x, v, outside);
      const auto& q = v.coefficients;
      const auto bp = collect::apply<rga3::algebra, rga3::motor_layout,
                                     X::layout>(
          collect::setup<rga3::algebra, rga3::motor_layout, X::layout>(q),
          x.coefficients);
      expect(!outside, "the sandwich has a part off x's blades");
      expect(core::transform(x, v).coefficients == s, "library != sandwich");
      expect(run<A>(q, x.coefficients) == s, "form a != sandwich");
      expect(run<B>(q, x.coefficients) == s, "form b != sandwich");
      expect(bp == s, "form bp != sandwich");
      if (unit) {
        expect(run<C>(q, x.coefficients) == s,
               "form c != sandwich on a unit motor");
      } else {
        c_differs += run<C>(q, x.coefficients) != s ? 1 : 0;
      }
    }
  }
  constexpr auto k =
      collect::count<rga3::algebra, rga3::motor_layout, X::layout>();
  std::println("{}: {} random and {} unit motors; forms a, b, bp equal the "
               "sandwich on all, c on every unit motor, c differs on {} of "
               "{} random motors",
               name, draws, draws, c_differs, draws);
  std::println("  pass: {} composed terms; setup mul {} add {} neg {} scl {} "
               "literal {}; apply mul {} add {} neg {}",
               k.composed, k.setup_mul, k.setup_add, k.setup_neg, k.setup_scl,
               k.setup_literal, k.apply_mul, k.apply_add, k.apply_neg);
}

struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
constexpr auto cga3_motor =
    core::blades<cga3, "e415 e425 e435 e12345 e235 e315 e125 e5">;

template <std::size_t Grade>
void check_conformal(std::mt19937_64& rng, int draws) {
  constexpr auto layout = core::of_grade<5, Grade>;
  using X = core::mv<cga3, layout, rational>;
  using V = core::mv<cga3, cga3_motor, rational>;
  int same = 0;
  for (int i = 0; i < draws; ++i) {
    X x;
    x.coefficients = random_array<X::component_count>(rng);
    V v;
    v.coefficients = random_array<8>(rng);
    const auto y = collect::apply<cga3, cga3_motor, layout>(
        collect::setup<cga3, cga3_motor, layout>(v.coefficients),
        x.coefficients);
    same += y == core::transform(x, v).coefficients ? 1 : 0;
  }
  expect(same == draws, "conformal(3): the pass != transform");
  constexpr auto k = collect::count<cga3, cga3_motor, layout>();
  std::println("conformal(3) motor, x of grade {} ({} blades): the pass "
               "equals transform on {} of {}",
               Grade, X::component_count, same, draws);
  std::println("  pass: {} composed terms; setup mul {} add {} neg {} scl {} "
               "literal {}; apply mul {} add {} neg {}",
               k.composed, k.setup_mul, k.setup_add, k.setup_neg, k.setup_scl,
               k.setup_literal, k.apply_mul, k.apply_add, k.apply_neg);
}

} // namespace

int main() {
  std::mt19937_64 rng{20261110};
  constexpr int draws = 200;
  check_rigid<rga3::Point<rational>, forms::transform_point_a<rational>,
              forms::transform_point_b<rational>,
              forms::transform_point_c<rational>>("point", rng, draws);
  check_rigid<rga3::Line<rational>, forms::transform_line_a<rational>,
              forms::transform_line_b<rational>,
              forms::transform_line_c<rational>>("line", rng, draws);
  check_rigid<rga3::Plane<rational>, forms::transform_plane_a<rational>,
              forms::transform_plane_b<rational>,
              forms::transform_plane_c<rational>>("plane", rng, draws);
  check_conformal<1>(rng, 50);
  check_conformal<2>(rng, 50);
  check_conformal<3>(rng, 50);
  check_conformal<4>(rng, 50);
  std::println("failures: {}", failures);
  return failures == 0 ? 0 : 1;
}
