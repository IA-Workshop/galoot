// A sum of two algebras: a - b with a line of rigid(3) and a vector of
// conformal(3). This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;
using Vector = core::mv<cga3, core::of_grade<5, 1>, double>;

[[maybe_unused]] void use(const Line& a, const Vector& b) {
  [[maybe_unused]] const auto r = a - b;
}

} // namespace
