// The printing header, the rga3 preset and the core's umbrella header, which
// between them include every Galoot header, in that order, then the header
// that defines the modes again. core.public_macros preprocesses this file
// with -dM and checks that the only macros named GALOOT_ left defined are the
// ones a user may read or set. Sorting the includes would lose their order.
// clang-format off
#include <galoot/core/format.hpp>
#include <galoot/presets/rga3.hpp>
#include <galoot/core.hpp>
#include <galoot/core/assert.hpp>
// clang-format on
