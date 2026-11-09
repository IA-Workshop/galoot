#ifndef GALOOT_CORE_HPP
#define GALOOT_CORE_HPP

// The umbrella header of the core: it includes every public core header, so
// that `#include <galoot/core.hpp>` gives the whole core. assert.hpp is
// included by the headers that use it.

#include <galoot/core/basis_change.hpp>  // IWYU pragma: export
#include <galoot/core/bits.hpp>          // IWYU pragma: export
#include <galoot/core/blade_product.hpp> // IWYU pragma: export
#include <galoot/core/blade_set.hpp>     // IWYU pragma: export
#include <galoot/core/dual.hpp>          // IWYU pragma: export
#include <galoot/core/linear.hpp>        // IWYU pragma: export
#include <galoot/core/metric.hpp>        // IWYU pragma: export
#include <galoot/core/mv.hpp>            // IWYU pragma: export
#include <galoot/core/plan.hpp>          // IWYU pragma: export
#include <galoot/core/product.hpp>       // IWYU pragma: export
#include <galoot/core/transform.hpp>     // IWYU pragma: export
#include <galoot/core/unary.hpp>         // IWYU pragma: export

#endif // GALOOT_CORE_HPP
