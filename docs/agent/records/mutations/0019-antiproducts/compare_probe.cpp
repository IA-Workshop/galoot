// A product compared with a value of a named layout: the result's layout is
// the canonical set, so == has no overload, and the error shows the result
// type. This file must not compile.
#include <galoot/core.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, double>;
using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;

[[maybe_unused]] bool on_line(const Point& p, const Point& q,
                              const Line& line) {
  return (p ^ q) == line;
}

} // namespace
