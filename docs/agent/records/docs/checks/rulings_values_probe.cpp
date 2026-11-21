// The values that the design records gained with the rulings on notation and
// on transform: the antiproduct lines of the notation key and the authoring
// rules, and the rotor 2 - e12 that tells transform from the classical
// sandwich with no coincidence of a quarter turn. Each is derived by hand in
// the record that quotes it; this file confirms that the library computes it.
// Over int, so every claim is a static_assert.
//
//   g++ -std=c++23 -I include -fsyntax-only rulings_values_probe.cpp
//   clang++ -std=c++23 -I include -fsyntax-only rulings_values_probe.cpp

#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Vector = core::mv<e3, core::blades<e3, "e1 e2 e3">, int>;
using Bivector = core::mv<e3, core::blades<e3, "e23 e31 e12">, int>;
using Rotor = core::mv<e3, core::blades<e3, "1 e12 e13 e23">, int>;
using Odd = core::mv<e3, core::blades<e3, "e1 e2 e3 e123">, int>;
using Full =
    core::mv<e3, core::blades<e3, "1 e1 e2 e3 e12 e13 e23 e123">, int>;

constexpr Vector turned(const Rotor& r, const Vector& v) {
  return core::truncate<Vector>(r * v * ~r);
}

// The mirrors of chapter 6 as planes, and their antiproduct:
// (e23 + e31) antiproduct e23 = e123 - e3, the complement of 1 - e12.
constexpr Bivector g{!Vector{1, 0, 0}};
constexpr Bivector h{!Vector{1, 1, 0}};
static_assert(g == Bivector{1, 0, 0});
static_assert(h == Bivector{1, 1, 0});
static_assert(Odd{h % g} == Odd{0, 0, -1, 1});
static_assert(Odd{h % g} == Odd{!Rotor{1, -1, 0, 0}});

// (e123 - e3) % e3 % (e123 + e3) = 2 e3, which is transform(e3, e123 - e3).
constexpr Odd versor{0, 0, -1, 1};
constexpr Vector x3{0, 0, 1};
static_assert(core::antireverse(versor) == Odd{0, 0, 1, 1});
static_assert(Full{versor % x3 % core::antireverse(versor)} ==
              Full{0, 0, 0, 2, 0, 0, 0, 0});
static_assert(core::transform(x3, versor) == Vector{0, 0, 2});

// a % b = left(right(a) * right(b)) on one pair that are not blades.
constexpr Full a{1, 2, 0, 0, 0, 0, -3, 1};
constexpr Full b{0, 0, 1, -1, 0, 4, 0, 0};
static_assert(Full{a % b} ==
              Full{core::left_complement(core::right_complement(a) *
                                         core::right_complement(b))});

// s = (2, 1, 0)(1, 0, 0) = 2 - e12, cosine 3/5 and sine 4/5, factor 5, on
// (2, -3, 5): the sandwich gives (18, -1, 25), transform(v, s) gives
// (6, 17, -25), and transform(v, !s) gives the sandwich.
constexpr Rotor s{Vector{2, 1, 0} * Vector{1, 0, 0}};
constexpr Vector v{2, -3, 5};
static_assert(s == Rotor{2, -1, 0, 0});
static_assert(turned(s, v) == Vector{18, -1, 25});
static_assert(core::transform(v, s) == Vector{6, 17, -25});
static_assert(Odd{!s} == Odd{0, 0, -1, 2});
static_assert(core::transform(v, !s) == Vector{18, -1, 25});

// The quarter turn's coincidence: r = 1 - e12 agrees on (1, 0, 0) and not on
// (0, 0, 1).
constexpr Rotor r{1, -1, 0, 0};
static_assert(turned(r, Vector{1, 0, 0}) == Vector{0, 2, 0});
static_assert(core::transform(Vector{1, 0, 0}, r) == Vector{0, 2, 0});
static_assert(core::transform(x3, r) == Vector{0, 0, -2});
static_assert(turned(r, x3) == Vector{0, 0, 2});

} // namespace

int main() {}
