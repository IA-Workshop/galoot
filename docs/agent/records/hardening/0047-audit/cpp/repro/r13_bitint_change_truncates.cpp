// A scalar whose std::numeric_limits has no specialization, here Clang's
// _BitInt(128), an integer type: change_basis takes it as a scalar that
// divides exactly and halves the origin's coefficient by truncation; the
// integer division check of a / s and the exact checks of truncate are
// skipped too. The guide states this class of scalar ("not detected").
#include <cstdio>
#include <galoot/core.hpp>
namespace core = galoot::core;
using big = _BitInt(128);
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct r410 { static constexpr auto value = core::R<4, 1, 0>(); };
struct to_r410 {
  using source = cga3;
  using target = r410;
  static constexpr auto value = core::conformal_change<3>();
};
using Origin = core::mv<cga3, core::blades<cga3, "e4">, big>;
int main() {
  const auto image = core::change_basis<to_r410>(Origin{big{1}});
  // e4 = (e'5 - e'4) / 2, so the exact image is -1/2 on e'4 and 1/2 on e'5.
  std::printf("image of the origin over _BitInt(128): %d %d\n",
              static_cast<int>(image.coefficients[0]),
              static_cast<int>(image.coefficients[1]));
  return 0;
}
