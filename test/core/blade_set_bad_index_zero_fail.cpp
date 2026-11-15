// A name with the index 0: generators count from e1.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r400 {
  static constexpr auto value = galoot::core::R<4, 0, 0>();
};

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<r400, "e1 e10">;

} // namespace
