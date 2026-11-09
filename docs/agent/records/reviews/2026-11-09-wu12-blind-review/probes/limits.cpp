// Reviewer's probe of basis_change.hpp's stated compile limits. Select with
// -DKIND=0 (dense change of R(0,0,N), full<PN>), 1 (conformal_change<PD>,
// full<PD+2>), 2 (dense, vectors of N); -DPN=... -DPD=...
#include <cstddef>
#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
namespace core = galoot::core;
template <std::size_t n> consteval core::basis_change<n> dense() {
  core::basis_change<n> c{};
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) c.rows[i][j] = i == j ? 2 : 1;
    c.denominators[i] = 1;
  }
  return c;
}
#if KIND == 1
struct src { static constexpr auto value = core::conformal<PD>(); };
struct dst { static constexpr auto value = core::R<PD + 1, 1, 0>(); };
struct chg { using source = src; using target = dst; static constexpr auto value = core::conformal_change<PD>(); };
constexpr std::size_t n = PD + 2;
#else
struct src { static constexpr auto value = core::R<0, 0, PN>(); };
struct chg { using source = src; using target = src; static constexpr auto value = dense<PN>(); };
constexpr std::size_t n = PN;
#endif
#if KIND == 2
using L = core::mv<src, core::of_grade<n, 1>, double>;
#else
using L = core::mv<src, core::full<n>, double>;
#endif
double use(const L& x) {
  auto y = core::change_basis<chg>(x);
  auto z = core::change_basis_back<chg>(y);
  return y.coefficients[0] + z.coefficients[0];
}
