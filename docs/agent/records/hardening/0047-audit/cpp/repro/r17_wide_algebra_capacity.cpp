// The bulk dual of a vector in R(GEN, 0, 0), built with MaxN 32. The plan's
// term lists get the capacity product_capacity gives, 2^(GEN - 1) for a
// vector times the antiscalar, though that product has at most GEN terms.
// Build with -DGEN=8 to 32. Measured: GCC builds every GEN to 32 in about
// 0.2 s and 85 MiB (its constant evaluator holds the unused places of an
// array cheaply); Clang builds to 21 and from 22 stops with "cannot allocate
// array; evaluated array bound 2097152 exceeds the limit (1048576)", four
// errors in all. The rows of r17_runs.tsv are these compiles.
#include <galoot/core.hpp>
#ifndef GEN
#define GEN 16
#endif
namespace core = galoot::core;
struct alg { static constexpr auto value = core::R<GEN, 0, 0, 32>(); };
using Vector = core::mv<alg, core::of_grade<GEN, 1>, double>;
auto dual(const Vector& v) { return core::bulk_dual(v); }
