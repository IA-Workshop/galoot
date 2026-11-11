// A user's unit that names a line l at namespace scope and negates it, as
// a short example would. It compiles in every build; in the lint preset,
// clang-tidy's check of confusable identifiers must find no name in the
// library's headers that a reader could take for l, such as I.
#include <galoot/presets/rga3.hpp>

namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Line = rga3::Line<int>;

constexpr Point p{1, 0, 0, 1};
constexpr Point q{1, 2, 0, 1};
constexpr Line l{p ^ q};

// Reversing the points reverses the line.
static_assert(Line{q ^ p} == -l);
