// A metric with more generators than a blade mask has bits cannot be formed,
// even by hand (core.metric_type_width). This file must not compile.
#include <limits>

#include <galoot/core/metric.hpp>

namespace {

constexpr auto too_wide = std::numeric_limits<galoot::core::mask_t>::digits + 1;

struct wide_metric {
  static constexpr galoot::core::metric<too_wide> value{};
};

} // namespace
