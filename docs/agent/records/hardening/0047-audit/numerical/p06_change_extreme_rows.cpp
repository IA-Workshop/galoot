// The magnitudes x < 0 ? -x : x in multiply_noting (basis_change.hpp) and
// multiply_checked (blade_product.hpp) are undefined for the least
// std::int64_t. A change's rows are int, and every value its checks form is
// kept within -max to max, so the question is whether any input reaches
// them with that value.
//
// -DCASE=1: the most extreme rows a user can write, INT_MIN everywhere it
// keeps the rows independent, between two 3-generator forms. Expected: a
// compile error with one of the change's own messages (an overflow, or the
// inner products not kept), and no undefined behavior, which a constant
// evaluation would report as "not a constant expression".
// -DCASE=2: the guard itself, called at run time with the least value, the
// input no path of the library supplies (run under -fsanitize=undefined).
#include <climits>
#include <cstdint>
#include <cstdio>
#include <limits>

#include <galoot/core.hpp>

namespace core = galoot::core;

struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};

#if CASE == 1
struct extreme {
  using source = r300;
  using target = r300;
  static constexpr auto value = core::from_rows(
      {{INT_MIN, INT_MIN, 0}, {INT_MIN, INT_MAX, INT_MIN}, {0, INT_MIN, INT_MIN}},
      {1, 1, 1});
};
using P = core::mv<r300, core::blades<r300, "e1">, double>;
const auto image = core::change_basis<extreme>(P{1.0});
int main() { return 0; }
#else
int main(int argc, char** /*argv*/) {
  bool fits = true;
  const auto least = std::numeric_limits<std::int64_t>::min() + (argc - 1);
  const auto product = core::detail::multiply_noting(least, 1, fits);
  std::printf("multiply_noting(min, 1) = %lld, fits %d\n",
              static_cast<long long>(product), static_cast<int>(fits));
  return 0;
}
#endif
