// A metric with more generators than a blade mask has bits cannot be formed,
// even by hand (core.metric_type_width). This file must not compile.
#include <galoot/core/bits.hpp>
#include <galoot/core/metric.hpp>

namespace {

constexpr auto too_wide = galoot::core::mask_bits + 1;

struct wide_metric {
  static constexpr galoot::core::metric<too_wide> value{};
};

} // namespace
