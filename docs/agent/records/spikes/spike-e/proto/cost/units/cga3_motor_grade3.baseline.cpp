#include <array>
#include <cstddef>

#include <galoot/core.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct alg {
  static constexpr auto value = core::conformal<3>();
};

constexpr auto versor = core::blade_set<8>{{{{25U, -1}, {26U, -1}, {28U, -1}, {31U, 1}, {22U, 1}, {21U, -1}, {19U, 1}, {16U, 1}}}};
constexpr auto object = core::blade_set<10>{{{{7U, 1}, {11U, 1}, {19U, 1}, {13U, 1}, {21U, 1}, {25U, 1}, {14U, 1}, {22U, 1}, {26U, 1}, {28U, 1}}}};

