#include <galoot/presets/rga3.hpp>
namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Plane = rga3::Plane<double>;
[[gnu::target("arch=x86-64")]] auto fallback(const Point& p, const Plane& g) { return core::project(p, g); }
