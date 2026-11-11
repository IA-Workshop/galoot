#include <array>
#include <cstddef>

#include <galoot/core.hpp>

#include "../../collect.hpp"

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct alg {
  static constexpr auto value = core::conformal<3>();
};

constexpr auto versor = core::blade_set<16>{{{{1U, 1}, {2U, 1}, {4U, 1}, {8U, 1}, {16U, 1}, {7U, 1}, {11U, 1}, {19U, 1}, {13U, 1}, {21U, 1}, {25U, 1}, {14U, 1}, {22U, 1}, {26U, 1}, {28U, 1}, {31U, 1}}}};
constexpr auto object = core::blade_set<10>{{{{3U, 1}, {5U, 1}, {9U, 1}, {17U, 1}, {6U, 1}, {10U, 1}, {18U, 1}, {12U, 1}, {20U, 1}, {24U, 1}}}};

static_assert(detail::first_kernel<alg, versor, object, false>.size() > 0);
static_assert(detail::second_kernel<alg, versor, object, true>.size() > 0);
static_assert(
    collect::collected_transform<alg, versor, object>.products.size() > 0);
