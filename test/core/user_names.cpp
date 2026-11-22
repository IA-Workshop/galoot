// A user's unit that names a pseudoscalar I and a line rn at namespace scope,
// as a short example would. It compiles in every build; in the lint build
// preset, clang-tidy's check of confusable identifiers must find no name in
// the library's headers that a reader could take for I or rn, such as l or m.
//
// It cannot share a file with linear_user_names.cpp, whose l and ls would
// clash with the I declared here.
#include <galoot/core.hpp>
#include <galoot/core/format.hpp> // IWYU pragma: keep
#include <galoot/presets/rga3.hpp>

namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;

constexpr Point o{0, 0, 0, 1};
constexpr Point x{1, 0, 0, 1};
constexpr Point y{0, 1, 0, 1};
constexpr Point z{0, 0, 1, 1};

// The pseudoscalar's customary name is a capital, which the naming check
// does not accept for a constant.
// NOLINTNEXTLINE(readability-identifier-naming)
constexpr auto I = o ^ x ^ y ^ z;
constexpr auto rn = o ^ x;

// Swapping two of the points reverses the pseudoscalar, and swapping the
// ends of the line reverses the line.
static_assert((o ^ y ^ x ^ z) == -I);
static_assert((x ^ o) == -rn);
