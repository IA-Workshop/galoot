#ifndef GALOOT_PRESETS_RGA3_OBJECTS_HPP
#define GALOOT_PRESETS_RGA3_OBJECTS_HPP

// The objects of rigid(3), the rigid geometric algebra of three dimensions:
// points, lines, and planes, and the motors and flectors that move them. The
// algebra is R(3, 0, 1): e1, e2, e3 square to 1 and e4, the projective
// generator, to 0. The layouts are those of Lengyel's rigid geometric algebra
// (rigidgeometricalgebra.org), so the coefficients of its published formulas
// copy over in the order and with the signs in which they are written:
//
//   using Point = galoot::presets::rga3::Point<double>;
//   using Line = galoot::presets::rga3::Line<double>;
//   Point p{0, 0, 0, 1};  // the origin
//   Point q{1, 0, 0, 1};
//   Line l{p ^ q};        // the x axis, from p to q: direction (1, 0, 0)
//
// A blade is named by its generators in the order of their product, so
// e41 = -e14, e31 = -e13, and e321 = -e123, and a coefficient written on e41
// multiplies -e14. The antiscalar is e1234. With <galoot/core/format.hpp>
// included, the types print by these names: the x axis written by its
// coefficients, std::format("{}", Line{1, 0, 0, 0, 0, 0}), gives
// {e41: 1, e42: 0, e43: 0, e23: 0, e31: 0, e12: 0}.
//
// The types are the core's multivectors, and every operation on them is the
// core's, found by argument-dependent lookup: wedge (^) joins, antiwedge (&)
// meets, transform moves an object by a motor or a flector, and compose
// chains motions. A result whose layout is not one of the preset's, such as
// p ^ q, converts to the named type explicitly (Line{p ^ q}).
//
// Under `using namespace std::ranges` or `using namespace std::views`, an
// unqualified transform(x, v) finds the function object std::ranges::transform
// or std::views::transform, which turns off argument-dependent lookup, so it
// does not compile; write galoot::core::transform(x, v).

#include <concepts>

#include <galoot/core.hpp> // IWYU pragma: export

namespace galoot::presets::rga3 {

/// The algebra of the preset, rigid(3) = R(3, 0, 1): e1, e2, e3 are
/// Euclidean and e4 is the null projective generator. Every type of the
/// preset is a multivector of this tag, so a diagnostic names it
/// galoot::presets::rga3::algebra.
struct algebra {
  /// The metric of rigid(3).
  static constexpr auto value = core::rigid<3>();

  /// The names of the sixteen blades, as Lengyel's rigid geometric algebra
  /// writes them: the order of a name's generators gives its orientation,
  /// so e41 = -e14 and e321 = -e123. format.hpp prints each component under
  /// the name that has the orientation its layout stores, so a line prints
  /// on e41, e42, e43, e23, e31, e12 and a plane on e423, e431, e412, e321.
  /// A blade held with the other orientation, as e14 is in the join p ^ q,
  /// prints by its canonical name, e and its indices in increasing order.
  static constexpr auto names =
      "1 e1 e2 e3 e4 e41 e42 e43 e23 e31 e12 e423 e431 e412 e321 e1234";
};

/// The layout of a Point, x e1 + y e2 + z e3 + w e4, in that order. The
/// point has weight w, and it is unitized when w = 1.
inline constexpr auto point_layout = core::blades<algebra, "e1 e2 e3 e4">;

/// The layout of a Line: its direction (vx, vy, vz) on e41, e42, e43 and
/// then its moment (mx, my, mz) on e23, e31, e12, where e41 = -e14,
/// e42 = -e24, e43 = -e34, and e31 = -e13. The line p ^ q through unitized
/// points runs from p to q, so its direction is q - p, and its moment is
/// p x q for points of any weight.
inline constexpr auto line_layout =
    core::blades<algebra, "e41 e42 e43 e23 e31 e12">;

/// The layout of a Plane: its normal (gx, gy, gz) on e423, e431, e412 and
/// then its offset gw on e321, where e423 = e234, e431 = -e134,
/// e412 = e124, and e321 = -e123. A point p lies on the plane g when the
/// scalar antiwedge(p, g) = gx x + gy y + gz z + gw w is zero. The plane
/// p ^ q ^ r through three points that run counterclockwise when seen from
/// the tip of its normal has that normal.
inline constexpr auto plane_layout =
    core::blades<algebra, "e423 e431 e412 e321">;

/// The layout of a Motor, on the blades of even grade: (Q_vx, Q_vy, Q_vz,
/// Q_vw) on e41, e42, e43, e1234 and then (Q_mx, Q_my, Q_mz, Q_mw) on e23,
/// e31, e12, 1, the order and coefficient names of Lengyel's Motor page.
/// transform(x, Q) moves x to Q ⟇ x ⟇ antireverse(Q), so lines and planes
/// move as the points that span them. compose(Q2, Q1) is Q2 ⟇ Q1, which
/// applies Q1 first and then Q2. The translator
/// tx e23 + ty e31 + tz e12 + e1234 moves points by 2t, and the rotor
/// l sin(phi) + e1234 cos(phi), for a unitized line l, turns by 2 phi,
/// counterclockwise when seen from the tip of the direction of l: motor
/// coefficients hold half distances and half angles.
inline constexpr auto motor_layout =
    core::blades<algebra, "e41 e42 e43 e1234 e23 e31 e12 1">;

/// The layout of a Flector, on the blades of odd grade: (F_px, F_py, F_pz,
/// F_pw) on e1, e2, e3, e4 and then (F_gx, F_gy, F_gz, F_gw) on e423, e431,
/// e412, e321, the order and coefficient names of Lengyel's Flector page.
/// transform(x, F) moves x to -F ⟇ x^ ⟇ antireverse(F), where x^ is the grade
/// involution of x, so a reflected point keeps a positive weight and a
/// reflected line or plane is the one spanned by the reflected points in the
/// same order. The antiproduct of two motors or of two flectors is a motor,
/// and that of a motor and a flector, in either order, is a flector, so the
/// result of compose converts to a Motor or a Flector
/// (Flector<T>{compose(q, f)}).
inline constexpr auto flector_layout =
    core::blades<algebra, "e1 e2 e3 e4 e423 e431 e412 e321">;

/// A point over the scalar type T, on point_layout: constructed from exactly
/// four values x, y, z, w, and zero by default. T has no default, and a T
/// that is not a ring scalar is rejected where the type is used, with a
/// message for the requirement it fails.
template <std::destructible T>
using Point = core::mv<algebra, point_layout, T>;

/// A line over the scalar type T, on line_layout: constructed from exactly
/// six values, the direction vx, vy, vz and the moment mx, my, mz, and zero
/// by default. T has no default, and a T that is not a ring scalar is
/// rejected where the type is used, with a message for the requirement it
/// fails.
template <std::destructible T>
using Line = core::mv<algebra, line_layout, T>;

/// A plane over the scalar type T, on plane_layout: constructed from exactly
/// four values, the normal gx, gy, gz and the offset gw, and zero by default.
/// T has no default, and a T that is not a ring scalar is rejected where the
/// type is used, with a message for the requirement it fails.
template <std::destructible T>
using Plane = core::mv<algebra, plane_layout, T>;

/// A motor over the scalar type T, on motor_layout: constructed from exactly
/// eight values, Q_vx, Q_vy, Q_vz, Q_vw, Q_mx, Q_my, Q_mz, Q_mw, and zero by
/// default. T has no default, and a T that is not a ring scalar is rejected
/// where the type is used, with a message for the requirement it fails.
template <std::destructible T>
using Motor = core::mv<algebra, motor_layout, T>;

/// A flector over the scalar type T, on flector_layout: constructed from
/// exactly eight values, F_px, F_py, F_pz, F_pw, F_gx, F_gy, F_gz, F_gw, and
/// zero by default. T has no default, and a T that is not a ring scalar is
/// rejected where the type is used, with a message for the requirement it
/// fails.
template <std::destructible T>
using Flector = core::mv<algebra, flector_layout, T>;

} // namespace galoot::presets::rga3

#endif // GALOOT_PRESETS_RGA3_OBJECTS_HPP
