#include <array>
#include <cstddef>

#include <galoot/core.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct alg {
  static constexpr auto value = core::rigid<3>();
};

constexpr std::array<detail::kernel_term, 36> first{{
    {0, 0, 7, -1, true},
    {0, 1, 6, -1, true},
    {0, 2, 5, 1, true},
    {0, 3, 0, -1, true},
    {0, 4, 1, 1, true},
    {0, 5, 2, 1, true},
    {1, 0, 6, 1, false},
    {1, 1, 7, -1, false},
    {1, 2, 3, -1, true},
    {1, 3, 1, -1, false},
    {1, 4, 0, -1, false},
    {1, 5, 4, 1, true},
    {2, 0, 5, -1, false},
    {2, 1, 3, 1, false},
    {2, 2, 7, -1, false},
    {2, 3, 2, -1, false},
    {2, 4, 4, -1, false},
    {2, 5, 0, -1, false},
    {3, 0, 3, -1, false},
    {3, 1, 5, -1, false},
    {3, 2, 6, -1, false},
    {3, 3, 4, 1, false},
    {3, 4, 2, -1, false},
    {3, 5, 1, 1, false},
    {4, 0, 0, -1, false},
    {4, 1, 1, 1, false},
    {4, 2, 2, 1, false},
    {5, 0, 1, -1, false},
    {5, 1, 0, -1, false},
    {5, 2, 4, 1, false},
    {6, 0, 2, -1, false},
    {6, 1, 4, -1, false},
    {6, 2, 0, -1, false},
    {7, 0, 4, 1, false},
    {7, 1, 2, -1, false},
    {7, 2, 1, 1, false},
}};

constexpr std::array<detail::kernel_term, 36> second{{
    {0, 0, 3, -1, true},
    {0, 1, 4, -1, true},
    {0, 2, 5, -1, true},
    {1, 0, 4, -1, false},
    {1, 1, 3, 1, false},
    {1, 3, 5, 1, false},
    {2, 0, 5, -1, false},
    {2, 2, 3, 1, false},
    {2, 3, 4, -1, false},
    {3, 1, 2, 1, true},
    {3, 2, 1, -1, true},
    {3, 3, 0, -1, true},
    {3, 5, 5, 1, false},
    {3, 6, 4, -1, false},
    {3, 7, 3, -1, false},
    {4, 1, 5, -1, false},
    {4, 2, 4, 1, false},
    {4, 3, 3, 1, false},
    {5, 0, 2, -1, false},
    {5, 2, 0, 1, false},
    {5, 3, 1, -1, false},
    {5, 4, 5, -1, false},
    {5, 6, 3, 1, false},
    {5, 7, 4, -1, false},
    {6, 0, 1, 1, false},
    {6, 1, 0, -1, false},
    {6, 3, 2, -1, false},
    {6, 4, 4, 1, false},
    {6, 5, 3, -1, false},
    {6, 7, 5, -1, false},
    {7, 0, 0, -1, false},
    {7, 1, 1, -1, false},
    {7, 2, 2, -1, false},
    {7, 4, 3, -1, false},
    {7, 5, 4, -1, false},
    {7, 6, 5, -1, false},
}};

static_assert(first.size() + second.size() > 0);
