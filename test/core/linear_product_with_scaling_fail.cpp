// A product of a line over double and a motor over float, with every core
// header included, so that the scaling's rejecting overloads are candidates
// too: the message is the product's. This file must not compile.
#include <galoot/core.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;
using Motor =
    core::mv<rga3, core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">,
             float>;

[[maybe_unused]] void use(const Line& a, const Motor& b) {
  [[maybe_unused]] const auto r = a * b;
}

} // namespace
