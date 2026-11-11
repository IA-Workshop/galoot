#include <array>
#include <cstddef>

#include <galoot/core.hpp>

#include "../../collect.hpp"

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct alg {
  static constexpr auto value = core::conformal<3>();
};

constexpr std::array<detail::kernel_term, 40> first{{
    {0, 0, 5, 1, true},
    {0, 1, 2, 1, true},
    {0, 2, 1, -1, true},
    {0, 3, 11, 1, true},
    {0, 4, 12, 1, true},
    {1, 0, 2, -1, false},
    {1, 1, 5, 1, false},
    {1, 2, 0, 1, true},
    {1, 3, 8, -1, true},
    {1, 4, 9, -1, true},
    {2, 0, 1, 1, false},
    {2, 1, 0, -1, false},
    {2, 2, 5, 1, false},
    {2, 3, 6, 1, true},
    {2, 4, 7, 1, true},
    {3, 0, 0, 1, false},
    {3, 1, 1, 1, false},
    {3, 2, 2, 1, false},
    {3, 3, 3, 1, true},
    {3, 4, 4, 1, true},
    {4, 0, 4, 1, false},
    {4, 1, 7, -1, false},
    {4, 2, 9, -1, false},
    {4, 3, 0, 1, false},
    {4, 3, 10, -1, true},
    {5, 0, 7, 1, false},
    {5, 1, 4, 1, false},
    {5, 2, 12, -1, false},
    {5, 3, 1, 1, false},
    {5, 3, 13, -1, true},
    {6, 0, 9, 1, false},
    {6, 1, 12, 1, false},
    {6, 2, 4, 1, false},
    {6, 3, 2, 1, false},
    {6, 3, 14, -1, true},
    {7, 0, 12, -1, false},
    {7, 1, 9, 1, false},
    {7, 2, 7, -1, false},
    {7, 3, 5, -1, false},
    {7, 3, 15, 1, true},
}};

constexpr std::array<detail::kernel_term, 40> second{{
    {0, 1, 2, -1, true},
    {0, 2, 1, 1, true},
    {0, 3, 0, 1, true},
    {0, 4, 4, 1, true},
    {1, 0, 2, 1, false},
    {1, 2, 0, -1, false},
    {1, 3, 1, 1, false},
    {1, 5, 4, 1, false},
    {2, 0, 1, -1, false},
    {2, 1, 0, 1, false},
    {2, 3, 2, 1, false},
    {2, 6, 4, 1, false},
    {3, 3, 3, 1, true},
    {3, 4, 0, 1, false},
    {3, 5, 1, 1, false},
    {3, 6, 2, 1, false},
    {4, 3, 4, 1, false},
    {5, 0, 0, 1, false},
    {5, 1, 1, 1, false},
    {5, 2, 2, 1, false},
    {5, 7, 4, -1, false},
    {6, 2, 3, 1, false},
    {6, 4, 1, 1, false},
    {6, 5, 0, -1, false},
    {6, 7, 2, -1, false},
    {7, 2, 4, 1, false},
    {8, 1, 3, -1, false},
    {8, 4, 2, 1, false},
    {8, 6, 0, -1, false},
    {8, 7, 1, 1, false},
    {9, 1, 4, -1, false},
    {10, 4, 4, -1, false},
    {11, 0, 3, 1, false},
    {11, 5, 2, 1, false},
    {11, 6, 1, -1, false},
    {11, 7, 0, -1, false},
    {12, 0, 4, 1, false},
    {13, 5, 4, -1, false},
    {14, 6, 4, -1, false},
    {15, 7, 4, 1, false},
}};

constexpr auto summary =
    collect::summarize<8, 5, 16>(first, second);
constexpr auto result =
    collect::make_collected<summary, 8, 5>();
static_assert(result.products.size() > 0);
