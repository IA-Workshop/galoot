// Spike F: named access returns the component it names. Compile-time checks
// only; the unit compiles exactly when every check holds. Expected values
// come from CONVENTIONS 1.1 and 1.6 (e41 = -e14, so its mask is {1, 4}, bits
// 0 and 3, with sign -1; e423 = +e234; e431 = -e134; e321 = -e123), not from
// the fixture's code. Run by run_named.py diag with the S4 flags.
#include "fixture.hpp"

using namespace cdf;
using namespace cdf::rga3_types;

// The parser against the orientation table of CONVENTIONS 1.6.
static_assert(parse_name(fixed_string("e41"), 4).mask == 0b1001);
static_assert(parse_name(fixed_string("e41"), 4).sign == -1);
static_assert(parse_name(fixed_string("e23"), 4).sign == 1);
static_assert(parse_name(fixed_string("e31"), 4).sign == -1);
static_assert(parse_name(fixed_string("e423"), 4).mask == 0b1110);
static_assert(parse_name(fixed_string("e423"), 4).sign == 1);
static_assert(parse_name(fixed_string("e431"), 4).sign == -1);
static_assert(parse_name(fixed_string("e321"), 4).sign == -1);
static_assert(parse_name(fixed_string("1"), 4).mask == 0);

// Each spelling reads the slot of the blade it names.
constexpr Line line{{10, 20, 30, 40, 50, 60}};  // e41 e42 e43 e23 e31 e12
constexpr Motor motor{{1, 2, 3, 4, 5, 6, 7, 8}}; // 1 e41 ... e12 e1234
static_assert(get<"e41">(line) == 10);
static_assert(get<"e31">(line) == 50);
static_assert(get<"e12">(line) == 60);
static_assert(get<"1">(motor) == 1);
static_assert(get<"e1234">(motor) == 8);
static_assert(line["e43"] == 30);
static_assert(motor["e23"] == 5);

// Writing through the reference changes that slot only.
constexpr Line written = [] {
  Line l{};
  get<"e42">(l) = 7;
  l["e23"] = 9;
  return l;
}();
static_assert(written.c[1] == 7 && written.c[3] == 9);
static_assert(written.c[0] == 0 && written.c[2] == 0 && written.c[4] == 0);

// A name absent from the set, or present with the other orientation, is
// rejected by the constraint (see named_detect.cpp for requires-expressions).
constexpr auto line_blades =
    blades_of<rga3, "e41", "e42", "e43", "e23", "e31", "e12">;
static_assert(!has_blade<rga3, line_blades, "e1">);
static_assert(!has_blade<rga3, line_blades, "e14">);
static_assert(has_blade<rga3, line_blades, "e41">);

int main() { return 0; }
