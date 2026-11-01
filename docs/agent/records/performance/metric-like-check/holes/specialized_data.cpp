// As specialized.cpp, but the specialization's data lies as well: its gram
// is a type whose subscript returns the same row whatever the index, so every
// read sees a symmetric form while the stored entries are not one. This file
// compiles only when the concept accepts it.
#include <array>
#include <cstddef>

#include <galoot/core/metric.hpp>

struct lying_rows {
  std::array<std::array<galoot::core::entry_type, 2>, 2> stored;
  [[nodiscard]] constexpr const std::array<galoot::core::entry_type, 2>&
  operator[](std::size_t /*i*/) const noexcept {
    return stored[0];
  }
};

template <>
struct galoot::core::metric<2> {
  lying_rows gram;
  static constexpr std::size_t generator_count = 2;
  [[nodiscard]] constexpr bool is_symmetric() const noexcept { return true; }
  [[nodiscard]] constexpr bool entries_in_range() const noexcept {
    return true;
  }
};

namespace core = galoot::core;

struct skewed {
  static constexpr core::metric<2> value{{{{{1, 1}, {2, 0}}}}};
};
static_assert(core::metric_like<skewed>);

int main() { return 0; }
