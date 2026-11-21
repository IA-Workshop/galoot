// A product coefficient that overflows coefficient_type, on an input of this
// correction's own: diag(5000, ..., 5000) in six generators, every entry
// within max_form_entry, and e123456 e123456 = -5000^6 = -1.5625e22, beyond
// 2^63 (about 9.22e18). The build must fail; the question is how many
// errors each compiler reports.
#include <cstdint>

#include <galoot/core.hpp>

namespace core = galoot::core;

struct six {
  static constexpr auto value = core::from_gram({{5000, 0, 0, 0, 0, 0},
                                                 {0, 5000, 0, 0, 0, 0},
                                                 {0, 0, 5000, 0, 0, 0},
                                                 {0, 0, 0, 5000, 0, 0},
                                                 {0, 0, 0, 0, 5000, 0},
                                                 {0, 0, 0, 0, 0, 5000}});
};
using B = core::mv<six, core::blades<six, "e123456">, std::int64_t>;
constexpr auto r = B{1} * B{1};

int main() { return 0; }
