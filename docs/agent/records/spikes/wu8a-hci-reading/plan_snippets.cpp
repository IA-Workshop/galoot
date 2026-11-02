// The usage snippets of plan 0016 section 4 that WU8a can compile, copied as
// written, with the statements that are not declarations placed in a
// function (at namespace scope they are not C++). SNIPPET chooses the
// block: 1 is "Declaring a type, making a value, and reading and writing it
// by name" as written; 2 is the same with the statements moved into a
// function; 3 is the scalar and narrowing lines of "What other misuse looks
// like".
#include <galoot/core.hpp>

#ifndef SNIPPET
#define SNIPPET 2
#endif

namespace gc = galoot::core;

// An algebra is a tag type that carries its metric: rigid(3) is R(3,0,1).
struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};

// A line stores its direction on e41, e42, e43 and its moment on e23, e31,
// e12; each name is the product of its generators in the order written.
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = gc::mv<rga3, line_layout, double>;

#if SNIPPET == 1
Line axis{0.0, 0.0, 1.0, 0.0, 0.0, 0.0}; // the z axis through the origin
double vz = gc::get<"e43">(axis);        // 1.0
gc::get<"e23">(axis) = 2.0;              // write a coefficient by its name
Line none;                               // every coefficient zero
bool moved = axis != Line{0.0, 0.0, 1.0, 0.0, 0.0, 0.0}; // exact comparison
#elif SNIPPET == 2
bool snippet() {
  Line axis{0.0, 0.0, 1.0, 0.0, 0.0, 0.0}; // the z axis through the origin
  double vz = gc::get<"e43">(axis);        // 1.0
  gc::get<"e23">(axis) = 2.0;              // write a coefficient by its name
  Line none;                               // every coefficient zero
  bool moved = axis != Line{0.0, 0.0, 1.0, 0.0, 0.0, 0.0}; // exact comparison
  return moved && vz == 1.0 && none == Line{};
}
#elif SNIPPET == 3
bool snippet() {
  float f = 0.1f;
  double d = 0.1;
  gc::mv<rga3, gc::blades<rga3, "e1 e2">, float> v{d, f};
  return v.coefficients[0] == f;
}
#endif

int main() { return 0; }
