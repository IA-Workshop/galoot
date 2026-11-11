// Prints, for each case of the collection pass's cost units, the versor's
// and the object's layouts, the middle layout of the transform's first
// antiproduct, and the transform's two kernels, as JSON for units.py.
#include <array>
#include <cstddef>
#include <print>

#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;
namespace rga3 = galoot::presets::rga3;

namespace {

struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
constexpr auto cga3_motor =
    core::blades<cga3, "e415 e425 e435 e12345 e235 e315 e125 e5">;
// Every blade of even antigrade: grades 1, 3, and 5.
constexpr auto cga3_even = core::blades<
    cga3, "e1 e2 e3 e4 e5 e123 e124 e125 e134 e135 e145 e234 e235 e245 "
          "e345 e12345">;

template <std::size_t K>
void print_set(const char* key, const core::blade_set<K>& set) {
  std::print("\"{}\": [", key);
  for (std::size_t i = 0; i < K; ++i) {
    std::print("{}[{}, {}]", i ? ", " : "", set.blades[i].mask,
               set.blades[i].sign);
  }
  std::print("], ");
}

template <std::size_t N>
void print_kernel(const char* key, const std::array<detail::kernel_term, N>& k) {
  std::print("\"{}\": [", key);
  for (std::size_t i = 0; i < N; ++i) {
    std::print("{}[{}, {}, {}, {}, {}]", i ? ", " : "", k[i].a, k[i].b,
               k[i].out, k[i].coefficient, k[i].first ? 1 : 0);
  }
  std::print("]");
}

template <class Alg, auto V, auto X>
void print_case(const char* name, const char* algebra, int n, bool comma) {
  std::print("  \"{}\": {{\"algebra\": \"{}\", \"n\": {}, ", name, algebra, n);
  print_set("versor", V);
  print_set("object", X);
  print_set("middle", detail::sandwich_middle_layout<Alg, V, X>);
  print_kernel("first", detail::first_kernel<Alg, V, X, false>);
  std::print(", ");
  print_kernel("second", detail::second_kernel<Alg, V, X, true>);
  std::println("}}{}", comma ? "," : "");
}

} // namespace

int main() {
  using A = rga3::algebra;
  std::println("{{");
  print_case<A, rga3::motor_layout, rga3::point_layout>("rigid3_motor_point",
                                                         "rigid3", 4, true);
  print_case<A, rga3::motor_layout, rga3::line_layout>("rigid3_motor_line",
                                                        "rigid3", 4, true);
  print_case<A, rga3::motor_layout, rga3::plane_layout>("rigid3_motor_plane",
                                                         "rigid3", 4, true);
  print_case<cga3, cga3_motor, core::of_grade<5, 1>>("cga3_motor_grade1",
                                                     "conformal3", 5, true);
  print_case<cga3, cga3_motor, core::of_grade<5, 2>>("cga3_motor_grade2",
                                                     "conformal3", 5, true);
  print_case<cga3, cga3_motor, core::of_grade<5, 3>>("cga3_motor_grade3",
                                                     "conformal3", 5, true);
  print_case<cga3, cga3_motor, core::of_grade<5, 4>>("cga3_motor_grade4",
                                                     "conformal3", 5, true);
  print_case<cga3, cga3_even, core::of_grade<5, 1>>("cga3_even_grade1",
                                                    "conformal3", 5, true);
  print_case<cga3, cga3_even, core::of_grade<5, 2>>("cga3_even_grade2",
                                                    "conformal3", 5, false);
  std::println("}}");
}
