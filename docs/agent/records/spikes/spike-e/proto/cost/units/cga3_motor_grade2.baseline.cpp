#include <array>
#include <cstddef>

#include <galoot/core.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct alg {
  static constexpr auto value = core::conformal<3>();
};

constexpr auto versor = core::blade_set<8>{{{{25U, -1}, {26U, -1}, {28U, -1}, {31U, 1}, {22U, 1}, {21U, -1}, {19U, 1}, {16U, 1}}}};
constexpr auto object = core::blade_set<10>{{{{3U, 1}, {5U, 1}, {9U, 1}, {17U, 1}, {6U, 1}, {10U, 1}, {18U, 1}, {12U, 1}, {20U, 1}, {24U, 1}}}};

