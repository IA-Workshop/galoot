// Four plans of rigid(3) and three maps of every blade of six generators,
// each read in a static_assert and in two functions, so that a time trace of
// this unit shows whether a plan or a map read three times is generated once
// (core.plan_evaluated_once and core.plan_map_evaluated_once). The unit is
// compiled and not run. The counts in the static_asserts are worked by hand:
// a product of two blades is one term unless both hold e4, whose square is
// 0, a wedge is one term unless the blades share a generator, an
// antiproduct is the product of the complements, and a map has one term for
// each blade.
#include <cstddef>

#include <galoot/core/blade_product.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::detail::BinaryOperation;
using core::detail::product_plan;
using core::detail::unary_map;
using core::detail::UnaryOperation;

namespace {

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

struct r6 {
  static constexpr auto value = core::R<6, 0, 0>();
};

constexpr auto point = core::blades<rga3, "e1 e2 e3 e4">;
constexpr auto line = core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
constexpr auto plane = core::blades<rga3, "e423 e431 e412 e321">;
constexpr auto motor = core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;

// 8 x 8 pairs less the 4 x 4 whose blades both hold e4.
constexpr const auto& motor_motor =
    product_plan<BinaryOperation::gp, rga3, motor, motor>;
static_assert(motor_motor.terms.size() == 48);
// 6 x 4 pairs less the 3 x 3 whose blades both hold e4.
constexpr const auto& line_plane =
    product_plan<BinaryOperation::gp, rga3, line, plane>;
static_assert(line_plane.terms.size() == 15);
// Each vector shares a generator with three of the six bivectors.
constexpr const auto& point_line =
    product_plan<BinaryOperation::wedge, rga3, point, line>;
static_assert(point_line.terms.size() == 12);
// The complements of the motor's blades are 1, the six bivectors, and e1234,
// and those of the point's are trivectors: the 8 x 4 pairs less the 3 x 3 of
// a bivector holding e4 with a trivector holding it, and less the 3 of e1234
// with a trivector holding e4.
constexpr const auto& motor_point =
    product_plan<BinaryOperation::antigp, rga3, motor, point>;
static_assert(motor_point.terms.size() == 20);

constexpr const auto& right_six =
    unary_map<UnaryOperation::right_complement, r6, core::full<6>>;
static_assert(right_six.terms.size() == 64);
constexpr const auto& left_six =
    unary_map<UnaryOperation::left_complement, r6, core::full<6>>;
static_assert(left_six.terms.size() == 64);
constexpr const auto& reverse_six =
    unary_map<UnaryOperation::reverse, r6, core::full<6>>;
static_assert(reverse_six.terms.size() == 64);

} // namespace

// The readers have external linkage on purpose: a function with internal
// linkage that nothing calls is unused and need not be emitted, while these
// must be compiled as a program that uses the plans and maps would compile
// them.
// NOLINTBEGIN(misc-use-internal-linkage)
std::size_t motor_terms() {
  return product_plan<BinaryOperation::gp, rga3, motor, motor>.terms.size();
}

core::coefficient_type motor_scalar() {
  const auto& plan = product_plan<BinaryOperation::gp, rga3, motor, motor>;
  return plan.terms.front().coefficient;
}

std::size_t line_plane_terms() {
  return product_plan<BinaryOperation::gp, rga3, line, plane>.terms.size();
}

core::coefficient_type line_plane_first() {
  const auto& plan = product_plan<BinaryOperation::gp, rga3, line, plane>;
  return plan.terms.front().coefficient;
}

std::size_t point_line_terms() {
  return product_plan<BinaryOperation::wedge, rga3, point, line>.terms.size();
}

core::coefficient_type point_line_first() {
  const auto& plan = product_plan<BinaryOperation::wedge, rga3, point, line>;
  return plan.terms.front().coefficient;
}

std::size_t motor_point_terms() {
  return product_plan<BinaryOperation::antigp, rga3, motor, point>.terms.size();
}

core::coefficient_type motor_point_first() {
  const auto& plan = product_plan<BinaryOperation::antigp, rga3, motor, point>;
  return plan.terms.front().coefficient;
}

std::size_t right_six_terms() {
  return unary_map<UnaryOperation::right_complement, r6, core::full<6>>.terms.size();
}

core::coefficient_type right_six_last() {
  const auto& map =
      unary_map<UnaryOperation::right_complement, r6, core::full<6>>;
  return map.terms.back().coefficient;
}

std::size_t left_six_terms() {
  return unary_map<UnaryOperation::left_complement, r6, core::full<6>>.terms.size();
}

core::coefficient_type left_six_last() {
  const auto& map =
      unary_map<UnaryOperation::left_complement, r6, core::full<6>>;
  return map.terms.back().coefficient;
}

std::size_t reverse_six_terms() {
  return unary_map<UnaryOperation::reverse, r6, core::full<6>>.terms.size();
}

core::coefficient_type reverse_six_last() {
  const auto& map = unary_map<UnaryOperation::reverse, r6, core::full<6>>;
  return map.terms.back().coefficient;
}
// NOLINTEND(misc-use-internal-linkage)
