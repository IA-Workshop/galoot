// A name that starts with a digit but is not 1, the scalar.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r400 {
  static constexpr auto value = galoot::core::R<4, 0, 0>();
};

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<r400, "e1 12">;

} // namespace
