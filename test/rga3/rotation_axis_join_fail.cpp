// A rotation about the join p ^ q of two points, which is a multivector on
// the wedge's blades and must be converted to a Line first. This file must
// not compile.
#include <galoot/rga3.hpp>

namespace {

namespace rga3 = galoot::rga3;

[[maybe_unused]] void use(const rga3::Point<double>& p,
                          const rga3::Point<double>& q) {
  [[maybe_unused]] const auto r = rga3::rotation(p ^ q, 0.5);
}

} // namespace
