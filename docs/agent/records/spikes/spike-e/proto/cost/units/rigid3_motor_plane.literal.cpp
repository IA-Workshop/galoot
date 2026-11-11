#include <array>
#include <cstddef>

#include <galoot/core.hpp>

#include "../../collect.hpp"

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct alg {
  static constexpr auto value = core::rigid<3>();
};

constexpr std::array<detail::kernel_term, 28> first{{
    {0, 0, 3, -1, true},
    {0, 1, 5, 1, true},
    {0, 2, 6, 1, true},
    {0, 3, 0, 1, true},
    {1, 0, 5, -1, false},
    {1, 1, 3, -1, false},
    {1, 2, 7, 1, true},
    {1, 3, 1, 1, true},
    {2, 0, 6, -1, false},
    {2, 1, 7, -1, false},
    {2, 2, 3, -1, false},
    {2, 3, 2, 1, true},
    {3, 0, 7, 1, false},
    {3, 1, 6, -1, false},
    {3, 2, 5, 1, false},
    {3, 3, 4, -1, true},
    {4, 0, 4, 1, false},
    {4, 1, 2, 1, false},
    {4, 2, 1, -1, false},
    {5, 0, 2, -1, false},
    {5, 1, 4, 1, false},
    {5, 2, 0, 1, false},
    {6, 0, 1, 1, false},
    {6, 1, 0, -1, false},
    {6, 2, 4, 1, false},
    {7, 0, 0, 1, false},
    {7, 1, 1, 1, false},
    {7, 2, 2, 1, false},
}};

constexpr std::array<detail::kernel_term, 20> second{{
    {0, 0, 3, 1, true},
    {1, 1, 3, 1, false},
    {2, 2, 3, 1, false},
    {3, 0, 0, -1, true},
    {3, 1, 1, -1, true},
    {3, 2, 2, -1, true},
    {3, 7, 3, -1, false},
    {4, 3, 3, -1, false},
    {5, 0, 1, -1, false},
    {5, 1, 0, 1, false},
    {5, 3, 2, 1, false},
    {5, 6, 3, -1, false},
    {6, 0, 2, -1, false},
    {6, 2, 0, 1, false},
    {6, 3, 1, -1, false},
    {6, 5, 3, 1, false},
    {7, 1, 2, -1, false},
    {7, 2, 1, 1, false},
    {7, 3, 0, 1, false},
    {7, 4, 3, -1, false},
}};

constexpr auto summary =
    collect::summarize<8, 4, 8>(first, second);
constexpr auto result =
    collect::make_collected<summary, 8, 4>();
static_assert(result.products.size() > 0);
