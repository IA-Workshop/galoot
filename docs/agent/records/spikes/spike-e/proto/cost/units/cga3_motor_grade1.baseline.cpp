#include <array>
#include <cstddef>

#include <galoot/core.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct alg {
  static constexpr auto value = core::conformal<3>();
};

constexpr auto versor = core::blade_set<8>{{{{25U, -1}, {26U, -1}, {28U, -1}, {31U, 1}, {22U, 1}, {21U, -1}, {19U, 1}, {16U, 1}}}};
constexpr auto object = core::blade_set<5>{{{{1U, 1}, {2U, 1}, {4U, 1}, {8U, 1}, {16U, 1}}}};

static_assert(versor.size() + object.size() > 0);
