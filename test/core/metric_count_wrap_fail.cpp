// A count so large that p + q + z wraps around to a small n is rejected as
// too many generators, because each count is checked before the sum
// (core.metric_count_wrap). This file must not compile.
#include <cstddef>
#include <limits>

#include <galoot/core/metric.hpp>

namespace {

struct wrapped {
  static constexpr auto value =
      galoot::core::R<std::numeric_limits<std::size_t>::max(), 0, 2>();
};

} // namespace
