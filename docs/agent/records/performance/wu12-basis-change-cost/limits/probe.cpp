// Compile-cost probe of basis_change.hpp: a dense change of R(0, 0, N), whose
// inner products are all zero, so every invertible P keeps them, or
// conformal_change<N - 2>, on the full layout or on the vectors.
#include <cstddef>
#include <galoot/core.hpp>

namespace core = galoot::core;

template <std::size_t N> struct null_algebra {
  static constexpr auto value = core::R<0, 0, N>();
};
template <std::size_t N> consteval core::basis_change<N> dense() {
  core::basis_change<N> c{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      c.rows[i][j] = i == j ? 2 : 1;
    }
    c.denominators[i] = 1;
  }
  return c;
}
template <std::size_t N> struct dense_tag {
  using source = null_algebra<N>;
  using target = null_algebra<N>;
  static constexpr auto value = dense<N>();
};
template <std::size_t D> struct cga { static constexpr auto value = core::conformal<D>(); };
template <std::size_t D> struct cga_diag { static constexpr auto value = core::R<D + 1, 1, 0>(); };
template <std::size_t D> struct cga_tag {
  using source = cga<D>;
  using target = cga_diag<D>;
  static constexpr auto value = core::conformal_change<D>();
};

#if KIND_CONFORMAL
using tag = cga_tag<NGEN - 2>;
#else
using tag = dense_tag<NGEN>;
#endif
#if LAYOUT_FULL
constexpr auto layout = core::full<NGEN>;
#elif LAYOUT_BIVECTORS
constexpr auto layout = core::of_grade<NGEN, 2>;
#else
constexpr auto layout = core::of_grade<NGEN, 1>;
#endif

double use(const core::mv<tag::source, layout, double>& x) {
#if BACK
  const core::mv<tag::target, layout, double> y{};
  return core::change_basis_back<tag>(y).coefficients[0] + x.coefficients[0];
#else
  return core::change_basis<tag>(x).coefficients[0];
#endif
}
