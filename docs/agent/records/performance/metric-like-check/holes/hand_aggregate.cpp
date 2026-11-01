// A metric written by hand as an aggregate, with an asymmetric form. The
// concept must reject it: this file compiles only when it does.
#include <galoot/core/metric.hpp>

namespace core = galoot::core;

struct skewed {
  static constexpr core::metric<2> value{{{{0, 1}, {2, 0}}}};
};
static_assert(!core::metric_like<skewed>);

int main() { return 0; }
