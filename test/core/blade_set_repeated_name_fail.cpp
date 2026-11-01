// One name given twice.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r400 {
  static constexpr auto value = galoot::core::R<4, 0, 0>();
};

[[maybe_unused]] constexpr auto twice =
    galoot::core::blades<r400, "e12 e3 e12">;

} // namespace
