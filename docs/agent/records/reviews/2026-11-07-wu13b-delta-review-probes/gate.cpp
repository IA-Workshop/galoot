// Which scalars and transforms the rounding check now covers.
#include <cstdio>
#include <stdfloat>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct r400 { static constexpr auto value = core::R<4, 0, 0>(); };
constexpr auto PL = core::blades<rga3, "e1 e2 e3 e4">;
constexpr auto LL = core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
constexpr auto ML = core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
constexpr auto FL = core::blades<rga3, "e1 e2 e3 e4 e423 e431 e412 e321">;
using H = core::mv<cga3, core::of_grade<5, 4>, double>;
constexpr auto CM = decltype(H{} % H{})::layout;
constexpr auto CF = decltype((H{} % H{}) % H{})::layout;
constexpr auto CV = core::of_grade<5, 1>;
template <class T> void row(const char* n) {
  using core::detail::transform_rounding_checked;
  std::printf("%-12s motor/point %d motor/line %d flector/line %d cga3 motor/vector %d cga3 flector/vector %d full<4>/full<4> r400 %d\n", n,
    (int)transform_rounding_checked<rga3, ML, PL, T>, (int)transform_rounding_checked<rga3, ML, LL, T>,
    (int)transform_rounding_checked<rga3, FL, LL, T>, (int)transform_rounding_checked<cga3, CM, CV, T>,
    (int)transform_rounding_checked<cga3, CF, CV, T>, (int)transform_rounding_checked<r400, core::full<4>, core::of_grade<4, 1>, T>);
}
#ifdef __STDCPP_BFLOAT16_T__
// bfloat16 in a constant evaluation of the R(4,0,0) non-versor (K = 3, still
// checked), which must fail if uncommented; and rigid(3), unchecked.
#define BF(...) __VA_ARGS__
#else
#define BF(...)
#endif
int main() {
  row<float>("float"); row<double>("double"); row<long double>("long double");
#ifdef __STDCPP_FLOAT16_T__
  row<std::float16_t>("float16_t");
#endif
#ifdef __STDCPP_BFLOAT16_T__
  row<std::bfloat16_t>("bfloat16_t");
#endif
}
