// An algebra with more generators than max_generators is rejected, when no
// larger MaxN is given, with a message that says so (core.metric_bound). This
// file must not compile.
#include <galoot/core/metric.hpp>

namespace {

struct too_large {
  static constexpr auto value = galoot::core::rigid<16>();
};

} // namespace
