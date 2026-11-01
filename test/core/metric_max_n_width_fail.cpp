// A MaxN wider than a blade mask is rejected with a message that says so
// (core.metric_max_n_width). This file must not compile.
#include <galoot/core/bits.hpp>
#include <galoot/core/metric.hpp>

namespace {

constexpr auto too_wide = galoot::core::mask_bits + 1;

struct wide_bound {
  static constexpr auto value = galoot::core::R<1, 0, 0, too_wide>();
};

} // namespace
