// The transform of transform_consteval_nonzero_fail.cpp over the test
// rational: in R(4,0,0), x = 1 on the scalar and V = 1 + e1234 give
// 2 + 2 e1234, and the result keeps the scalar alone. This file must not
// compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/transform.hpp>

// The compile-fail tests compile with the library's include paths only, so
// the header is found from this file.
#include "../support/rational.hpp"

namespace {

namespace core = galoot::core;

struct r400 {
  static constexpr auto value = core::R<4, 0, 0>();
};

using T = test_support::rational;
using X = core::mv<r400, core::blades<r400, "1">, T>;
using V = core::mv<r400, core::blades<r400, "1 e1234">, T>;

[[maybe_unused]] constexpr X kept = core::transform(X{T{1}}, V{T{1}, T{1}});

} // namespace
