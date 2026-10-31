// A MaxN wider than a blade mask is rejected with a message that says so
// (core.metric_max_n_width). This file must not compile.
#include <limits>

#include <galoot/core/metric.hpp>

namespace {

constexpr auto too_wide = std::numeric_limits<galoot::core::mask_t>::digits + 1;

struct wide_bound {
  static constexpr auto value = galoot::core::R<1, 0, 0, too_wide>();
};

} // namespace
