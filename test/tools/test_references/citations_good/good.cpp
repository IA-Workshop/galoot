// Fixture for tools/test_references.py citations: text that resembles a
// citation but is mathematics, a published source, or plain English, so the
// check must pass. Not compiled.
//
// A C3 rotation composed three times is the identity, as is a C2 rotation
// composed twice; the Klein group is V4 = C2 x C2, and S2 is the sphere.
// Cl(1,1) is isomorphic to M2(R), the 2 by 2 real matrices.
// The geometric product follows Dorst, Fontijne, and Mann, Geometric Algebra
// for Computer Science (2007), §6.1 and chapter 7.
// The decision falls to exact arithmetic when the error bound admits zero.
// Each generated plan lists the terms of one product.
// Note 1: the findings of the scan are printed one per line.
// Namespaces are left to review; this header states its conventions here.
// The performance cores run two threads each.
#include <doctest/doctest.h>

TEST_CASE("a rotor in R(3, 0, 1) composed with its reverse is the scalar 1") {}
