// R<N>(p, q, r) with p + q + r different from N is rejected with a message
// that says so (core.metric_split). This file must not compile.
#include <galoot/core/metric.hpp>

namespace {

struct wrong_count {
  static constexpr auto value = galoot::core::R<4>(3, 0, 2);
};

} // namespace
