// A metric made from raw integers by std::bit_cast, which calls no
// constructor. The concept must reject it: this file compiles only when it
// does.
#include <array>
#include <bit>

#include <galoot/core/metric.hpp>

namespace core = galoot::core;

struct skewed {
  static constexpr core::metric<2> value =
      std::bit_cast<core::metric<2>>(std::array<core::entry_type, 4>{0, 1, 2, 0});
};
static_assert(!core::metric_like<skewed>);

int main() { return 0; }
