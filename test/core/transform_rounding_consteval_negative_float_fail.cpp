// A transform over float in a constant evaluation that drops a negative
// coefficient far above its rounding error: in R(4,0,0), x = 1 on the
// scalar and V = 1 - e1234, which is not a versor, give
// V ⟇ x ⟇ antireverse(V) = 2 - 2 e1234, since 1 ⟇ 1 = e1234 and
// 1 ⟇ e1234 = e1234 ⟇ 1 = 1 there, and the result keeps the scalar alone,
// so -2 e1234 is dropped, while each product is exact. The check compares
// the size of what is dropped, and a constant evaluation always runs it.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/transform.hpp>

namespace {

namespace core = galoot::core;

struct r400 {
  static constexpr auto value = core::R<4, 0, 0>();
};

using T = float;
using X = core::mv<r400, core::blades<r400, "1">, T>;
using V = core::mv<r400, core::blades<r400, "1 e1234">, T>;

[[maybe_unused]] constexpr X kept = core::transform(X{T{1}}, V{T{1}, T{-1}});

} // namespace
