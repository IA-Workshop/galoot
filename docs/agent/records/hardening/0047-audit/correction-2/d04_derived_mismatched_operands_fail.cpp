// A class derived from an mv, as one operand of a binary operation whose
// other operand is an mv of another scalar type or another algebra. The
// rejecting overloads of these operations require
// detail::mismatched_operands<X, Y>, which asks multivector<X> and
// multivector<Y>, and multivector is true of an mv type only, not of a class
// derived from one. -DOP=<k> picks the operation, -DFORM=<f> the operands:
//   OP  1 geometric_product   2 wedge         3 *           4 ^
//       5 geometric_antiproduct 6 antiwedge   7 %           8 &
//       9 +                  10 -            11 dot        12 antidot
//      13 project            14 antiproject  15 transform  16 compose
//   FORM 1  derived (double) on the left, an mv over float on the right
//        2  an mv over float on the left, derived (double) on the right
//        3  derived (R(3,0,0)) on the left, an mv of R(4,0,0) on the right
//        4  the mv itself (double) on the left, an mv over float: the
//           library's own message, for comparison
//        5  derived (double) on the left, the mv over double on the right:
//           a valid call, which builds
// Every form but 5 must not compile.
#include <galoot/core.hpp>

namespace core = galoot::core;

struct r3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
struct r4 {
  static constexpr auto value = core::R<4, 0, 0>();
};

using V = core::mv<r3, core::blades<r3, "e1 e2 e3">, double>;
using Vf = core::mv<r3, core::blades<r3, "e1 e2 e3">, float>;
using W = core::mv<r4, core::blades<r4, "e1 e2 e3">, double>;
struct D : V {
  using V::V;
};

#if FORM == 1
#define LEFT D{1.0, 2.0, 3.0}
#define RIGHT Vf{1.0f, 2.0f, 3.0f}
#elif FORM == 2
#define LEFT Vf{1.0f, 2.0f, 3.0f}
#define RIGHT D{1.0, 2.0, 3.0}
#elif FORM == 3
#define LEFT D{1.0, 2.0, 3.0}
#define RIGHT W{1.0, 2.0, 3.0}
#elif FORM == 4
#define LEFT V{1.0, 2.0, 3.0}
#define RIGHT Vf{1.0f, 2.0f, 3.0f}
#elif FORM == 5
#define LEFT D{1.0, 2.0, 3.0}
#define RIGHT V{1.0, 2.0, 3.0}
#endif

void use() {
#if OP == 1
  (void)core::geometric_product(LEFT, RIGHT);
#elif OP == 2
  (void)core::wedge(LEFT, RIGHT);
#elif OP == 3
  (void)(LEFT * RIGHT);
#elif OP == 4
  (void)(LEFT ^ RIGHT);
#elif OP == 5
  (void)core::geometric_antiproduct(LEFT, RIGHT);
#elif OP == 6
  (void)core::antiwedge(LEFT, RIGHT);
#elif OP == 7
  (void)(LEFT % RIGHT);
#elif OP == 8
  (void)(LEFT & RIGHT);
#elif OP == 9
  (void)(LEFT + RIGHT);
#elif OP == 10
  (void)(LEFT - RIGHT);
#elif OP == 11
  (void)core::dot(LEFT, RIGHT);
#elif OP == 12
  (void)core::antidot(LEFT, RIGHT);
#elif OP == 13
  (void)core::project(LEFT, RIGHT);
#elif OP == 14
  (void)core::antiproject(LEFT, RIGHT);
#elif OP == 15
  (void)core::transform(LEFT, RIGHT);
#elif OP == 16
  (void)core::compose(LEFT, RIGHT);
#endif
}

int main() { return 0; }
