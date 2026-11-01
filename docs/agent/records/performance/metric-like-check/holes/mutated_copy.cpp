// A metric from a factory, changed afterwards in a constexpr function. The
// concept must reject it: this file compiles only when it does.
#include <galoot/core/metric.hpp>

namespace core = galoot::core;

constexpr core::metric<3> skew() {
  auto m = core::rigid<2>();
  m.gram[0][1] = 7;
  return m;
}
struct skewed {
  static constexpr core::metric<3> value = skew();
};
static_assert(!core::metric_like<skewed>);

int main() { return 0; }
