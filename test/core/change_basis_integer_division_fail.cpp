// The origin e4 of conformal(3), over std::int64_t: its image
// (e'5 - e'4) / 2 needs a division.
// This file must not compile.
#include <cstdint>

#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
struct r410 {
  static constexpr auto value = core::R<4, 1, 0>();
};
struct cga3_to_r410 {
  using source = cga3;
  using target = r410;
  static constexpr auto value = core::conformal_change<3>();
};

using X = core::mv<cga3, core::blades<cga3, "e4">, std::int64_t>;

[[maybe_unused]] void use(const X& x) {
  [[maybe_unused]] auto r = core::change_basis<cga3_to_r410>(x);
}

} // namespace
