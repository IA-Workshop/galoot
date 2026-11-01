# 0029 Floating-point options in the presets

- Status: accepted; that release presets leave NDEBUG undefined is recorded in decision 0041
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, WU1), under C18; a decision record takes effect when recorded, and the owner may reverse it.

## Context

S15 asks the test build to state its floating-point options, including contraction of multiply-add, so that declared variation (V14) has a recorded cause; DoD-9 asks the presets to declare `-ffp-contract`. Both compilers contract by default: probed in WU1, `double f(double a, double b, double c) { return a * b + c; }` at `-std=c++23 -O2 -march=haswell` compiles to one `vfmadd` on GCC 16.2.1 and on Clang 23.1.4, and to none with `-ffp-contract=off`. Their rules for when they contract differ (GCC's `fast` contracts across statements, Clang's default `on` only within one expression; from the compilers' documentation, not probed). The owner approved `-ffp-contract=off` in every preset (decision 0020, item 5).

## Decision

Every preset's `CMAKE_CXX_FLAGS` has `-ffp-contract=off`. No preset uses `-ffast-math` or any option that relaxes IEEE 754 semantics (C7, V14, S15). `-O2` is the release level, `-O3` appears only in the `*-measure-o3` comparison presets, and each preset states its level in its own body.

## Consequences

Rounded results agree between the compilers wherever contraction was the only difference. A future test that wants contracted code for P6 does so in its own preset, recorded by decision.
