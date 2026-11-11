// One function per operation, each one expression on the rga3 preset's
// double types, so that the code the compiler generates for an operation
// can be read on its own. rga3.kernels_inline compiles this file at -O2
// without the checking mode and fails if any of these functions calls or
// jumps to a function of the library: an operation compiles to the
// arithmetic of its terms in place, with no call left behind.
//
// The first seven are the operations of the benchmark programs beside this
// file. The other four reach the parts of the library those seven do not: a
// sum, a unary map, a dot product, and a change of basis.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/unary.hpp>
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;
using Plane = rga3::Plane<double>;
using Motor = rga3::Motor<double>;

namespace {

// A change of rigid(3) to itself that doubles e4, so that its image of a
// point is the point with its e4 coefficient doubled.
struct doubled_e4 {
  using source = rga3::algebra;
  using target = rga3::algebra;
  static constexpr auto value = core::from_rows(
      {
          {1, 0, 0, 0},
          {0, 1, 0, 0},
          {0, 0, 1, 0},
          {0, 0, 0, 2},
      },
      {1, 1, 1, 1});
};

} // namespace

// Each function has external linkage, so that the compiler emits its code
// although nothing in this file calls it.
// NOLINTBEGIN(misc-use-internal-linkage)
Line wedge_point_point(const Point& p, const Point& q) { return Line{p ^ q}; }

Plane wedge_point_point_point(const Point& p, const Point& q, const Point& r) {
  return Plane{(p ^ q) ^ r};
}

Point antiwedge_line_plane(const Line& l, const Plane& g) {
  return Point{l & g};
}

Point transform_point(const Point& x, const Motor& q) {
  return core::transform(x, q);
}

Line transform_line(const Line& x, const Motor& q) {
  return core::transform(x, q);
}

Plane transform_plane(const Plane& x, const Motor& q) {
  return core::transform(x, q);
}

Motor compose_motors(const Motor& a, const Motor& b) {
  return Motor{core::compose(a, b)};
}

Line sum_lines(const Line& l, const Line& m) { return l + m; }

Line reverse_line(const Line& l) { return core::reverse(l); }

auto dot_points(const Point& p, const Point& q) { return core::dot(p, q); }

auto change_point(const Point& p) { return core::change_basis<doubled_e4>(p); }
// NOLINTEND(misc-use-internal-linkage)
