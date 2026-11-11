// Two values of a change compared with ==, which basis_change does not
// define: two values for one matrix can differ, as these two do, so the
// error must point at same_rows, which compares them as written.
// This file must not compile.
#include <galoot/core/basis_change.hpp>

namespace {

namespace core = galoot::core;

[[maybe_unused]] bool compare() {
  constexpr auto a = core::from_rows({{1, 0}, {0, 1}}, {1, 1});
  constexpr auto b = core::from_rows({{2, 0}, {0, 2}}, {2, 2});
  return a == b;
}

} // namespace
