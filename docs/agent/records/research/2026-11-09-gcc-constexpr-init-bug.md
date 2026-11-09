# GCC 16.2.1 wrong code: a constant evaluation narrows a value-initialized array's shared initializer

- Written by: Claude (agent, Opus 5.5, cpp-expert role, researcher), 2026-11-09 UTC, for decision 0223 item 6, on `records/gcc-wrong-code-scan` from `develop` at `000ee6e`.
- Trees scanned: `develop` at `000ee6e` and the unit `feature/core-basis-change` at `0fa98ac` (the branch tip when the scan started; read through a detached checkout, not modified).
- Compilers: GCC 16.2.1 20260810 (`/usr/bin/g++`, the only GCC installed; `x86_64-pc-linux-gnu-g++` is the same build), Clang 23.1.4, libstdc++ 16, on the development machine.
- Files: everything this record cites is in `2026-11-09-gcc-constexpr-init-bug/` beside it; every compile and test run is a row of its campaign log `runs.tsv`.
- Result in one paragraph: the defect is a GCC front-end defect, independent of optimization level and of every option tried. It is not specific to `std::array`, to default member initializers, or to explicit constant expressions: any run-time execution of a value-initialization `{}` of an array (at any depth) of a class whose value-initialized state is not all zero bits is wrong once a constant evaluation of the same declaration in the same translation unit writes first at index 2 or later of the array's shared initializer, and GCC starts such evaluations by itself for ordinary run-time code. **The library is exposed**: the run-time kernels `run_map` and `run_product` (`include/galoot/core/mv.hpp:335`, `:347`) and `run_sum` (`include/galoot/core/linear.hpp:162`) have the pattern, an instrumented build shows the damage at run time in four test programs built from three sources, and a short program with a public conversion between layouts over the test rational returns a wrong motor (section 5). The tests do not see it today, because in every damaged result the tests produce, each damaged place is assigned again before it is read; a result place that no term writes keeps the damaged value, and no test reaches that case. A canary test and a workaround are proposed (section 6), not applied.

## 1. Departures, skipped checks, and ambiguities (C20)

1. **Older versions of GCC not tried.** Only GCC 16.2.1 is installed, and the brief forbids installing toolchains. Sending the reproducer to an online compiler service would be contact with a third party, which the brief also forbids. Which GCC releases have the defect is unknown.
2. **The defect was known in part.** Lesson 0065, the spike `docs/agent/records/spikes/gcc-shared-array-initializer/`, the knowledge-base note `cpp-language/lang-gcc-shared-array-initializer.md`, and `upstream-awareness.md` record the same defect for an array that is a class member with a default member initializer (`std::array<q, 6> c{};`), with a workaround in `coefficient_storage` (`mv.hpp:369`) and a canary in `test/core/mv_test.cpp:254-285`. Those records state that "a local `std::array<q, 6> a{}` was not affected". This record finds that a local array is affected when the evaluated and the run-time initialization are the same declaration (section 3.3); the earlier probes used two different functions, which is why they saw no damage. I did not edit the lesson or the knowledge base (items 4 and 5 of section 7).
3. **The scan is dynamic, with a static sweep beside it.** Rather than judging by reading which of some 200 value-initializations a constant evaluation reaches, I instrumented every one-line `T name{};` and `T name = {};` local in `include/` and `test/` of both trees (`instrument.py`), built the full suite under `gcc-debug` and `gcc-release`, and logged each value that differed at run time from a fresh value-initialization (section 4.1). What the instrumentation cannot see, and how I covered it, is listed in section 4.4. `gcc-asan` (-O1) was not scanned: -O1 folds calls as -O2 does (`results-8-folding.txt`), so I judged `gcc-release` to cover it; that is inferred, not run.
4. **The instrumented builds left out `-Werror`** and force-included `init_probe.hpp`. Seventeen tests fail in each instrumented build, the same seventeen in all four; every one is an effect of the instrumentation, not of the library (section 4.2). The uninstrumented suite was not run; nothing in `include/` or `test/` changed on this branch.
5. **Untimed compiles (P13).** Two early `-fdump-tree-original` compiles of `probe.cpp`, in scratch space, ran outside `tools/timed`. Every other compile, build, and test run is in `runs.tsv`. The probe case files were run once while exploring and then a second time to capture their output in the `results-*.txt` files, which the first runs did not save; the log holds both sets (P10 departure, for the record's reproducibility).
6. **Two scratch worktrees** (`.claude/worktrees/gcc-scan-inst-develop` and `-basis`, detached at `000ee6e` and `0fa98ac`) held the instrumented copies and their build trees. I removed them after the scan; `instrument.py` with `instrument-exclusions.txt` recreates them.
7. **Ambiguity: "default member initializer" in the brief.** The brief names the pattern by default member initializers. The defect needs only an element type whose value-initialized state is not all zero bits; a user-provided default constructor with no member initializers shows it too (section 3.2). The scan follows the wider reading.

## 2. What was asked and what was found

| Question (decision 0223 item 6, the brief) | Answer |
| --- | --- |
| Minimal reproducer | `minimal.cpp`, 23 lines, no Galoot code: exit 1 on GCC at -O0 and -O2, exit 0 on Clang (`results-minimal.txt`) |
| Which flags | Every optimization level (-O0, -O1, -Og, -O2, -O3, -Os, -Ofast), `-std=c++17` to `c++26`, `gnu++23` |
| GCC versions | 16.2.1 only (section 1, item 1) |
| Options that avoid it | None of 18 tried (section 3.2) |
| Depends on default member initializers? | No: on the element's value-initialized state not being all zero bits |
| Depends on `std::array`? | No: a C array, one- or two-dimensional, a struct member or a local |
| Depends on the form of value-initialization? | Yes: `T t{}`, `T t = {}`, `T t = T{}` are wrong; `T t = T()` and `T t;` are right; a helper returning `T{}` is wrong |
| Exposures in the library | Three run-time kernels (section 4.3); damage observed in tests, overwritten; a wrong public result demonstrated (section 5) |
| Exposures in the tests | None observed at run time; latent sites listed (section 4.3) |
| Canary or workaround | Proposed, not applied (section 6) |

## 3. The defect

### 3.1 Smallest form

```cpp
#include <array>

struct q {
  long n = 0;
  long d = 1;
};

constexpr std::array<q, 3> make(int i) {
  std::array<q, 3> a{};
  a[static_cast<unsigned>(i)].n = 1;
  return a;
}

[[maybe_unused]] constexpr auto once = make(2);

int main() {
  volatile int i = 0;
  return make(i)[2].d == 1 ? 0 : 1;
}
```

The language fixes the result: value-initialization runs each element's default member initializers, so `make(0)[2].d` is 1 and the program exits 0 ([dcl.init.general], value-initialization of an array initializes each element; [class.base.init] for the member initializers) [background]. GCC 16.2.1 exits 1 at -O0 and at -O2; Clang 23.1.4 exits 0. It is a wrong-code defect of GCC, not undefined or unspecified behavior: the program is valid and reads nothing indeterminate.

### 3.2 What it depends on

All from `probe.cpp`, one macro per dimension, run by `run.sh` on both compilers; the `results-*.txt` files hold every line. Unless stated, the constant evaluation writes element [2][2] of a 3 × 3 `std::array` of `fraction { long n = 0; long d = 1; }` and the run-time call writes nothing, so the printed mask is the damage alone (bit k is element k / 3, k % 3).

| Dimension | Varied | GCC 16.2.1 | Source |
| --- | --- | --- | --- |
| Element type | default member initializers on both members; an aggregate with them; a user-provided default constructor and no member initializers; an aggregate with `d = 1` declared before `n = 0`; a member initializer on `d` only, with `n` set by the constructor | wrong in all five | `results-3-shape.txt` (elem0 to elem4) |
| Container | nested `std::array`; flat `std::array<fraction, 9>`; struct with a C array `fraction a[3][3]`; struct with a nested `std::array` | wrong in all four | `results-3-shape.txt` (cont0 to cont3) |
| Form | `T t{}`, `T t = {}`, `T t = T{}` | wrong | `results-3-shape.txt` (init0, 1, 3) |
| | `T t = T()`; `T t;` (default-initialization); `T t{}` then every element assigned | right | (init2, 4, 5) |
| | `T t = zero()` with `zero()` returning `T{}` | wrong | `results-5-contexts.txt` (init6) |
| | one initializer per element from index packs | right | (init7) |
| What evaluates it at compile time | `constexpr` variable; `static_assert`; `constinit` variable; template argument; before or after `main` in the file | wrong in all | `results-3-shape.txt` (ce*, order1-ce*) |
| | `const T g = make();` at namespace scope (not `constexpr`) | wrong | `results-5-contexts.txt` (ce5) |
| | `static const T s = make();` in a function | wrong | (ce6) |
| | `const T x = make();` or `T x = make();`, automatic, in ordinary run-time code | wrong | (ce7), `results-6-runtime-calls.txt` (ce8) |
| | `return make()[0][0].d;` in run-time code | right at -O0; wrong at -O1, -Og, -O2, -O3, -Os | `results-6-runtime-calls.txt`, `results-8-folding.txt` (ce9) |
| | no evaluation at compile time | right | (ce0) |
| Template | `make` a function template | wrong, as the non-template | `results-5-contexts.txt` |
| Store | assignment of a whole element, or of one member | wrong in both | `results-3-shape.txt` |
| Size | 2 × 2, any stores | right (no index reaches 2) | `results-3-shape.txt` (n2-*) |
| | 4 × 4 and 5 × 5 | wrong | (n4-*, n5-*) |
| Options at -O2 | `-fno-elide-constructors`, `-fimplicit-constexpr`, `-fno-inline`, `-fno-tree-ccp`, `-fno-strict-aliasing`, `-D_GLIBCXX_ASSERTIONS`, `-fsanitize=undefined`, `-fsanitize=address`, `-flto`, `-fconstexpr-ops-limit=100000000`, `-fconstexpr-cache-depth=0`, `-fno-threadsafe-statics`, `-fno-exceptions`, `-fpermissive`, `-fno-optimize-sibling-calls`, `-ftrivial-auto-var-init=pattern` and `=zero`, `-fno-gnu-unique` | wrong with every one | `results-4-flags.txt` |
| Language mode | `-std=c++17`, `c++20`, `c++23`, `c++26`, `gnu++23` | wrong in all | `results-4-flags.txt` |

Clang 23.1.4 is right in every case it compiles (it rejects five GCC-only options).

The rows on implicit evaluation matter most for a library. GCC tries to constant-evaluate the initializer of a variable whose initializer is a call with constant arguments, even an automatic non-`const` variable in a run-time function, and at -O1 and above it also folds such calls inside expressions. So a translation unit with no `constexpr` variable and no `static_assert` can still damage a run-time call with non-constant arguments, as long as some other call of the same function in that unit has constant arguments. ASan and UBSan stay silent (the values are wrong, not invalid).

### 3.3 Which writes damage what

From `results-2-separate.txt` and `results-7-flat.txt` (the run-time call writes nothing):

- One-dimensional, 9 elements: the first write of the constant evaluation at index i ≥ 2 zero-fills elements i to 8 at run time (store at 2: elements 2 to 8; at 3: 3 to 8; at 8: 8 only). A first write at index 0 or 1 does no damage, and then later writes at any index do none either (stores {1, 3}, {0, 4}, {1, 4}: no damage; {2, 4}: from 2; {3, 4}: from 3).
- Nested 3 × 3: the rule holds at each level. A first write in a row at column 2 zero-fills column 2 of every row (all rows share one row initializer); a first write at row 2 zero-fills row 2. The diagonal stores [0][0], [1][1], [2][2] of the original reproducer damage column 2 of every row, and the run-time stores repair [2][2], which leaves the two wrong elements [0][2] and [1][2] that the Sage cross-check saw.
- The damage is a zero fill, so it is invisible for an element whose value-initialized state is all zero bits (`int`, `double`, `float`, `std::complex<double>`, a struct of integers), as the earlier records said.

### 3.4 Mechanism, as far as observed

`gimple-make-ce0.txt` and `gimple-make-ce1.txt` are the start of the run-time `make()` as GCC lowers it, without and with the constant evaluation (store at [2][2]). Without it, a loop pair sets `n = 0, d = 1` for rows 0 to 2 and columns 0 to 2 (`if (D == 2)`). With it, the object is first zero-filled (`<retval> = {};`) and the same loops stop at row 1 and column 1 (`if (D == 1)`), so row 2 and column 2 keep the zero fill. That is direct evidence that the constant evaluation changed the bounds of the initializer the front end later lowers [checked-self]. That GCC holds a value-initialized array as one shared element initializer over an index range and that the evaluation's first write splits that range in place, shortening the original to [0, i − 1] when at least two elements precede i, is my reading of the pattern; I did not read GCC's source [inferred]. It fits every case above, including why indices 0 and 1 are safe and why `T()` (a different lowering) is not affected.

## 4. Scan of the library and the tests

### 4.1 Method

`instrument.py` appends, on the same line (so line numbers stay), `::galoot_init_probe::check(name, __FILE__, __LINE__);` to every local declaration `T name{};` or `T name = {};` that is not `constexpr`, in `include/` and `test/`: 160 sites at `000ee6e` and 206 at `0fa98ac`. With `--kernels`, the three run-time kernels get `check_kernel` instead, which also reports which damaged places no term of the kernel writes. Eight declarations at class scope, where a statement cannot go, are excluded (`instrument-exclusions.txt`) and judged by reading (section 4.3). `init_probe.hpp`, force-included into every test translation unit, does nothing in a constant evaluation; at run time it compares the value with a fresh `const T fresh{}` made in the probe (a declaration no constant evaluation writes into) and appends a line to a log for each mismatch. `scan.sh` builds and runs the full suite under one preset.

Positive control (`results-control.txt`; the control is `probe.cpp` with the check appended to `matrix t{};`, built with `-include init_probe.hpp -DRT_STORES=0 -DSTORES=0x100`): GCC with the constant evaluation logs the damage and still prints the wrong mask 0x1e4 that the uninstrumented probe prints (`results-3-shape.txt`, elem0), so the check neither misses nor hides the defect; GCC without the constant evaluation, and Clang either way, log nothing.

### 4.2 Runs

| Tree | Preset | Build | Tests | Log lines |
| --- | --- | --- | --- | --- |
| `develop` `000ee6e` | gcc-debug (-O0) | 37.7 s, exit 0 | 482 of 499 pass | 54 |
| `develop` `000ee6e` | gcc-release (-O2) | 52.4 s, exit 0 | 482 of 499 pass | 54 |
| unit `0fa98ac` | gcc-debug | 41.2 s, exit 0 | 520 of 537 pass | 54 |
| unit `0fa98ac` | gcc-release | 52.9 s, exit 0 | 520 of 537 pass | 54 |

The four logs, sorted by event, program, site, and damaged places, are identical; `scan/probe.log` is the `develop` gcc-debug one, and `scan/ctest-summary-*.txt` list the failures. The seventeen failing tests in each build are all caused by the instrumentation:

- `core.bare_compile`, `rga3.bare_compile`: compile a header without the forced include, so `::galoot_init_probe` is undeclared.
- `core.plan_cost_model.*` (8): their recorded constant-evaluation thresholds no longer hold with the extra calls.
- `core.mv_narrowing_int`, `core.mv_narrowing_float`: expect `-Werror=narrowing`, which the scan dropped.
- `core.mv_scalar_move_only`: the script appended a statement to a declaration at namespace scope in that compile-fail test.
- `core.mv_truncate_unchecked_counted`, `core.mv_part_never_checks_counted`, `core.transform_checking_recorded`, `core.transform_unchecked_recorded`: count the comparisons of a counting scalar, and the probe compares.

The cost models and the counting tests thus ran under GCC without the probe only in earlier, uninstrumented runs; nothing on this branch changes them.

### 4.3 Exposures, by site

"Damage" means the instrumented build saw a run-time value-initialization differ from a fresh one. Line numbers are the same on both trees for files the unit does not change.

**Library: exposed, damage observed, results not wrong in the tests**

| Site | What it is | Seen in (both trees, both presets) | Damaged places, all written by a term | Would a wrong result be caught? |
| --- | --- | --- | --- | --- |
| `include/galoot/core/mv.hpp:335`, `run_map` | `std::array<T, R> out{};` in the run-time kernel of conversions between layouts and of maps | `core_mv_test` (3 events: places 4 to 7 of an 8-place result), `rga3_objects_test` (23 events: places 2 and 3 of a 4-place result, places 4 and 5 of a 6-place one) | yes, every event | Not today: no damaged place reaches a result. Over the test rational, a damaged place left in a result reads 0/0, and the `==` of a multivector over the rational compares members, so a test that compares the whole result with an independent value would fail. No test exercises a conversion or map in which a place after the first write is left unwritten and the same instantiation is evaluated at compile time; section 5 builds one. |
| `include/galoot/core/mv.hpp:347`, `run_product` | the same in the run-time kernel of products | `core_transform_mode_0` and `_1` (`test/core/transform_assert_fail.cpp`; 28 events, the sandwich's first and second kernels over rigid(3), results of 4, 6, and 8 places, damaged from place 2, 3, 4, 5, or 7 on) | yes, every event | Same reasoning. Whether the public interface can give a product kernel a result place that no term writes (for instance a product into a declared target layout) I did not establish; if it cannot, `run_product` can be damaged but never wrong. |
| `include/galoot/core/linear.hpp:162`, `run_sum` | the same in `x + y`, `x - y` | no damage in the suite | — | Its target holds every blade of x and y and each is assigned (its comment), so I infer every place is written and a wrong result needs a target larger than the union; not checked. |

Why damage without a wrong result: each kernel assigns the first term on every place it reaches (`accumulate<C, First>`), so a zero-filled place that a term reaches is overwritten before it is read. Only places no term reaches keep the value from `out{}`, which is the value the comment in `run_map` promises ("the positions no term reaches are zero").

**Library: pattern present, not exposed**

- Every other value-initialized array in `include/` holds a type whose value-initialized state is all zero bits (`kernel_term`, `plan_term`, `typed_term`, `term`, `oriented_blade`, `bool`, `char`, `std::size_t`, `int`, `coefficient_type` = `std::int64_t`), so the zero fill equals the correct value; or it is in a `consteval` function, never run at run time. This covers `basis_change.hpp:349`, `:458`, `:746` and the other basis-change arrays, which are of `coefficient_type`.
- `transform.hpp:510`, `:526`, `:586` hold a scalar T that satisfies `rounding_checked_scalar` (an IEC 559 type rounding to nearest), whose value-initialized +0 is all zero bits [inferred for every such type; checked for `float` and `double` by the scan, which logged nothing there].
- `mv.hpp:369` (`coefficients{Ts{}...}`, one initializer per place) is the earlier workaround, with its canary; functions that `return R{}` or `return {}` for a multivector go through it. The scan logged nothing for it.
- `blade_set.hpp:388` (a class member `std::array<char, L> chars{}`): `char`.

**Tests: latent, no damage at run time in any build**

These value-initialize arrays of a type whose zero is not all zero bits (the test rational, `tracked`, or a template parameter the tests instantiate with them). None was damaged in the four builds, so no current test result is affected; each becomes an exposure if a later change evaluates its function at compile time with a first write at index 2 or later and also runs it in the same translation unit, which includes calling it with constant arguments from run-time code (section 3.2).

- Unit branch: `test/support/diagonalize.hpp:93`, `:94`, `:185`, `:200`, `:216` (`rational_matrix<N>`). This is the code the Sage cross-check tripped over. In the unit's tests it is evaluated at compile time only for two generators, where no index reaches 2; the scan confirms no damage. A wrong result there would usually be loud (the Sage dump aborted with "rational: the denominator is zero"), but not necessarily: a damaged entry that is overwritten before use, or one compared only with another output of the same miscompiled path, would pass.
- Unit branch: `test/core/basis_change_identities_test.cpp:67`, `test/core/basis_change_validation_test.cpp:157` to `:160`.
- Both trees: `test/core/mv_test.cpp:114`, `:1056`, `:1068` (the tests' own reference kernels, `std::array<T, ...> out{}`, the same shape as `run_map`); `test/core/transform_rounding_test.cpp:417`, `:418`, `:507`, `:581`, `:610`, `:611`, `:988`, `:989`; `test/core/transform_identities_test.cpp:207`, `:208`; `test/presets/rga3/chains_test.cpp:43`; `test/presets/rga3/motions_test.cpp:73`, `:88`, `:301`.
- Class scope, judged by reading: `test/presets/rga3/point_groups_test.cpp:41` (`element::coefficients{}`, `std::array<rational, 8>`) is the original member form, but no function that makes or writes an `element` is `constexpr`, so nothing evaluates it at compile time; not exposed. The other class-scope sites (`bits_test.cpp:37`, `:291`, `blade_product_test.cpp:84`, `blade_set_test.cpp:60`, `:104`, `:422`) hold integers or `char`.

### 4.4 What the scan does not see

- Declarations split over lines, designated initializers with a `{}` member, temporaries, and `T t = f()` where `f` returns `T{}` (which is damaged, section 3.2). A grep of both trees for these forms found multivector returns (`return R{}`, `return {}`), which go through the per-place initializer; `return {}` after a failing `static_assert` in `change_basis`; returns of small test structs and of test scalar stand-ins that hold no array; the basis-change designated initializers (`.inverse = {}` and the like), whose arrays are of `coefficient_type`; and one compile-fail test. None value-initializes an array of a type whose zero is not all zero bits [checked by reading]. The record does not claim the multi-line grep is exhaustive.
- Class-member arrays at run time: none in `include/` holds such a type apart from `mv.hpp:369`, which has the workaround and a canary.
- Run-time code the suite does not execute, and the README examples beyond what the suite builds. A user's translation unit is not covered by any scan; section 5 shows what it can meet.

## 5. A wrong result through the public interface

`library_exposure.cpp` (no change to the library) converts a value on the layout `e42 e43` of rigid(3) to the motor layout `1 e41 e42 e43 e23 e31 e12 e1234`, over the test rational. The conversion's map kernel writes places 2 and 3 only. A `constexpr` motor of the same conversion makes the first write of the constant evaluation land on place 2; the run-time conversion in `main` then returns a motor whose places 4 to 7 hold 0/0 (`results-library.txt`):

| Build | Denominators of the run-time motor | Equal to the expected motor |
| --- | --- | --- |
| g++ -O0 and -O2, with the constant | 1 1 1 1 0 0 0 0 | no |
| g++ -O0 and -O2, without it | 1 1 1 1 1 1 1 1 | yes |
| clang++ -O0 and -O2, either way | 1 1 1 1 1 1 1 1 | yes |

So any user of the library on GCC 16 with an exact scalar type of their own (a rational or a fixed-point class with a nonzero member) can get a wrong multivector from a conversion to a wider layout, from a map, or possibly from an `apply` into a larger target, whenever the same instantiation is also evaluated at compile time in that translation unit, explicitly or by GCC's own folding of a call with constant arguments. The kernels are unchanged on the unit branch (`git diff 000ee6e 0fa98ac` touches only `basis_change.hpp` and `core.hpp`), so the unit's change of basis, which calls `run_map` (`basis_change.hpp:985`), has the same exposure [inferred from the shared code; not demonstrated with a change of basis].

## 6. Proposals (not applied)

### 6.1 Code workaround in the three kernels

Replace `std::array<T, R> out{};` in `run_map`, `run_product`, and `run_sum`. Two forms were tried in scratch copies of `include/` and both make `library_exposure.cpp` right on GCC at -O0 and -O2 (`results-library.txt`):

- **A. One initializer per place**, as `coefficient_storage` already does:

  ```cpp
  template <class T, std::size_t... P>
  constexpr std::array<T, sizeof...(P)>
  zero_places(std::index_sequence<P...> /*places*/) {
    return {((void)P, T{})...};
  }
  // in each kernel:
  std::array<T, R> out = zero_places<T>(std::make_index_sequence<R>{});
  ```

  It relies on the form that lesson 0065 already chose and that has a canary, and on nothing new about GCC. It costs one pack expansion of R elements per kernel instantiation, up to 1,024 places for `full<10>`; each multivector type already pays one of the same size in `coefficient_storage`, but the kernels' extra build cost is unmeasured.

- **B. Value-initialization with parentheses**: `std::array<T, R> out = std::array<T, R>();`. One token, no build cost, and right on GCC in every case tried here, including the class-member form of the earlier spike (run row `gcc-init:g++:member-V5`). It rests on GCC lowering `T()` differently from `T{}`, which is an observation, not a documented guarantee, and a reader or a lint rule may "modernize" it back to `{}`.

Each needs a comment in the style of `mv.hpp:363` saying why the shorter form is not used. Option A is my recommendation (section 7, item 1). The same change is worth making in `test/support/diagonalize.hpp` (unit branch, five sites) and in the tests' reference kernels `test/core/mv_test.cpp:1056` and `:1068`, which are latent today.

### 6.2 Canary test

A test case in `core.mv` (`test/core/mv_test.cpp`), in the terms of the library's contract: "a conversion to a wider layout gives zero on every blade the source lacks, also when the same conversion is evaluated at compile time in the program". It is `library_exposure.cpp` as a doctest case: a namespace-scope `constexpr` conversion from `e42 e43` to the motor layout over the rational, and a run-time conversion of a value read through `volatile`, checked equal to the expected motor and checked place by place for denominator 1. It fails on GCC 16.2.1 against `develop` today and passes with either workaround, so it lands in the same unit as the workaround, or before it as a labelled failing commit (W2). A mutant that restores `out{}` in `run_map` should be killed by it on gcc-debug and be equivalent on clang-debug, as `shared_zero_initializer` is for the storage. A second case through `run_product` needs a product kernel with an unwritten result place; whether the interface allows one is the open question in section 4.3.

## 7. Open items for the owner

1. **Apply a workaround to the library's kernels, and which form.**
   - Options: A, one initializer per place (section 6.1); B, `T()` in place of `T{}`; C, leave the library as it is and document that GCC 16 can give wrong results for scalars whose zero is not all zero bits.
   - Tradeoffs: A matches the existing workaround and its rationale, at an unmeasured build cost for large layouts. B is free but rests on an undocumented difference in GCC and is easy to undo by accident. C leaves a wrong-result path on a supported compiler, against the library's promise that positions no term reaches are zero.
   - Recommendation: A, in a small core unit with the canary of 6.2, the mutant, a build-cost measurement for the largest layouts before and after (W10), and a blind review. If A's build cost is large, B with a comment and the canary, which would catch a regression either way.
2. **Who fixes `test/support/diagonalize.hpp`.** It is the unit's file, on an active branch, so this record only reports it (W7).
   - Options: the unit applies the same form now; or it waits for item 1's unit and follows its form.
   - Tradeoff: now is cheap and removes the trap the Sage cross-check hit; waiting keeps one form decided in one place.
   - Recommendation: the unit applies it now, as a test-support change with its own commit, since a later unit test that evaluates `diagonalize<3>` at compile time would otherwise be silently wrong or abort.
3. **A test rule.** Lesson 0065's rule ("cover one scalar whose zero is not all zero bits, in constant evaluation and at run time in one translation unit") did not catch this, because the exposure needs a specific kernel shape. Recommendation: extend the rule to "for each run-time kernel that value-initializes its result, a test over such a scalar in which the same kernel instantiation is evaluated at compile time with its first write past place 1 and at run time, with a result place no term writes". Tradeoff: one more test per kernel; it is the only way the suite sees this defect.
4. **Correct lesson 0065 and the knowledge base.** Both say a local array is unaffected and name the class-member form as the trigger. Recommendation: an addendum to lesson 0065 pointing here, and a revision of `knowledge-base/cpp-language/lang-gcc-shared-array-initializer.md` and its `upstream-awareness.md` row stating: the trigger is any value-initialization `{}` of an array of a type whose zero is not all zero bits, local or member, at any depth; a constant evaluation of the same declaration in the same translation unit whose first write into the shared initializer is at index 2 or later; implicit evaluations count (namespace-scope `const`, automatic variables initialized by calls with constant arguments, and at -O1 and above calls with constant arguments in expressions); `T()` and default-initialization are not affected, `T t = f()` with `f` returning `T{}` is; and the GIMPLE evidence of section 3.4. I did not edit either: the knowledge base asks for a branch and commits of its own, and my brief does not cover commits there. Tradeoff: none beyond the edit.
5. **Retest on each new GCC.** `minimal.cpp` has no dependencies. Recommendation: whoever updates the toolchain record runs it (exit 0 means fixed) before any workaround is removed. No report was sent to GCC's maintainers, and none should be by an agent.

## 8. Files

| File | What it is |
| --- | --- |
| `minimal.cpp`, `results-minimal.txt` | the smallest reproducer and its runs |
| `probe.cpp`, `run.sh`, `cases-*.tsv`, `results-*.txt` | the parameterized probe, its driver, the case files, and their outputs on both compilers |
| `gimple-make-ce0.txt`, `gimple-make-ce1.txt` | the lowered start of the run-time `make()` without and with the constant evaluation |
| `init_probe.hpp`, `instrument.py`, `instrument-exclusions.txt`, `scan.sh`, `results-control.txt` | the instrumentation, the scan driver, and the instrumentation's control |
| `scan/probe.log`, `scan/ctest-summary-*.txt` | what the scan logged and which tests failed in each build |
| `library_exposure.cpp`, `results-library.txt` | the wrong result through a public conversion, and the two workarounds tried |
| `runs.tsv` | the campaign log of every timed run (523 probe compiles, 5 configures, 6 builds, and 5 test runs, about 10.1 minutes of wall time in all; the first gcc-debug build of `develop` failed on the class-scope sites, and its first test run preceded the kernel check, so both were superseded) |

Agent cost: not visible to me; for the coordinator to record.
