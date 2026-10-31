// An algebra with no generators is rejected with a message that says so
// (core.metric_zero). This file must not compile.
#include <galoot/core/metric.hpp>

namespace {

struct empty_algebra {
  static constexpr auto value = galoot::core::R<0, 0, 0>();
};

} // namespace
