// F2 and C4 probe: the first error for single-argument misuses, before and
// after the review fixes.
#include <complex>
#include <string>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <support/rational.hpp>

namespace core = galoot::core;
using test_support::rational;
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto scalar_layout = core::blades<rga3, "1">;
template <class T> using Line = core::mv<rga3, line_layout, T>;
template <class T> using Scalar = core::mv<rga3, scalar_layout, T>;
struct explicit_to_double {
  explicit operator double() const { return 1; }
};
struct derived_rational : rational {
  using rational::rational;
};

void f() {
#if CASE == 1
  [[maybe_unused]] Line<rational> x{1};
#elif CASE == 2
  [[maybe_unused]] Scalar<rational> x{1};
#elif CASE == 3
  [[maybe_unused]] Scalar<double> x{explicit_to_double{}};
#elif CASE == 4
  [[maybe_unused]] Line<double> x{explicit_to_double{}};
#elif CASE == 5
  [[maybe_unused]] Line<double> x{std::complex<double>{}};
#elif CASE == 6
  [[maybe_unused]] Scalar<std::complex<float>> x{std::complex<double>{}};
#elif CASE == 7
  [[maybe_unused]] Line<double> x{std::string{}};
#elif CASE == 8
  [[maybe_unused]] Line<double> x{nullptr};
#elif CASE == 9
  [[maybe_unused]] Scalar<rational> x{derived_rational{1}};
#elif CASE == 10
  [[maybe_unused]] Line<rational> x{derived_rational{1}};
#elif CASE == 11
  [[maybe_unused]] Scalar<std::complex<double>> x{1.0};
#endif
}
