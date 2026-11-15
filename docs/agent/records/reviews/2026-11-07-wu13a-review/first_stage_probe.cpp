#include <galoot/core.hpp>
#include <cstdio>
namespace core = galoot::core;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct deg3 { static constexpr auto value = core::from_gram({{0,1,0},{1,0,0},{0,0,0}}); };
struct r400 { static constexpr auto value = core::R<4,0,0>(); };
template <class Alg, core::blade_set V, core::blade_set X>
void show(const char* name) {
  std::printf("%-40s first restricted %zu, whole %zu\n", name,
    core::detail::first_kernel<Alg, V, X, false>.size(),
    core::detail::first_kernel<Alg, V, X, true>.size());
}
int main() {
  show<cga3, core::blades<cga3, "e1 e2 e3 e4 e5 e123 e124 e125 e134 e135 e145 e234 e235 e245 e345 e12345">, core::blades<cga3, "e1 e2 e3 e4 e5 e12345">>("cga3 even on e1 e2 e3 e4");
  show<cga3, core::blades<cga3, "1 e12 e13 e14 e15 e23 e24 e25 e34 e35 e45 e1234 e1235 e1245 e1345 e2345">, core::blades<cga3, "e1 e2 e3 e4 e5 e12345">>("cga3 odd on e1 e2 e3 e4");
  show<cga3, core::blades<cga3, "e1 e2 e3 e4 e5 e123 e124 e125 e134 e135 e145 e234 e235 e245 e345 e12345">, core::blades<cga3, "1 e1234 e1235 e1245 e1345 e2345">>("cga3 even on 1 e1234 e12");
  show<cga3, core::blades<cga3, "1 e12 e13 e14 e15 e23 e24 e25 e34 e35 e45 e1234 e1235 e1245 e1345 e2345">, core::blades<cga3, "1 e1234 e1235 e1245 e1345 e2345">>("cga3 odd on 1 e1234 e12");
  show<deg3, core::blades<deg3, "e1 e2 e3 e123">, core::blades<deg3, "1">>("deg3 even on 1");
  show<deg3, core::blades<deg3, "1 e12 e13 e23">, core::blades<deg3, "1">>("deg3 odd on 1");
  show<deg3, core::blades<deg3, "e1 e2 e3 e123">, core::blades<deg3, "e1 e2 e3">>("deg3 even on e1 e2 e3");
  show<deg3, core::blades<deg3, "1 e12 e13 e23">, core::blades<deg3, "e1 e2 e3">>("deg3 odd on e1 e2 e3");
  show<deg3, core::blades<deg3, "e1 e2 e3 e123">, core::blades<deg3, "e12 e13 e23">>("deg3 even on e12 e13 e23");
  show<deg3, core::blades<deg3, "1 e12 e13 e23">, core::blades<deg3, "e12 e13 e23">>("deg3 odd on e12 e13 e23");
  show<deg3, core::blades<deg3, "e1 e2 e3 e123">, core::blades<deg3, "e123">>("deg3 even on e123");
  show<deg3, core::blades<deg3, "1 e12 e13 e23">, core::blades<deg3, "e123">>("deg3 odd on e123");
  show<rga3, core::blades<rga3, "1 e12 e13 e14 e23 e24 e34 e1234">, core::blades<rga3, "1 e1234">>("rga3 even on 1 e1234");
  show<rga3, core::blades<rga3, "e1 e2 e3 e4 e123 e124 e134 e234">, core::blades<rga3, "1 e1234">>("rga3 odd on 1 e1234");
  show<rga3, core::blades<rga3, "1 e12 e13 e14 e23 e24 e34 e1234">, core::blades<rga3, "e1 e2 e3 e4">>("rga3 even on e1 e2 e3 e4");
  show<rga3, core::blades<rga3, "e1 e2 e3 e4 e123 e124 e134 e234">, core::blades<rga3, "e1 e2 e3 e4">>("rga3 odd on e1 e2 e3 e4");
  show<rga3, core::blades<rga3, "1 e12 e13 e14 e23 e24 e34 e1234">, core::blades<rga3, "e12 e13 e14 e23 e24 e34">>("rga3 even on e12 e13 e14 e23 e24 e34");
  show<rga3, core::blades<rga3, "e1 e2 e3 e4 e123 e124 e134 e234">, core::blades<rga3, "e12 e13 e14 e23 e24 e34">>("rga3 odd on e12 e13 e14 e23 e24 e34");
  show<rga3, core::blades<rga3, "1 e12 e13 e14 e23 e24 e34 e1234">, core::blades<rga3, "e123 e124 e134 e234">>("rga3 even on e123 e124 e134 e234");
  show<rga3, core::blades<rga3, "e1 e2 e3 e4 e123 e124 e134 e234">, core::blades<rga3, "e123 e124 e134 e234">>("rga3 odd on e123 e124 e134 e234");
  show<rga3, core::blades<rga3, "e23">, core::blades<rga3, "e1 e2 e3 e4">>("rga3 e23 on point");
  show<rga3, core::blades<rga3, "e23 e31 e12 1">, core::blades<rga3, "e1 e2 e3 e4">>("rga3 bulk motor on point");
  show<rga3, core::blades<rga3, "e23 e31 e12 1">, core::blades<rga3, "e41 e42 e43 e23 e31 e12">>("rga3 bulk motor on line");
  show<rga3, core::blades<rga3, "e1 e2 e3">, core::blades<rga3, "e1 e2 e3 e4">>("rga3 bulk flector on point");
  show<rga3, core::blades<rga3, "e123">, core::blades<rga3, "e423 e431 e412 e321">>("rga3 e123 on plane");
  constexpr auto motor = core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
  constexpr auto plane5 = core::blades<cga3, "e4235 e4315 e4125 e3215">;
  constexpr auto cmotor = core::compose_layout<cga3, plane5, plane5>;
  show<rga3, motor, core::blades<rga3, "e1 e4">>("rga3 motor on {e1,e4}");
  show<rga3, motor, core::blades<rga3, "e1">>("rga3 motor on {e1}");
  show<rga3, motor, core::blades<rga3, "1">>("rga3 motor on {1}");
  show<r400, core::blades<r400, "1 e1234">, core::blades<r400, "1">>("R400 1+I on {1}");
  show<cga3, cmotor, core::of_grade<5, 1>>("cga3 motor on round point");
  show<cga3, cmotor, core::of_grade<5, 2>>("cga3 motor on grade 2");
  show<cga3, core::of_grade<5,4>, core::of_grade<5, 1>>("cga3 plane-ish on vectors");
}
