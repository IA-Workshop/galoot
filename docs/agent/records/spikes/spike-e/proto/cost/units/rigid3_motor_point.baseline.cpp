#include <array>
#include <cstddef>

#include <galoot/core.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct alg {
  static constexpr auto value = core::rigid<3>();
};

constexpr auto versor = core::blade_set<8>{{{{9U, -1}, {10U, -1}, {12U, -1}, {15U, 1}, {6U, 1}, {5U, -1}, {3U, 1}, {0U, 1}}}};
constexpr auto object = core::blade_set<4>{{{{1U, 1}, {2U, 1}, {4U, 1}, {8U, 1}}}};

static_assert(versor.size() + object.size() > 0);
