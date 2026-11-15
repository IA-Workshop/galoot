// conformal<D>() with D + 2 wrapping around to zero is rejected only as too
// many generators: the message for n = 0 would be false, so it must not
// appear (core.metric_conformal_wrap_zero). This file must not compile.
#include <cstddef>
#include <limits>

#include <galoot/core/metric.hpp>

namespace {

struct wrapped_to_zero {
  static constexpr auto value =
      galoot::core::conformal<std::numeric_limits<std::size_t>::max() - 1>();
};

} // namespace
