# 0010 Spike C: diagnostics for five misuse cases

Date: 2026-10-31. Checked by: a spike subagent (Claude Opus 5.5) working for the owner. Scope: plan 0001, section 9, Spike C. It records what a user sees on GCC 16.2.1 and Clang 23.1.4 (decision 0006) for five misuses of a throwaway layer shaped as decision 0021 fixes it: the algebra is a tag type that carries its metric, and blade sets are class-type template arguments. It compares the raw deduction or constraint failure with a constrained catch-all overload whose body is a `static_assert` with a literal message, plus four variants, and checks whether C++26 computed `static_assert` messages are usable under the S4 flags. Nothing here is library code; no file under `include/` exists or was touched.

Revision and provenance. The final runs were made on the files as kept, at revisions `aa8051c0be1d` to `63e2014f8bdc` (`develop`). `runs.tsv` marks them `-dirty` only because `tools/timed` appends to the tracked `runs.tsv` itself; the spike files are untracked. Earlier exploratory runs at `7d9505e4` and `96eb5fc7` used intermediate versions of the fixture, and their results are superseded. The owner committed governance, decision, plan, and record changes while the spike ran (`783ff86` to `63e2014`; `git diff --name-only 007cc32 63e2014`). Those commits touch no compiler, `tools/` script, or Spike A file that this spike reads, and STYLE S7 now states the hybrid that the fixture already follows. Sources and results live in `docs/agent/records/spikes/spike-cdf/`, which is not committed as I write this. Until it is, the figures are labeled not reproducible (V10).

How each claim was checked:

- **Local**: compiled on this machine by the scripts named, with `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -fno-diagnostics-color`, unless a different standard is named.
- **Inferred**: my reasoning, not checked.

No external source was read. The paper numbers given for the C++26 features (P2741, P2573) and the defect report (P2593) are from my own knowledge, not read in this session (V9); every claim about compiler behavior rests on the local compiles alone. Diagnostic texts include absolute paths, so byte counts depend on the checkout location; line counts do not.

## Summary

1. C++26 computed `static_assert` messages (P2741) are not usable under the S4 flags in C++23 mode. Both compilers reject the declaration itself, even in a correct program, and even when the assertion holds: GCC with "‘static_assert’ with non-string message only available with ‘-std=c++2c’" [-Werror=c++26-extensions], and Clang with "'static_assert' with a user-generated message is a C++26 extension" [-Werror,-Wc++26-extensions]. `= delete("reason")` (P2573) fails the same way. Under `-std=c++26` both work. This confirms the architecture finding of plan 0001 section 2 for Clang 23 and extends it to GCC 16 (Local).
2. In C++23 mode Clang defines `__cpp_static_assert` as 202306L and `__cpp_deleted_function` as 202403L, which advertise the C++26 features it accepts only as extensions. GCC defines 201411L and leaves the second undefined. So a guard on the feature-test macro, as STYLE S2 prescribes for later-edition features, would select the C++26 path on Clang and then fail under `-Werror`. A guard on `__cplusplus > 202302L` would be needed instead (Local; the guard is Inferred).
3. `static_assert(false, "literal")` in a template body (P2593, adopted as a defect report) works in C++23 mode on both compilers, under S4 and under bare `-std=c++23` (Local).
4. The pattern as planned, a constrained catch-all with `void` return and a literal `static_assert` (form K below), does print the message. But for gp, transform, and get, the first error is a follow-on, "deduced type ‘const void’ for ‘r’ is incomplete" (GCC) or "variable has incomplete type 'const void'" (Clang). The call's type is checked at once, while the catch-all's body is instantiated later. Giving the catch-all a deduced `auto` return type (form A) moves the message to the first error. Returning a placeholder of a plausible type after the assertion (form P) leaves exactly one error: 9 lines on GCC and 8 on Clang for every case that a catch-all handles (Local).
5. Every catch-all whose body asserts (K, A, P, G) makes generic code see the misuse as well-formed. `requires { gp(x, y); }` and `std::is_convertible_v<Motor, Point>` are true for all five misuses on both compilers. With A and P, the probe itself becomes a hard error, because deducing the return type instantiates the body. The raw form (R) and a deleted catch-all (D) answer correctly (Local).
6. Raw failures already name the algebras for case 1 on both compilers ("deduced conflicting types for parameter ‘Alg’ (‘cdf::rga3’ and ‘cdf::r400’)"). On Clang they are also short and readable for constraints: case 4 ends in "because 'one_parity(blade_set<2>{{{0, 1}}, {{1, 1}}})' evaluated to false", and case 5 in "because 'a * b' would be invalid". GCC's raw constraint failures run 25 to 40 lines, and GCC prints blade sets through libstdc++ internals (`std::array<unsigned int, 4>{std::__array_traits<unsigned int, 4>::_Type{1, 2, 4, 8}}`). Neither compiler prints blade names, only masks and signs. The exception is Clang's "aka" text, which keeps the alias as written, for example `mv<rga3, blades_of<rga3, "1", "e41", "e42", "e43", "e23", "e31", "e12", "e1234">>` in cases 31 and 32 (Local).
7. An invalid blade name (case 22) fails inside `consteval` parsing, and the decision 0012 helper's name carries the message: GCC's first error is "call to non-‘constexpr’ function ‘void cdf::blade_name_is_not_valid_in_this_algebra()’", 14 lines. On Clang it is a note on line 12 of 34, under "constexpr variable 'blades_of<cdf::rga3, fixed_string<3>{{"e1"}}, fixed_string<3>{{"e5"}}>' must be initialized by a constant expression". The form of the overloads makes no difference here (Local).

Recommendation for WU15 (Inferred from the measurements):

- Use form P for functions: a constrained catch-all, mutually exclusive with the real overload, with a deduced `auto` return type, whose body is `static_assert(false, "<message in the terms of the algebra>");` followed by `return <placeholder>;` of the type the real overload would plausibly return (the first operand, or `T{}`). It puts the library's message on GCC's line 6 and Clang's line 2 (each compiler's first line is "In file included from …"), and it is the only error. The instantiation context printed with it names the operand types, and with them the algebra tags (`[with X = mv<rga3, …>; Y = mv<r400, …>]`). That supplies what a literal cannot.
- For conversion functions, form K is enough: their return type is fixed, so there is no follow-on error.
- Record in WU15 that catch-alls defeat detection. Galoot's own concepts must test the data (`same_algebra`, `ring_scalar`, `has_blade`) and never `requires { gp(a, b); }`. The documentation should say that `std::is_convertible_v` is true for a lossy pair, although the conversion never compiles. If the owner prefers honest detection to messages, form D (`= delete`) is honest and almost as short (GCC 11 lines; Clang 7 to 29), but it has no message.
- Keep messages free of quote characters (plan 0002, risk 8). The literals used here contain none.
- Upgrade path: when the baseline moves to C++26, form G gives computed messages that name the algebras and blades ("gp: the operands belong to different algebras, rga3 and r400"; "get<e1>: not in the blade set {-e14, -e24, -e34, e23, -e13, e12}"). Until then it is unusable (finding 1). A pragma or `-isystem` would silence the error (below), but either one relies on a compiler extension, which S2 forbids.

## The fixture and the forms

`fixture.hpp` (in `records/spikes/spike-cdf/`) reuses Spike A's `gen.hpp` for metrics and blade products. It defines:

- `fixed_string<N>`, a structural class with a `consteval` constructor from a string literal. `parse_name` turns "e41" into the mask {1, 4} with sign −1 relative to e14 (CONVENTIONS 1.1, 1.6). "1" names the scalar, and only single-digit indices are accepted.
- The algebra tags `rga3` (R(3,0,1)) and `r400` (R(4,0,0)), each with a `static constexpr` metric `value` and a `name`. The two have the same n and are different algebras.
- `blade_set<K>` holds masks and orientation signs. `blades_of<Alg, "e41", …>` builds one from names.
- `mv<Alg, blade_set B, T = double>`, an aggregate holding `std::array<T, B.size>`. An explicit conversion function, constrained to non-lossy targets, keeps it an aggregate (S8).
- `gp`, constrained by `ring_scalar<T>`; `transform`, a stub constrained by `one_parity(V)` (CONVENTIONS 4.4); and `get<"name">`, constrained by `has_blade`.

`SPIKE_FORM` selects how misuse is rejected:

| Form | Rejecting overload |
| --- | --- |
| R | None: the constrained overload alone (raw deduction or constraint failure) |
| K | R plus a constrained catch-all returning `void`, body `static_assert(false, "literal")` |
| A | K with a deduced `auto` return type |
| P | A, plus `return <placeholder>;` after the assertion |
| D | A constrained catch-all declared `= delete`, without a message |
| G | K with a computed message (C++26, P2741) |

`diag.cpp` writes each misuse as a user would, `auto r = gp(p, v); return r.c[0] + …;`:

| Case | Misuse |
| --- | --- |
| 1 | `gp` of an rga3 point and an R(4,0,0) vector |
| 21 | `get<"e1">(line)`: a valid blade not in the set (case 2) |
| 22 | `blades_of<rga3, "e1", "e5">`: a name not valid in the algebra (case 2) |
| 23 | `get<"e14">(line)`, where Line holds e41 = −e14: the opposite orientation (case 2) |
| 31 | `const Point r = motor;`: implicit lossy conversion (case 3) |
| 32 | `static_cast<Point>(motor)`: explicit, still lossy (case 3) |
| 4 | `transform(p, v)` with v on {1, e1}, mixed parity |
| 5 | `gp(a, a)` over a scalar `length` with + and − but no * |
| 10 | Control: the correct uses, which must compile in every form |
| 9 | Detection: six `requires`-expression and `is_convertible` answers, packed into one number |

The control compiles cleanly in forms R, K, A, P, and D on both compilers, and also under bare `-std=c++23 -pedantic-errors` in forms R and P (Local; `out/diag/pedantic.tsv`).

## Line counts

Lines / error lines / line on which the library's own message first appears ("-" when it does not appear). "c++23" is the S4 configuration; G is also shown under `-std=c++26`. From `out/diag/summary.tsv`, printed by `summarize.py c`.

GCC 16.2.1:

| Case | R | K | A | P | D | G (c++23) | G (c++26) |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 10 control | 0 / 0 / - | 0 / 0 / - | 0 / 0 / - | 0 / 0 / - | 0 / 0 / - | 18 / 4 / - | 0 / 0 / - |
| 1 different algebras | 15 / 1 / - | 13 / 2 / 10 | 13 / 2 / 6 | 9 / 1 / 6 | 11 / 1 / - | 30 / 6 / 26 | 13 / 2 / 10 |
| 21 blade not in set | 34 / 1 / - | 12 / 2 / 9 | 12 / 2 / 6 | 9 / 1 / 6 | 11 / 1 / - | 29 / 6 / 25 | 12 / 2 / 9 |
| 22 name not in algebra | 14 / 1 / 9 | 14 / 1 / 9 | 14 / 1 / 9 | 14 / 1 / 9 | 14 / 1 / 9 | 31 / 5 / 25 | 14 / 1 / 9 |
| 23 opposite orientation | 34 / 1 / - | 12 / 2 / 9 | 12 / 2 / 6 | 9 / 1 / 6 | 11 / 1 / - | 29 / 6 / 25 | 12 / 2 / 9 |
| 31 implicit lossy | 4 / 1 / - | 9 / 1 / 6 | 9 / 1 / 6 | 9 / 1 / 6 | 11 / 1 / - | 26 / 5 / 22 | 9 / 1 / 6 |
| 32 explicit lossy | 6 / 1 / - | 9 / 1 / 6 | 9 / 1 / 6 | 9 / 1 / 6 | 11 / 1 / - | 26 / 5 / 22 | 9 / 1 / 6 |
| 4 mixed parity | 25 / 1 / - | 13 / 2 / 10 | 13 / 2 / 6 | 9 / 1 / 6 | 11 / 1 / - | 30 / 6 / 26 | 13 / 2 / 10 |
| 5 scalar without * | 40 / 1 / - | 13 / 2 / 10 | 13 / 2 / 6 | 9 / 1 / 6 | 11 / 1 / - | 30 / 6 / 26 | 13 / 2 / 10 |

Clang 23.1.4:

| Case | R | K | A | P | D | G (c++23) | G (c++26) |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 10 control | 0 / 0 / - | 0 / 0 / - | 0 / 0 / - | 0 / 0 / - | 0 / 0 / - | 14 / 4 / - | 0 / 0 / - |
| 1 different algebras | 7 / 1 / - | 11 / 2 / 5 | 11 / 2 / 2 | 8 / 1 / 2 | 29 / 2 / - | 24 / 6 / 18 | 11 / 2 / 5 |
| 21 blade not in set | 22 / 1 / - | 11 / 2 / 5 | 11 / 2 / 2 | 8 / 1 / 2 | 28 / 2 / - | 24 / 6 / 18 | 11 / 2 / 5 |
| 22 name not in algebra | 34 / 3 / 12 | 34 / 3 / 12 | 34 / 3 / 12 | 34 / 3 / 12 | 34 / 3 / 12 | 46 / 7 / 24 | 34 / 3 / 12 |
| 23 opposite orientation | 22 / 1 / - | 11 / 2 / 5 | 11 / 2 / 2 | 8 / 1 / 2 | 28 / 2 / - | 24 / 6 / 18 | 11 / 2 / 5 |
| 31 implicit lossy | 13 / 1 / - | 8 / 1 / 2 | 8 / 1 / 2 | 8 / 1 / 2 | 7 / 1 / - | 20 / 5 / 14 | 8 / 1 / 2 |
| 32 explicit lossy | 17 / 1 / - | 8 / 1 / 2 | 8 / 1 / 2 | 8 / 1 / 2 | 11 / 1 / - | 20 / 5 / 14 | 8 / 1 / 2 |
| 4 mixed parity | 10 / 1 / - | 11 / 2 / 5 | 11 / 2 / 2 | 8 / 1 / 2 | 16 / 2 / - | 24 / 6 / 18 | 11 / 2 / 5 |
| 5 scalar without * | 13 / 1 / - | 11 / 2 / 5 | 11 / 2 / 2 | 8 / 1 / 2 | 25 / 2 / - | 24 / 6 / 18 | 11 / 2 / 5 |

## First diagnostic lines

Literal first line of the output. GCC's raw and deleted forms begin with the context line "diag.cpp: In function ‘double use(const cdf::rga3_types::Point&, …)’:". Every form that instantiates a fixture body begins with "In file included from diag.cpp:16:" on both compilers. Clang's raw and deleted forms begin directly with the error. So the first line that says anything is the first error line:

| Case | GCC, R | Clang, R | Both compilers, P (message) |
| --- | --- | --- | --- |
| 1 | no matching function for call to ‘gp(const cdf::rga3_types::Point&, const R400Vector&)’ | no matching function for call to 'gp' | static assertion failed: gp: the operands belong to different algebras |
| 21 | no matching function for call to ‘get<"e1">(const cdf::rga3_types::Line&)’ | no matching function for call to 'get' | static assertion failed: get<name>: no blade of this name, with this orientation, is in the blade set |
| 22 | call to non-‘constexpr’ function ‘void cdf::blade_name_is_not_valid_in_this_algebra()’ | constexpr variable 'blades_of<cdf::rga3, fixed_string<3>{{"e1"}}, fixed_string<3>{{"e5"}}>' must be initialized by a constant expression | (as R) |
| 23 | no matching function for call to ‘get<"e14">(const cdf::rga3_types::Line&)’ | no matching function for call to 'get' | as 21 |
| 31 | conversion from ‘mv<[...],cdf::blade_set<8>{…}>’ to non-scalar type ‘…’ requested | no viable conversion from 'const mv<[...], blade_set<8>{{{0, 9, 10, 12, 6, 5, 3, 15}}, …}>' to 'const mv<[...], blade_set<4>{…}>' | static assertion failed: conversion drops blades of its input: this is a lossy conversion, request it with truncate<Blades>(x) |
| 32 | could not convert ‘q’ from ‘const cdf::rga3_types::Motor’ {aka …} | no matching conversion for static_cast from 'const Motor' (aka 'const mv<rga3, blades_of<rga3, "1", "e41", …>>') to 'Point' … | as 31 |
| 4 | no matching function for call to ‘transform(const cdf::rga3_types::Point&, const Mixed&)’ | no matching function for call to 'transform' | static assertion failed: transform: the versor has blades of both parities, so it is neither even nor odd (CONVENTIONS 4.4) |
| 5 | no matching function for call to ‘gp(cdf::rga3_types::Vector<length>&, cdf::rga3_types::Vector<length>&)’ | no matching function for call to 'gp' | static assertion failed: gp: the scalar type must be the same for both operands and support +, -, * and conversion from an integer coefficient |

The reason for a raw failure comes in later lines. On GCC it is "the expression ‘cdf::one_parity(V) [with V = cdf::blade_set<2>{{{0, 1}}, {{1, 1}}}]’ evaluated to ‘false’" for case 4, and "the required expression ‘(a * b)’ is invalid" for case 5. On Clang it is the "because …" notes quoted in the summary. Full texts are in `out/diag/<compiler>-<std>-<form>-case<N>.txt`.

- **Claim.** In forms K and G, and in D on Clang, the void return of the catch-all raises a second error at the user's use of the result, and in K and G it comes first. Checked: Local (the error-line column, and the texts). Form A puts the message first. Form P removes the second error.
- **Claim.** GCC prints mask arrays that contain 0 correctly (`{0, 1}`, `{0, 9, 10, …}`); note 0008's misprint of nested metric rows did not appear for one-dimensional mask arrays. Checked: Local (grep of all GCC texts).
- **Claim.** With the computed messages of form G, the blade names come out canonical, because the fixture prints masks: "-e14" for e41, and "e" for the scalar. A library message would print the oriented names. Checked: Local; the remedy is Inferred.

## Detection

`out/diag/detect.tsv`, case 9. "Well-formed" means that generic code is told that the misuse compiles.

| Form | gp, two algebras | gp, scalar without * | get<"e1">(line) | is_convertible Motor to Point | transform, mixed parity | gp, correct | Hard errors from probing |
| --- | --- | --- | --- | --- | --- | --- | --- |
| R | ill-formed | ill-formed | ill-formed | ill-formed | ill-formed | well-formed | 0 |
| K | well-formed | well-formed | well-formed | well-formed | well-formed | well-formed | 0 |
| A | well-formed | well-formed | well-formed | well-formed | well-formed | well-formed | 4 |
| P | well-formed | well-formed | well-formed | well-formed | well-formed | well-formed | 4 |
| D | ill-formed | ill-formed | ill-formed | ill-formed | ill-formed | well-formed | 0 |
| G (c++26) | well-formed | well-formed | well-formed | well-formed | well-formed | well-formed | 0 |

The two compilers agree on every row (Local). For A and P, the four hard errors are the four function catch-alls. The conversion catch-all has a fixed return type, so probing it raises no error, but the answer is still wrong.

## C++26 messages under C++23 flags

`run_probes.sh`, results in `out/probes/summary.tsv`:

| Probe | GCC, S4 c++23 | GCC, bare c++23 | Clang, S4 c++23 | Clang, bare c++23 | Both, S4 c++26 |
| --- | --- | --- | --- | --- | --- |
| Computed `static_assert` message, assertion holds | error | warning | error | warning | compiles |
| `= delete("reason")`, unused | error | warning | error | compiles silently | compiles |
| `static_assert(false, "literal")` in a template, never instantiated | compiles | compiles | compiles | compiles | compiles |
| Computed message inside `#pragma … diagnostic ignored "-Wc++26-extensions"` | compiles (also with `-pedantic-errors`) | | compiles (also with `-pedantic-errors`) | | |
| Computed message in a header included with `-isystem` | compiles | | compiles | | |
| Computed message in a header included with `-I` | error | | error | | |

In the last three rows a failing assertion (`-DFK=5`) prints its computed message on both compilers.

- **Claim.** The pragma silences the extension diagnostic even when the template is instantiated outside the pragma region, so a library could wrap its computed messages in it. Checked: Local. It still uses a C++26 feature as a C++23 extension, which S2 ("Compiler extensions are disabled") forbids; I do not recommend it (Inferred).
- **Claim.** Under bare `-std=c++23`, Clang does not warn about `= delete("reason")` at all, while GCC warns. Checked: Local.

## Reproduction

From the repository root, serially. The pool was running, but no parallel build was needed. These compiles are diagnostics, not measurements, so they are not pinned.

```bash
S=docs/agent/records/spikes/spike-cdf
tools/timed test - spike-c:run_probes -- bash $S/run_probes.sh       # C++26 probes, 5 s
tools/timed test - spike-c:run_diag -- python3 -I $S/run_diag.py     # 126 compiles plus 12 detection compiles, 29 s
python3 -I $S/summarize.py c                                         # the tables above
```

No randomness is involved, so there are no seeds.

## Open questions

1. Catch-alls against honest detection: take P, with the documented consequence for `requires` and `is_convertible`, or D, honest but without a message? I recommend P (C9), but it is the owner's call because it shapes the public interface.
2. C++26 messages: when the baseline moves (S2, a decision), form G is the upgrade. Its guard must test `__cplusplus`, not `__cpp_static_assert`, on Clang.
3. Case 22's Clang output is 34 lines with the helper name on line 12. A `static_assert` in the name parser cannot help, because the parser runs in constant evaluation. Whether WU4 should validate names in a `consteval` function that a catch-all calls, to bring the name to the first line, was not tested.
4. GCC prints blade sets through libstdc++ internals (`std::__array_traits<…>::_Type`). A blade set stored as a C array instead of `std::array` might print more compactly. Not measured.
