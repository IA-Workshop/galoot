// conformal<D>() with D so large that D + 2 wraps around to a small n is
// rejected as too many generators, because D is checked before D + 2
// (core.metric_conformal_wrap). This file must not compile.
#include <cstddef>
#include <limits>

#include <galoot/core/metric.hpp>

namespace {

struct wrapped {
  static constexpr auto value =
      galoot::core::conformal<std::numeric_limits<std::size_t>::max()>();
};

} // namespace
