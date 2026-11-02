// Prints the construction and conversion traits of mv types for one-argument
// sources, to compare the header before and after the review fixes.
#include <array>
#include <complex>
#include <cstdint>
#include <cstdio>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <support/rational.hpp>

namespace core = galoot::core;
using test_support::rational;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto scalar_layout = core::blades<rga3, "1">;
template <class T> using Point = core::mv<rga3, point_layout, T>;
template <class T> using Line = core::mv<rga3, line_layout, T>;
template <class T> using Scalar = core::mv<rga3, scalar_layout, T>;

// A class with an explicit conversion to double.
struct explicit_to_double {
  explicit operator double() const { return 1; }
};
// A class with an implicit conversion to double.
struct implicit_to_double {
  operator double() const { return 1; }  // NOLINT
};
// A class derived from the rational.
struct derived_rational : rational {
  using rational::rational;
};
struct derived_line : Line<double> {
  using mv::mv;
};

#define ROW(M, V)                                                             \
  std::printf("%-44s constructible %d convertible %d\n", #M " <- " #V,        \
              int(std::is_constructible_v<M, V>),                              \
              int(std::is_convertible_v<V, M>))

int main() {
  using d_arr4 = double (&)[4];
  using cd_arr4 = const double (&)[4];
  using d_arr6 = double (&)[6];
  using arr6 = std::array<double, 6>;
  using arr4 = std::array<double, 4>;
  using span4 = std::span<const double, 4>;
  ROW(Line<double>, int);
  ROW(Line<double>, double);
  ROW(Line<double>, explicit_to_double);
  ROW(Line<double>, implicit_to_double);
  ROW(Line<double>, std::complex<double>);
  ROW(Line<double>, std::string);
  ROW(Line<double>, arr6);
  ROW(Line<double>, const double*);
  ROW(Line<double>, std::vector<double>);
  ROW(Line<double>, Line<float>);
  ROW(Line<double>, derived_line);
  ROW(Line<rational>, int);
  ROW(Line<rational>, std::int64_t);
  ROW(Line<rational>, rational);
  ROW(Line<rational>, derived_rational);
  ROW(Line<rational>, double);
  ROW(Scalar<double>, int);
  ROW(Scalar<double>, double);
  ROW(Scalar<double>, explicit_to_double);
  ROW(Scalar<double>, implicit_to_double);
  ROW(Scalar<double>, std::complex<double>);
  ROW(Scalar<rational>, int);
  ROW(Scalar<rational>, rational);
  ROW(Scalar<rational>, derived_rational);
  ROW(Scalar<std::complex<double>>, double);
  ROW(Scalar<std::complex<double>>, std::complex<float>);
  ROW(Scalar<std::complex<float>>, std::complex<double>);
  ROW(Line<std::complex<float>>, std::complex<double>);
  ROW(Line<std::complex<double>>, std::complex<float>);
  ROW(Point<double>, d_arr4);
  ROW(Point<double>, cd_arr4);
  ROW(Point<double>, d_arr6);
  ROW(Point<double>, double[4]);
  ROW(Point<double>, arr4);
  ROW(Point<double>, span4);
  ROW(Scalar<double>, double (&)[1]);
  return 0;
}
