// Independent probe: what transform gives for a classical rotor in R(3,0,0)
// and for motors of the rga3 preset. Expected values from check_transform.py.
#include <cstdio>
#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;

namespace {

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Vector = core::mv<e3, core::blades<e3, "e1 e2 e3">, int>;

constexpr Vector a{1, 0, 0};
constexpr Vector b{1, 1, 0};
constexpr auto r = b * a; // 1 - e12

constexpr Vector e1{1, 0, 0};
constexpr Vector e3v{0, 0, 1};
constexpr Vector w{1, 2, 3};

// The classical sandwich.
static_assert(core::truncate<Vector>(r * e1 * ~r) == Vector{0, 2, 0});
static_assert(core::truncate<Vector>(r * e3v * ~r) == Vector{0, 0, 2});
static_assert(core::truncate<Vector>(r * w * ~r) == Vector{-4, 2, 6});
// transform by r itself: v -> -(~r v r), the negated inverse rotation.
static_assert(core::transform(e1, r) == Vector{0, 2, 0});
static_assert(core::transform(e3v, r) == Vector{0, 0, -2});
static_assert(core::transform(w, r) == Vector{-4, 2, -6});
// transform by the right (or left) complement of r: the classical rotation.
static_assert(core::transform(w, core::right_complement(r)) == Vector{-4, 2, 6});
static_assert(core::transform(w, core::left_complement(r)) == Vector{-4, 2, 6});
static_assert(core::transform(e3v, !r) == Vector{0, 0, 2});

// rga3: a quarter turn about the z axis, e43 + antiscalar, lambda = 2.
constexpr rga3::Motor<int> quarter{0, 0, 1, 1, 0, 0, 0, 0};
static_assert(core::transform(rga3::Point<int>{1, 0, 0, 1}, quarter) ==
              rga3::Point<int>{0, 2, 0, 2});
static_assert(core::transform(rga3::Point<int>{0, 0, 1, 1}, quarter) ==
              rga3::Point<int>{0, 0, 2, 2});
// 3 L + 4 antiscalar, L through (1, 2, 0) along z (moment (2, -1, 0)),
// lambda = 25: turns by 2 phi with cos 2 phi = 7/25, sin 2 phi = 24/25.
constexpr rga3::Motor<int> offaxis{0, 0, 3, 4, 6, -3, 0, 0};
static_assert(core::transform(rga3::Point<int>{2, 2, 5, 1}, offaxis) ==
              rga3::Point<int>{32, 74, 125, 25});
// Twice translation(1, 3, -2): lambda = 4.
constexpr rga3::Motor<int> move{0, 0, 0, 2, 1, 3, -2, 0};
static_assert(core::transform(rga3::Point<int>{0, 0, 0, 1}, move) ==
              rga3::Point<int>{4, 12, -8, 4});
// Order of compose: offaxis first, then move.
static_assert(core::transform(rga3::Point<int>{2, 2, 5, 1},
                              rga3::Motor<int>{core::compose(move, offaxis)}) ==
              core::transform(core::transform(rga3::Point<int>{2, 2, 5, 1},
                                              offaxis),
                              move));

} // namespace

int main() {
  const auto t = core::transform(w, r);
  std::printf("transform((1,2,3), b a) = (%d, %d, %d)\n", t.coefficients[0],
              t.coefficients[1], t.coefficients[2]);
}
