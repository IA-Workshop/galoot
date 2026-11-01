// Counts whose sum wraps around to zero are rejected only as too many
// generators: the message for n = 0 would be false, so it must not appear
// (core.metric_count_wrap_zero). This file must not compile.
#include <cstddef>
#include <limits>

#include <galoot/core/metric.hpp>

namespace {

struct wrapped_to_zero {
  static constexpr auto value =
      galoot::core::R<std::numeric_limits<std::size_t>::max(), 0, 1>();
};

} // namespace
