// R<N>(p, q, r) with a negative count is rejected with a message that says
// so, even when p + q + r equals N (core.metric_negative). This file must not
// compile.
#include <galoot/core/metric.hpp>

namespace {

struct negative_count {
  static constexpr auto value = galoot::core::R<3>(4, 0, -1);
};

} // namespace
