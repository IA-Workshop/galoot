// A multivector of R(10, 0, 0), whose blades have no one-digit names, formatted
// into auto s. This file must not compile.
#include <format>
#include <string>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/format.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct r10 {
  static constexpr auto value = core::R<10, 0, 0>();
};

// e1 alone, written by mask because names reach only e9: bit 0 stands for
// e1.
inline constexpr core::blade_set<1> e1_layout{{{{.mask = 1, .sign = 1}}}};
using Wide = core::mv<r10, e1_layout, double>;

[[maybe_unused]] void use(const Wide& x) { auto s = std::format("{}", x); }

} // namespace
