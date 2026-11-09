// Run under a header copy with the mutant inverse_rows_overflow_ignored: the
// change of inverse_rows_overflow.cpp, accepted, maps each e'j back by rows
// that hold 0 where a product overflowed. Prints the image of e'1, e'2, e'3.
#include <cstdio>
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
namespace core = galoot::core;
struct z3 { static constexpr auto value = core::R<0, 0, 3>(); };
struct bad { using source = z3; using target = z3;
  static constexpr auto value = core::from_rows({{0, 1048576, 0}, {0, -16777216, 46341}, {2, -378, 0}}, {1, 1073741824, 2147483647}); };
int main() {
  using V = core::mv<z3, core::blades<z3, "e1 e2 e3">, double>;
  const char* names[] = {"e'1", "e'2", "e'3"};
  for (int j = 0; j < 3; ++j) {
    V y{};
    y.coefficients.at(static_cast<std::size_t>(j)) = 1;
    const auto x = core::change_basis_back<bad>(y);
    std::printf("%s ->", names[j]);
    for (double c : x.coefficients) std::printf(" %.17g", c);
    std::printf("\n");
  }
}
