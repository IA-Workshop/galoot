// One function per operation, each one expression on the rga3 preset's
// double types, so that the code the compiler generates for an operation
// can be read on its own. rga3.kernels_inline compiles this file at -O2
// without the checking mode and fails if any of these functions calls or
// jumps to a function of the library: an operation compiles to the
// arithmetic of its terms in place, with no call left behind.
//
// The first seven are the operations of the benchmark programs beside this
// file. The next four reach the parts of the library those seven do not: a
// sum, a unary map, a dot product, and a change of basis. The rest call each
// public operation by its name and by its operator, on the rga3 preset's
// motors and flectors and on two multivectors of all sixteen blades, whose
// products are the largest of the algebra: a compiler that keeps a large
// operation out of line keeps the call at the user's side, where only such
// a probe sees it.
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
using Flector = rga3::Flector<double>;
using Full = core::mv<rga3::algebra, core::full<4>, double>;

namespace {

// A change of rigid(3) to itself that doubles e4, so that its image of a
// point is the point with its e4 coefficient doubled. Maybe unused, since a
// unit that holds one function alone uses it only for a change of basis.
struct doubled_e4 {
  using source = rga3::algebra;
  using target = rga3::algebra;
  [[maybe_unused]] static constexpr auto value = core::from_rows(
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

auto geometric_product_motors(const Motor& a, const Motor& b) {
  return core::geometric_product(a, b);
}

auto times_motors(const Motor& a, const Motor& b) { return a * b; }

auto geometric_product_full(const Full& a, const Full& b) {
  return core::geometric_product(a, b);
}

auto times_full(const Full& a, const Full& b) { return a * b; }

auto wedge_full(const Full& a, const Full& b) { return core::wedge(a, b); }

auto caret_full(const Full& a, const Full& b) { return a ^ b; }

auto antiproduct_motors(const Motor& a, const Motor& b) {
  return core::geometric_antiproduct(a, b);
}

auto antiproduct_motor_flector(const Motor& a, const Flector& b) {
  return core::geometric_antiproduct(a, b);
}

auto percent_motor_flector(const Motor& a, const Flector& b) { return a % b; }

auto antiproduct_full(const Full& a, const Full& b) {
  return core::geometric_antiproduct(a, b);
}

auto percent_full(const Full& a, const Full& b) { return a % b; }

auto antiwedge_full(const Full& a, const Full& b) {
  return core::antiwedge(a, b);
}

auto ampersand_full(const Full& a, const Full& b) { return a & b; }

auto reverse_full(const Full& a) { return core::reverse(a); }

auto tilde_full(const Full& a) { return ~a; }

auto antireverse_full(const Full& a) { return core::antireverse(a); }

auto grade_involution_full(const Full& a) { return core::grade_involution(a); }

auto right_complement_full(const Full& a) { return core::right_complement(a); }

auto bang_full(const Full& a) { return !a; }

auto left_complement_full(const Full& a) { return core::left_complement(a); }

auto plus_full(const Full& a, const Full& b) { return a + b; }

auto minus_full(const Full& a, const Full& b) { return a - b; }

auto bulk_full(const Full& a) { return core::bulk(a); }

auto weight_full(const Full& a) { return core::weight(a); }

auto bulk_dual_full(const Full& a) { return core::bulk_dual(a); }

auto weight_dual_full(const Full& a) { return core::weight_dual(a); }

auto left_bulk_dual_full(const Full& a) { return core::left_bulk_dual(a); }

auto left_weight_dual_full(const Full& a) { return core::left_weight_dual(a); }

auto dot_full(const Full& a, const Full& b) { return core::dot(a, b); }

auto antidot_full(const Full& a, const Full& b) { return core::antidot(a, b); }

auto project_point_plane(const Point& p, const Plane& g) {
  return core::project(p, g);
}

auto antiproject_plane_point(const Plane& g, const Point& p) {
  return core::antiproject(g, p);
}

auto change_full(const Full& a) { return core::change_basis<doubled_e4>(a); }

auto change_back_full(const Full& a) {
  return core::change_basis_back<doubled_e4>(a);
}

Full convert_motor(const Motor& m) { return Full{m}; }

auto truncate_full(const Full& a) { return core::truncate<Motor>(a); }

auto part_full(const Full& a) { return core::part<Motor>(a); }
// NOLINTEND(misc-use-internal-linkage)
