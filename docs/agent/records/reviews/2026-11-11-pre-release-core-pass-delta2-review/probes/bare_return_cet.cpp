
#include <galoot/core/basis_change.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/unary.hpp>
#include <galoot/core/transform.hpp>
#include <galoot/presets/rga3.hpp>
namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;
using Plane = rga3::Plane<double>;
using Motor = rga3::Motor<double>;
using Flector = rga3::Flector<double>;
using Full = core::mv<rga3::algebra, core::full<4>, double>;
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct vga3 { static constexpr auto value = core::R<3, 0, 0>(); };
using CFull = core::mv<cga3, core::full<5>, double>;
using CVec = core::mv<cga3, core::of_grade<5, 1>, double>;
using CBiv = core::mv<cga3, core::of_grade<5, 2>, double>;
using CEven = core::mv<cga3, core::merge<core::merge<core::of_grade<5, 0>, core::of_grade<5, 2>>, core::of_grade<5, 4>>, double>;
using CMotor = core::mv<cga3, core::merge<core::merge<core::of_grade<5, 0>, core::of_grade<5, 2>>, core::of_grade<5, 4>>, double>;
using VFull = core::mv<vga3, core::full<3>, double>;
using VVec = core::mv<vga3, core::of_grade<3, 1>, double>;
using VRotor = core::mv<vga3, core::merge<core::of_grade<3, 0>, core::of_grade<3, 2>>, double>;
namespace {
struct doubled_e4 {
  using source = rga3::algebra;
  using target = rga3::algebra;
  [[maybe_unused]] static constexpr auto value = core::from_rows(
      {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 2}}, {1, 1, 1, 1});
};
}
auto dot_pp(const Point& a, const Plane& b) { return core::dot(a, b); }
