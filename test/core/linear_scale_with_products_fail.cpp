// A line over double times the int 2, with every core header included, so
// that the products' rejecting overloads are candidates too: the message is
// the scaling's. This file must not compile.
#include <galoot/core.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;

[[maybe_unused]] void use(const Line& a) {
  [[maybe_unused]] const auto r = a * 2;
}

} // namespace
