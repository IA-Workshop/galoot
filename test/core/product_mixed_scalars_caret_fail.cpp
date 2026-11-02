// A product of two scalar types: caret. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = core::mv<rga3, line_layout, double>;
using Other = core::mv<rga3, core::full<4>, float>;

[[maybe_unused]] void use(const Line& a, const Other& b) {
  using core::geometric_product;
  using core::wedge;
  (void)(a ^ b);
}

} // namespace
