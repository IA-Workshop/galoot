// A line over the test rational plus a rational, x + s, a number that is a
// ring scalar but not an arithmetic type. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

// The compile-fail tests compile with the library's include paths only, so
// the header is found from this file.
#include "../support/rational.hpp"

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

using T = test_support::rational;
using Line = core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, T>;

[[maybe_unused]] Line use(const Line& x) { return x + T{1}; }

} // namespace
