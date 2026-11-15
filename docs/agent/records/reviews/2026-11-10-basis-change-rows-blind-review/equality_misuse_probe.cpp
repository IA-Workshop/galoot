// What a user sees who compares two changes with ==, now that basis_change
// has none. This file must not compile; its diagnostics are the record.
#include <galoot/core/basis_change.hpp>

namespace core = galoot::core;

constexpr auto a = core::from_rows({{1, 1}, {1, -1}}, {1, 2});
constexpr auto b = core::from_rows({{2, 2}, {1, -1}}, {2, 2});
static_assert(a == b);
