// Spike C: the five misuse cases of plan 0001 section 9, as a user would
// write them against fixture.hpp. One case per compile:
//
//   -DSPIKE_CASE=10  none: the correct uses compile (control)
//   -DSPIKE_CASE=1   gp of operands from two different algebras
//   -DSPIKE_CASE=21  get<"e1"> of a Line: a valid blade not in the set
//   -DSPIKE_CASE=22  a blade set declared with "e5" in rga3 (n = 4)
//   -DSPIKE_CASE=23  get<"e14"> of a Line, which holds e41 = -e14
//   -DSPIKE_CASE=31  implicit lossy conversion, Point p = motor
//   -DSPIKE_CASE=32  explicit lossy conversion, static_cast<Point>(motor)
//   -DSPIKE_CASE=4   transform by a versor of mixed parity
//   -DSPIKE_CASE=5   gp over a scalar type without multiplication
//   -DSPIKE_CASE=9   detection: what requires-expressions report per form
//
// with -DSPIKE_FORM=1..6 (R, K, D, G, A, P; see fixture.hpp). Run by
// run_diag.py, which passes -I to records/spikes/spike-a for gen.hpp.
#include "fixture.hpp"

#include <cstdint>
#include <type_traits>

using namespace cdf;
using namespace cdf::rga3_types;

using R400Vector = mv<r400, blades_of<r400, "e1", "e2", "e3", "e4">>;

// A scalar with addition but no multiplication, as a unit-carrying length
// would have: length times length is an area, not a length.
struct length {
  double m = 0;
  friend constexpr length operator+(length a, length b) { return {a.m + b.m}; }
  friend constexpr length operator-(length a, length b) { return {a.m - b.m}; }
  explicit constexpr length(std::int64_t k) : m(static_cast<double>(k)) {}
  constexpr length() = default;
  constexpr length(double v) : m(v) {}
};

#ifndef SPIKE_CASE
#error "define SPIKE_CASE"
#endif

double use(const Point& p, const Line& l, const Motor& q) {
#if SPIKE_CASE == 10
  const auto pp = gp(p, p);
  const Motor m{};
  const auto t = transform(p, m);
  const auto s = static_cast<Motor>(l);
  return pp.c[0] + t.c[0] + get<"e41">(l) + s.c[0] + get<"1">(q);
#elif SPIKE_CASE == 1
  const R400Vector v{};
  const auto r = gp(p, v);
  return r.c[0] + l.c[0] + q.c[0];
#elif SPIKE_CASE == 21
  return get<"e1">(l) + p.c[0] + q.c[0];
#elif SPIKE_CASE == 22
  using Bad = mv<rga3, blades_of<rga3, "e1", "e5">>;
  const Bad b{};
  return b.c[0] + p.c[0] + l.c[0] + q.c[0];
#elif SPIKE_CASE == 23
  return get<"e14">(l) + p.c[0] + q.c[0];
#elif SPIKE_CASE == 31
  const Point r = q;
  return r.c[0] + p.c[0] + l.c[0];
#elif SPIKE_CASE == 32
  const auto r = static_cast<Point>(q);
  return r.c[0] + p.c[0] + l.c[0];
#elif SPIKE_CASE == 4
  using Mixed = mv<rga3, blades_of<rga3, "1", "e1">>;
  const Mixed v{};
  const auto r = transform(p, v);
  return r.c[0] + l.c[0] + q.c[0];
#elif SPIKE_CASE == 5
  const Vector<length> a{};
  const auto r = gp(a, a);
  return r.c[0].m + p.c[0] + l.c[0] + q.c[0];
#elif SPIKE_CASE == 9
  return p.c[0] + l.c[0] + q.c[0];
#endif
}

#if SPIKE_CASE == 9
// What generic code sees: a requires-expression or a type trait asks whether
// the misuse is well-formed without compiling it. Each line is true when the
// overload set answers "no". Printed through a deliberately failing
// static_assert of the packed answers so one compile reports all five.
template <class X, class Y>
constexpr bool can_gp = requires(const X& x, const Y& y) { gp(x, y); };
template <class X, class V>
constexpr bool can_transform =
    requires(const X& x, const V& v) { transform(x, v); };
template <class X>
constexpr bool can_get_e1 = requires(const X& x) { get<"e1">(x); };
using Mixed = mv<rga3, blades_of<rga3, "1", "e1">>;
constexpr int answers =
    (can_gp<Point, R400Vector> ? 1 : 0) + (can_gp<Vector<length>, Vector<length>> ? 2 : 0) +
    (can_get_e1<Line> ? 4 : 0) + (std::is_convertible_v<Motor, Point> ? 8 : 0) +
    (can_transform<Point, Mixed> ? 16 : 0) + (can_gp<Point, Point> ? 32 : 0);
template <int A> struct detection_answers;
detection_answers<answers> show; // incomplete type: prints the value
#endif

int main() { return use({}, {}, {}) == 0.0 ? 0 : 1; }
