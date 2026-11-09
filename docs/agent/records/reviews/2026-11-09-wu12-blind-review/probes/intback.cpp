#include <cstdint>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct r410 { static constexpr auto value = core::R<4, 1, 0>(); };
struct cga3_to_r410 { using source = cga3; using target = r410; static constexpr auto value = core::conformal_change<3>(); };
using Y = core::mv<r410, core::blades<r410, "e1 e2 e3">, std::int64_t>;
auto f(const Y& y) { return core::change_basis_back<cga3_to_r410>(y); }
