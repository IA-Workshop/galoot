// e4 in an algebra of three generators.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r300 {
  static constexpr auto value = galoot::core::R<3, 0, 0>();
};

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<r300, "e1 e14">;

} // namespace
