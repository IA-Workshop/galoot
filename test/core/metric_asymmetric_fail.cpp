// A form that is not symmetric is rejected where the metric is made, with a
// message that says so (core.metric_asymmetric). This file must not compile.
#include <galoot/core/metric.hpp>

namespace {

struct lopsided {
  static constexpr auto value = galoot::core::from_gram({{1, 2}, {0, 1}});
};

} // namespace
