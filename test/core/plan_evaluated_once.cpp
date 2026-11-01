// Three plans of rigid(3), each read in a static_assert and in two
// functions, so that a time trace of this unit shows whether a plan read
// three times is generated once (core.plan_evaluated_once). The unit is
// compiled and not run. The counts in the static_asserts are worked by hand:
// a product of two blades is one term unless both hold e4, whose square is
// 0, and a wedge is one term unless the blades share a generator.
#include <cstddef>

#include <galoot/core/blade_product.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::detail::BinaryOperation;
using core::detail::product_plan;

namespace {

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

constexpr auto point = core::blades<4, "e1 e2 e3 e4">;
constexpr auto line = core::blades<4, "e41 e42 e43 e23 e31 e12">;
constexpr auto plane = core::blades<4, "e423 e431 e412 e321">;
constexpr auto motor = core::blades<4, "1 e41 e42 e43 e23 e31 e12 e1234">;

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

} // namespace

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
