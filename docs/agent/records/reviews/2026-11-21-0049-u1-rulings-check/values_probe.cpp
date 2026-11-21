// Reviewer's own probe of the values on records/docs-design-rulings, with
// layouts in other orders than the branch's probe, and the rigid(3) claim of
// the card's section.
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
struct rg {
  static constexpr auto value = core::rigid<3>();
};
using V3 = core::mv<e3, core::blades<e3, "e3 e1 e2">, int>;
using All = core::mv<e3, core::blades<e3, "e123 e23 e31 e12 e3 e2 e1 1">, int>;

constexpr All e(int i1, int i2, int i3) { return All{0, 0, 0, 0, i3, i2, i1, 0}; }

// running-text line: (e23 + e31) antiproduct e23 = e123 - e3
constexpr All plane_g{0, 1, 0, 0, 0, 0, 0, 0};
constexpr All plane_h{0, 1, 1, 0, 0, 0, 0, 0};
static_assert(All{!e(1, 0, 0)} == plane_g);
static_assert(All{!e(1, 1, 0)} == plane_h);
static_assert(All{plane_h % plane_g} == All{1, 0, 0, 0, -1, 0, 0, 0});
static_assert(All{e(1, 1, 0) * e(1, 0, 0)} == All{0, 0, 0, -1, 0, 0, 0, 1});
static_assert(All{!All{0, 0, 0, -1, 0, 0, 0, 1}} == All{1, 0, 0, 0, -1, 0, 0, 0});

// text-block line: (e123 - e3) % e3 % (e123 + e3) = 2 e3, both groupings
constexpr All w{1, 0, 0, 0, -1, 0, 0, 0};
static_assert(All{core::antireverse(w)} == All{1, 0, 0, 0, 1, 0, 0, 0});
static_assert(All{(w % e(0, 0, 1)) % core::antireverse(w)} == e(0, 0, 2));
static_assert(All{w % (e(0, 0, 1) % core::antireverse(w))} == e(0, 0, 2));

// T1 to T5, vectors on "e3 e1 e2" (so (x, y, z) is V3{z, x, y})
using Even = core::mv<e3, core::blades<e3, "e12 1">, int>;
constexpr Even r{-1, 1};  // 1 - e12
constexpr Even s{-1, 2};  // 2 - e12
static_assert(core::truncate<Even>(V3{0, 2, 1} * V3{0, 1, 0}) == s);
constexpr V3 sandwich(const Even& q, const V3& v) {
  return core::truncate<V3>(q * v * ~q);
}
static_assert(sandwich(r, V3{1, 0, 0}) == V3{2, 0, 0});
static_assert(core::transform(V3{1, 0, 0}, r) == V3{-2, 0, 0});
static_assert(core::transform(V3{1, 0, 0}, !r) == V3{2, 0, 0});
static_assert(sandwich(r, V3{0, 1, 0}) == V3{0, 0, 2});
static_assert(core::transform(V3{0, 1, 0}, r) == V3{0, 0, 2});
static_assert(sandwich(s, V3{5, 2, -3}) == V3{25, 18, -1});
static_assert(core::transform(V3{5, 2, -3}, s) == V3{-25, 6, 17});
static_assert(core::transform(V3{5, 2, -3}, !s) == V3{25, 18, -1});
static_assert(sandwich(s, V3{0, 1, 0}) == V3{0, 3, 4});
static_assert(core::transform(V3{0, 1, 0}, s) == V3{0, -3, 4});
static_assert(core::transform(V3{0, 1, 0}, !s) == V3{0, 3, 4});
constexpr V3 mirror{0, 1, 0};
constexpr V3 x5{3, 1, 2};
static_assert(core::truncate<V3>(-(mirror * x5 * mirror)) == V3{3, -1, 2});
static_assert(core::transform(x5, mirror) == V3{-3, 1, -2});
static_assert(core::transform(x5, !mirror) == V3{3, -1, 2});

// rigid(3): s complemented moves the point (2, -3, 5) with weight 1 to
// (18, -1, 25) with weight 5 = s s~.
using RS = core::mv<rg, core::blades<rg, "1 e12">, int>;
using Pt = core::mv<rg, core::blades<rg, "e1 e2 e3 e4">, int>;
static_assert(core::transform(Pt{2, -3, 5, 1}, !RS{2, -1}) == Pt{18, -1, 25, 5});

} // namespace

int main() {}
