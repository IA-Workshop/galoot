// galoot/core.hpp gives a multivector no formatter, even beside <format>:
// the formatter lives in galoot/core/format.hpp, which core.hpp does not
// include, so that a program that never prints a multivector does not pay
// for <format>. A program of its own, because format_test.cpp includes
// format.hpp, and std::formattable would see its formatter there.
#include <format>

#include <galoot/core.hpp>

#include <doctest/doctest.h>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;

// Reference: contract: galoot/core.hpp does not include
//   galoot/core/format.hpp, so with core.hpp and <format> alone
//   std::formattable answers no for a multivector
TEST_CASE("core.hpp alone gives a multivector no formatter") {
  static_assert(!std::formattable<Line, char>);
  CHECK(!std::formattable<Line, char>);
}

} // namespace
