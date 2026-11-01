#ifndef GALOOT_CORE_HPP
#define GALOOT_CORE_HPP

// The umbrella header of the core: it includes every public core header, so
// that `#include <galoot/core.hpp>` gives the whole core. assert.hpp is
// included by the headers that use it.

#include <galoot/core/bits.hpp>   // IWYU pragma: export
#include <galoot/core/metric.hpp> // IWYU pragma: export

#endif // GALOOT_CORE_HPP
