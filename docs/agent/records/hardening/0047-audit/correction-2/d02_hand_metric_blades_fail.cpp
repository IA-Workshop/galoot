// A metric written by hand that metric_like refuses for its values, reached
// through blades<Tag, "...">, the usual way to name a layout. -DCASE=<k>:
//   1  an entry beyond max_form_entry (32767), the layout from blades
//   2  a matrix that is not symmetric, the layout from blades
//   3  the entry beyond max_form_entry with the layout full<2>, which skips
//      blades (the input of the numerical read's p07 case 5), for comparison
//   4  the blades of case 1 alone, with no mv named
// This file must not compile.
#include <galoot/core.hpp>

namespace core = galoot::core;

struct big {
  static constexpr core::metric<2> value{{{{40000, 0}, {0, 1}}}};
};
struct skew {
  static constexpr core::metric<2> value{{{{1, 1}, {0, 1}}}};
};

#if CASE == 1
using M = core::mv<big, core::blades<big, "e1 e2">, double>;
const M m{};
#elif CASE == 2
using M = core::mv<skew, core::blades<skew, "e1 e2">, double>;
const M m{};
#elif CASE == 3
using M = core::mv<big, core::full<2>, double>;
const M m{};
#elif CASE == 4
constexpr auto set = core::blades<big, "e1 e2">;
#endif

int main() { return 0; }
