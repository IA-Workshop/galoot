// change_basis_layout given a blade set written by hand with the sign 0.
// This file must not compile.
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

constexpr core::blade_set<1> unsigned_blade{{{{.mask = 0b00001, .sign = 0}}}};

[[maybe_unused]] constexpr const auto& layout =
    core::change_basis_layout<cga3_to_r410, unsigned_blade>;

} // namespace
