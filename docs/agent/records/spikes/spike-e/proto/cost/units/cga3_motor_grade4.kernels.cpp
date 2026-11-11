#include <array>
#include <cstddef>

#include <galoot/core.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct alg {
  static constexpr auto value = core::conformal<3>();
};

constexpr std::array<detail::kernel_term, 40> first{{
    {0, 0, 3, -1, true},
    {0, 1, 4, -1, true},
    {0, 2, 14, 1, true},
    {0, 3, 13, -1, true},
    {0, 4, 10, -1, true},
    {1, 0, 6, -1, true},
    {1, 1, 7, -1, true},
    {1, 2, 15, 1, true},
    {1, 3, 10, 1, false},
    {1, 4, 13, -1, false},
    {2, 0, 8, -1, true},
    {2, 1, 9, -1, true},
    {2, 2, 10, -1, false},
    {2, 3, 15, 1, false},
    {2, 4, 14, -1, false},
    {3, 0, 11, 1, true},
    {3, 1, 12, 1, true},
    {3, 2, 13, 1, false},
    {3, 3, 14, 1, false},
    {3, 4, 15, 1, false},
    {4, 0, 5, 1, true},
    {4, 0, 15, -1, false},
    {4, 2, 7, -1, false},
    {4, 3, 9, -1, false},
    {4, 4, 12, 1, false},
    {5, 0, 2, -1, true},
    {5, 0, 14, 1, false},
    {5, 2, 4, 1, false},
    {5, 3, 12, -1, false},
    {5, 4, 9, -1, false},
    {6, 0, 1, 1, true},
    {6, 0, 13, -1, false},
    {6, 2, 12, 1, false},
    {6, 3, 4, 1, false},
    {6, 4, 7, 1, false},
    {7, 0, 0, 1, true},
    {7, 0, 10, -1, false},
    {7, 2, 9, 1, false},
    {7, 3, 7, -1, false},
    {7, 4, 4, 1, false},
}};

constexpr std::array<detail::kernel_term, 40> second{{
    {0, 7, 1, -1, true},
    {1, 6, 1, -1, false},
    {2, 5, 1, 1, false},
    {3, 0, 0, -1, true},
    {3, 5, 2, 1, true},
    {3, 6, 3, 1, true},
    {3, 7, 4, -1, true},
    {4, 0, 1, -1, false},
    {5, 4, 1, -1, false},
    {6, 1, 0, -1, false},
    {6, 4, 2, -1, false},
    {6, 6, 4, 1, false},
    {6, 7, 3, 1, false},
    {7, 1, 1, -1, false},
    {8, 2, 0, -1, false},
    {8, 4, 3, -1, false},
    {8, 5, 4, -1, false},
    {8, 7, 2, -1, false},
    {9, 2, 1, -1, false},
    {10, 0, 4, -1, false},
    {10, 1, 3, 1, false},
    {10, 2, 2, -1, false},
    {10, 7, 1, 1, false},
    {11, 3, 0, 1, false},
    {11, 4, 4, -1, false},
    {11, 5, 3, 1, false},
    {11, 6, 2, -1, false},
    {12, 3, 1, 1, false},
    {13, 0, 3, 1, false},
    {13, 1, 4, 1, false},
    {13, 3, 2, 1, false},
    {13, 6, 1, 1, false},
    {14, 0, 2, -1, false},
    {14, 2, 4, 1, false},
    {14, 3, 3, 1, false},
    {14, 5, 1, -1, false},
    {15, 1, 2, -1, false},
    {15, 2, 3, -1, false},
    {15, 3, 4, 1, false},
    {15, 4, 1, 1, false},
}};

static_assert(first.size() + second.size() > 0);
