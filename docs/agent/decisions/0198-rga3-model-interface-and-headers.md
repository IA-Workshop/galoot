# 0198 The rga3 model's public interface, its headers, and its five messages

- Status: recorded. The names, the alias templates, the scalar rules, and `rotation` at run time only were ruled by the owner on 2026-11-07 ("accept all recommendations", decision 0196 item 6: plan 0023 O2 to O7). The fifth catch-all (item 5) and the header split (item 1) are the implementer's choices, open to the owner and to the blind review (C18).
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), implementer of WU16
- Bears on: plan 0023 R1 to R6, O2 to O7, criterion 9; decisions 0162 item 9, 0187, 0196, 0197; CONVENTIONS 1.6, 5.1 to 5.3, 5.7, 5.8; STYLE S6, S10, S11

## Context

Plan 0023 proposed the model's names (R2), alias templates over derived classes (R3), the scalar rules of `rotation` and `translation` (R4, R5), four messages (R6), and a split into two headers (R1). The owner accepted the recommendations (decision 0196 item 6). The code is `9900399` (objects), `9813525` and `d44ec64` (motions), `554d3ba` (include comments); nothing under `include/galoot/core/` changed.

## Decision

1. **Headers.** `include/galoot/rga3/objects.hpp` (the tag, the layouts, the aliases; includes `<concepts>` and `<galoot/core.hpp>`), `include/galoot/rga3/motions.hpp` (`rotation`, `translation`, and their catch-alls; includes `<cmath>`, `<concepts>`, `<type_traits>`, the core, and `objects.hpp`), and the umbrella `include/galoot/rga3.hpp`, all in `galoot::rga3`, as R1 proposed. The split is kept: a user of the objects alone does not parse `<cmath>` (P3); it costs one file. Both model headers mark their include of `core.hpp` as an export to include-cleaner, as `core.hpp` marks its own, so that a user of a model header may use the core's names through it. The target is the INTERFACE library `galoot_rga3` (`galoot::rga3`), linking `galoot_core`, with its own header set compiled header by header.
2. **Names, as R2.** `algebra` (value `core::rigid<3>()`); `point_layout`, `line_layout`, `plane_layout`, `motor_layout`, `flector_layout` in CONVENTIONS 1.6's oriented names and 5.8's orders; `Point<T>`, `Line<T>`, `Plane<T>`, `Motor<T>`, `Flector<T>` as alias templates of `core::mv<algebra, ..._layout, T>`, constrained by `std::destructible` as `mv` is, with no default scalar (O2, O3, O4). No `Magnitude`, norm, `unitize`, `exp`, `log`, or concepts per object class.
3. **`rotation(l, theta)`.** For `std::floating_point T`, `l` a `Line<T>`, `theta` of type `T` exactly; returns `Motor<T>` holding l sin(θ/2) on the line's blades and cos(θ/2) on e1234, 0 on 1. `[[nodiscard]] inline`, not `constexpr` (O7). Its comment states that l is assumed unitized and a line, unchecked, and what a violation gives. Found by argument-dependent lookup through its `Line` argument.
4. **`translation(dx, dy, dz)`.** For `core::ring_scalar T` that is not `std::integral`, three arguments of one type; `[[nodiscard]] constexpr`; returns `Motor<T>` with ½d on e23, e31, e12 and 1 on e1234 (O6, 5.7). Called qualified, since plain scalars name no namespace of the model; the comment says so.
5. **Five catch-alls, each a `static_assert` that is the only error** (decision 0033's form P: a requires-expression asking a rejected call is told yes). The four of R6 (no `std::sin` and `std::cos`; an angle of another type; an axis that is not a `Line`; an integer scalar for `translation`) and a fifth, added at `d44ec64` and tested by `bf3cbbb`: `translation` over scalars of two types, such as `translation(1.0, 0, 2)`, which before found no candidate and listed both overloads with their deduction failures. Its message names the mix and the fix, and the compiler prints the two types that differ. The texts as printed are in `records/diagnostics/wu16-messages.md`.
6. **Pointers are not scalars.** `62ef7b9` adds probes that `translation` over `const char*`, alone or mixed with `int`, is not a valid call. They pin the `ring_scalar` constraints of the integer and mixed-type catch-alls, which no other test pinned (two mutants survived without them, `records/mutations/0023-rga3/`).

## Options weighed

- One header in place of two (R1's alternative): one file fewer, but every includer of the objects parses `<cmath>`. The split was kept; its cost is not measured separately (`records/performance/rga3-cost/` measures only the umbrella).
- No fifth catch-all: the mixed call is still rejected, but with GCC's and Clang's lists of candidates and no message of the library. The catch-all costs one overload and one test. Chosen for C9.
- Requires-clauses on the catch-alls: six mutants that remove them survive as equivalent (partial ordering picks the same overload). They are kept, since each states the case its catch-all rejects.

## Consequences

- A user writes `galoot::rga3::translation(...)` qualified and `rotation(l, theta)` unqualified; the README's examples do both.
- GCC's longest diagnostic lines spell the full `mv` types (R3's known cost of aliases); Clang spells a non-`Line` axis by its blade masks on the first error line (444 to 462 characters).
- The core's examples keep their own motor order (plan 0023 section 12).
