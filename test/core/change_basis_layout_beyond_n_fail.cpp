// change_basis_layout given a blade set written by hand that names e6 in
// conformal(3), which has five generators. A multivector rejects such a set
// before change_basis sees it, so only the layout can be given it.
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

constexpr core::blade_set<2> beyond{{{
    {.mask = 0b000001, .sign = 1},
    {.mask = 0b100000, .sign = 1},
}}};

[[maybe_unused]] constexpr const auto& layout =
    core::change_basis_layout<cga3_to_r410, beyond>;

} // namespace
