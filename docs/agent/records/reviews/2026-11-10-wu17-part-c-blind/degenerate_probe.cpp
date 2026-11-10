// Counts of one transform whose restricted first stage drops terms: in the
// degenerate form with e1 . e2 = 1 and every other product of generators 0,
// V on e1, e2, e3, e123 and x on e123. With checking off the first stage
// keeps 2 of its 4 terms, so a first stage kept whole makes 2 more
// multiplications.
#include <cstdio>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/transform.hpp>

#include <support/counting.hpp>

namespace core = galoot::core;
using test_support::counting;

struct degenerate3 {
  static constexpr auto value =
      core::from_gram({{0, 1, 0}, {1, 0, 0}, {0, 0, 0}});
};

int main() {
  constexpr auto vb = core::blades<degenerate3, "e1 e2 e3 e123">;
  constexpr auto xb = core::blades<degenerate3, "e123">;
  const core::mv<degenerate3, vb, counting> v{counting{1}, counting{2},
                                              counting{3}, counting{4}};
  const core::mv<degenerate3, xb, counting> x{counting{5}};
  test_support::reset_counts();
  [[maybe_unused]] const auto y = core::transform(x, v);
  const auto c = test_support::counts();
  std::printf("mul %d add %d sub %d neg %d cmp %d int %d def %d\n",
              c.multiplications, c.additions, c.subtractions, c.negations,
              c.comparisons, c.integer_constructions,
              c.default_constructions);
}
