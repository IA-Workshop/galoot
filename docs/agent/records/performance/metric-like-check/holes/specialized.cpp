// An explicit specialization of the library's own metric<2>, whose member
// functions report a valid form while the data is not one. Whether the
// concept accepts it shows which checks trust that metric<N> is the
// library's type. This file compiles only when the concept accepts it.
#include <array>
#include <cstddef>

#include <galoot/core/metric.hpp>

template <>
struct galoot::core::metric<2> {
  std::array<std::array<entry_type, 2>, 2> gram;
  static constexpr std::size_t generator_count = 2;
  [[nodiscard]] constexpr bool is_symmetric() const noexcept { return true; }
  [[nodiscard]] constexpr bool entries_in_range() const noexcept {
    return true;
  }
};

namespace core = galoot::core;

struct skewed {
  static constexpr core::metric<2> value{{{{0, 1}, {2, 0}}}};
};
static_assert(core::metric_like<skewed>);

int main() { return 0; }
