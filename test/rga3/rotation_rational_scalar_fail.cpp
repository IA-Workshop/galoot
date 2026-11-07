// A rotation about a line over the test rational, an exact scalar type with
// no std::sin or std::cos. This file must not compile.
#include <galoot/rga3.hpp>

// The compile-fail tests compile with the library's include paths only, so
// the header is found from this file.
#include "../support/rational.hpp"

namespace {

namespace rga3 = galoot::rga3;
using test_support::rational;

[[maybe_unused]] void use(const rga3::Line<rational>& l) {
  [[maybe_unused]] const auto r = rga3::rotation(l, rational{1, 2});
}

} // namespace
