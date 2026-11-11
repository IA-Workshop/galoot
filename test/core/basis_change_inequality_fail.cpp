// Two values of a change compared with !=, which the language rewrites as
// the negation of ==, which basis_change deletes: the error must point at
// same_rows, as for ==.
// This file must not compile.
#include <galoot/core/basis_change.hpp>

namespace {

namespace core = galoot::core;

[[maybe_unused]] bool compare() {
  constexpr auto a = core::from_rows({{1, 0}, {0, 1}}, {1, 1});
  constexpr auto b = core::from_rows({{2, 0}, {0, 2}}, {2, 2});
  return a != b;
}

} // namespace
