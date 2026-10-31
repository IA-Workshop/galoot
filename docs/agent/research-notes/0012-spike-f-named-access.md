# 0012 Spike F: named component access with a fixed-string template argument

Date: 2026-10-31. Checked by: a spike subagent (Claude Opus 5.5) working for the owner. Scope: plan 0001, section 9, Spike F. It tests `get<"e41">(x)`, with a fixed-string class-type template argument, on GCC 16.2.1 and Clang 23.1.4 (decision 0006): whether it works in C++23 mode, what a missing name reports, what it costs to compile, and an alternative. It uses the throwaway fixture of Spike C (research note 0010). Nothing here is library code.

Revision and provenance. The final runs were made on the files as kept, at revisions `aa8051c0be1d` to `63e2014f8bdc` (`develop`), marked `-dirty` only by `tools/timed`'s own append to `runs.tsv`. Earlier runs at `96eb5fc7` measured a first version of `get` whose index was found at run time at -O0 (below); they are superseded. The owner's commits during the spike (`783ff86` to `63e2014`) change governance, decisions, plans, and records only, none of which this spike reads. Sources and results are in `docs/agent/records/spikes/spike-cdf/`, not yet committed, so the figures are labeled not reproducible until they are (V10).

How each claim was checked:

- **Local**: compiled, and where stated measured, on this machine by the scripts named, with `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off`.
- **Inferred**: my reasoning, not checked.

## Summary

1. `get<"e41">(x)` works in C++23 mode on both compilers, under the S4 flags with `-Werror` and under bare `-std=c++23 -pedantic-errors` (`named_check.cpp` and the string unit, recorded in `out/named/diag/summary.tsv`). The name is a template parameter of a structural class type, `fixed_string<N>`, deduced from the string literal through a `consteval` constructor (class template argument deduction). `named_check.cpp` shows by `static_assert` that each name reads and writes its own slot, and that the parser gives the masks and signs of CONVENTIONS 1.6 (e41 = −e14, e423 = +e234, e431 = −e134, e321 = −e123). The rejection cases in it hold as well (Local).
2. A missing name, raw (form R of note 0010), takes 34 lines on GCC, whose first error is "no matching function for call to ‘get<"e1">(const cdf::rga3_types::Line&)’" and so shows the name as written. Clang takes 22 lines: "no matching function for call to 'get'", then "because 'has_blade<cdf::rga3, blade_set<6>{{{9, 10, 12, 6, 5, 3}}, {{-1, -1, -1, 1, -1, 1}}}, fixed_string<3>{{"e1"}}>' evaluated to false". With note 0010's catch-all (form P) it takes 9 and 8 lines, led by "static assertion failed: get<name>: no blade of this name, with this orientation, is in the blade set". A name that is invalid in the algebra fails in the parser through the decision 0012 helper (note 0010, case 22). The opposite orientation, `get<"e14">` on a Line that stores e41, is treated as missing (Local).
3. Cost per use is small but not free. Per added use, from 64 to 256 repetitions (3,456 more uses of 18 distinct accesses), the string form costs 1.20e6 retired instructions on GCC -O0 (1.05e6 at -O2) and 0.60e6 on Clang -O0 (1.09e6 at -O2). An integer index (`get_at<0>`) costs 0.81e6 / 0.68e6 and 0.24e6 / 0.71e6. So the string form costs 1.5× (GCC, Clang -O2) to 2.5× (Clang -O0) the index form. In absolute terms, 4,608 uses compile in 0.47 s on GCC -O0, against 0.35 s for the index form and 0.15 to 0.19 s for the baseline. The generated code is the same: `.text` sizes of the string and index forms are equal at every point. Peak memory grows by 17.8 KiB per use on GCC and 5.9 KiB on Clang, against 11.6 and 2.9 for the index (Local).
4. At -O0 the first version of `get` searched the blade set at run time on both compilers: 18 calls of `index_of` for 18 accesses. In `x.c[index_of(B, parse_name(Name, n))]` only `parse_name` (`consteval`) is forced constant. Binding the index to a `constexpr` local removes the calls (0 on both). This bears on P4 for debug builds (Local; `run_named.py diag`).
5. Symbols: each (type, name) pair is one instantiation of `get`, with mangled names of 213 to 267 characters on GCC and 252 to 306 on Clang. The index form has 158 to 204 and 165 to 204. GCC shows 36 index-form symbols for 18 instantiations, two names each, with the second 7 characters longer, as note 0008 saw for value arguments (the pairing was read with `nm` in a scratch compile that is not kept). Clang's mangling of the string form also carries the requires-clause (`…Q9has_bladeIT0_XT1_EXT_EE…`) (Local).
6. The alternative `x["e41"]` also works on both compilers. `operator[]` takes `name_in<Alg, B>`, whose `consteval` constructor from a string literal finds the blade or calls the failure helper. Its missing-name diagnostic is short, 12 lines on GCC and 13 on Clang, and begins "call to consteval function … is not a constant expression". GCC shows the name in that line (`name_in<…>("e1")`), and Clang in a note ("in call to 'name_in<3UL>("e1")'"). Both name the helper `blade_is_not_in_this_blade_set`. But `requires { x["e1"]; }` reports it well-formed on both compilers. It costs more per use than the string form (GCC -O0 1.65e6, Clang -O0 0.72e6), and at -O0 it passes the index as a run-time argument: `.text` is 253,547 bytes against 175,467 at 256 repetitions on GCC (Local). Inferred cause of the detection failure: a requires-expression does not evaluate the immediate invocation in its unevaluated operand.

Recommendation for WU8 (Inferred from the measurements):

- Use `get<"e41">(x)` with a structural `fixed_string` template parameter, constrained by a named concept (`has_blade`) so that generic code can detect it and the raw diagnostic shows the name. Pair it with WU15's catch-all (note 0010, form P) for the message.
- Bind the blade index to a `constexpr` variable inside `get` (finding 4), and check it with an assembly read at -O0, as `run_named.py diag` does.
- The subscript form is a working fallback, but it is not needed: it hides misuse from `requires` and costs more.
- WU8 still has to settle three things that this spike did not: whether the opposite orientation is rejected (as here) or returns the negated value (by value, so not writable); the name syntax for indices of 10 and above (n ≤ 16, decision 0004; this parser takes one digit per index); and a name for the scalar (here "1").

## The forms

`gen_named.py SPELLING REPS` writes a unit with REPS functions. Each function reads all 18 (type, name) pairs of the rga3 Point (e1 to e4), Line (e41, e42, e43, e23, e31, e12), and Motor (1, e41, …, e12, e1234) into a `volatile` sink, so the unit has 18 distinct accesses and 18 × REPS uses.

| Spelling | Access | Instantiates |
| --- | --- | --- |
| string | `get<"e41">(l)` | One `get` per (type, name) |
| index | `get_at<0>(l)`, an unconstrained `template <std::size_t I, …>` | One `get_at` per (type, index); the baseline, which has no name and no check: `get_at<6>` on a six-blade Line compiles and reads out of bounds |
| subscript | `l["e41"]` | One `operator[]` per type; the name is checked in a `consteval` constructor |

REPS = 0 is the baseline (fixture and `main`).

## Measurements

Same machine and method as note 0011: i9-14900K, pinned with `taskset -c 6` (sibling CPU 7 at most 0.04 busy, load average 1.0 to 1.1), compiles one at a time, peak memory from `measure.py`, and counters from `perf stat` through `pinned.py`. One sample per point. Wall times below 0.25 s are dominated by start-up and are not compared. From `out/named/named.tsv`, printed by `summarize.py f`.

Instructions (1e9), wall (s), and peak (MiB) at 1 and 256 repetitions (18 and 4,608 uses), with the baseline:

| Compiler, level | Baseline | string, 1 | string, 256 | index, 1 | index, 256 | subscript, 1 | subscript, 256 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| GCC -O0 | 0.731, 0.19, 68.5 | 0.797, 0.15, 71.7 | 6.290, 0.47, 148.7 | 0.780, 0.15, 71.8 | 4.483, 0.35, 120.0 | 0.776, 0.15, 71.9 | 8.327, 0.56, 160.1 |
| GCC -O2 | 0.730, 0.15, 68.9 | 0.796, 0.17, 73.0 | 5.609, 0.46, 151.8 | 0.781, 0.15, 72.9 | 3.890, 0.37, 124.1 | 0.785, 0.15, 73.0 | 7.163, 0.56, 170.9 |
| Clang -O0 | 0.655, 0.15, 111.4 | 0.696, 0.17, 112.9 | 3.466, 0.32, 139.3 | 0.670, 0.17, 112.7 | 1.758, 0.23, 125.8 | 0.675, 0.17, 112.7 | 3.985, 0.35, 130.1 |
| Clang -O2 | 0.661, 0.16, 113.1 | 0.711, 0.17, 116.4 | 5.666, 0.49, 149.8 | 0.686, 0.16, 116.2 | 3.907, 0.37, 137.3 | 0.690, 0.15, 116.1 | 6.171, 0.50, 141.3 |

Per added use (64 to 256 repetitions) and for the first 18 distinct accesses with their 18 uses (1 repetition minus the baseline):

| Compiler, level | string, per use | index, per use | subscript, per use | string, first 18 | index, first 18 | subscript, first 18 |
| --- | --- | --- | --- | --- | --- | --- |
| GCC -O0 | 1.196e6, 17.8 KiB | 0.807e6, 11.6 KiB | 1.646e6, 19.9 KiB | 66.3e6 | 49.3e6 | 45.8e6 |
| GCC -O2 | 1.050e6, 18.2 KiB | 0.681e6, 11.8 KiB | 1.392e6, 22.4 KiB | 65.6e6 | 50.7e6 | 54.9e6 |
| Clang -O0 | 0.603e6, 5.9 KiB | 0.237e6, 2.9 KiB | 0.722e6, 3.9 KiB | 41.5e6 | 15.2e6 | 20.0e6 |
| Clang -O2 | 1.089e6, 7.7 KiB | 0.711e6, 5.0 KiB | 1.203e6, 5.9 KiB | 50.6e6 | 25.1e6 | 29.2e6 |

- **Claim.** The string and index forms produce identical `.text` sizes at every point on both compilers (for example 175,467 bytes on GCC -O0 and 184,067 on Clang -O2 at 256 repetitions). Checked: Local (`size -A`). At -O2 GCC folds the identical functions to 4,437 bytes in every form. Clang does not fold them. Inferred: GCC's identical-code folding (`-fipa-icf`, enabled at -O2).
- **Claim.** Subscript `operator[]` is instantiated once per type: three symbols of 209, 253, and 287 characters on GCC. The table's count of 6 for subscript also includes three `std::array::operator[]` symbols (23 characters) that the symbol filter caught. Checked: Local, `nm` in a scratch compile that is not kept, so not reproducible from the records; the count of 6 is in `named.tsv`.
- **Claim.** The fixture's `get` uses `x.c[i]` with i a constant, so its -O0 body is an indexed load (0 calls; finding 4). Checked: Local, assembly of the string unit at -O0, recorded in `out/named/diag/summary.tsv`.

## Diagnostics

`run_named.py diag`, results in `out/named/diag/`. The unit is the string or subscript unit with its last access replaced by `l` and the name "e1" (not in a Line):

| Compiler | Spelling, fixture form | Lines | Error lines | First error line |
| --- | --- | --- | --- | --- |
| GCC | string, R | 34 | 1 | no matching function for call to ‘get<"e1">(const cdf::rga3_types::Line&)’ |
| GCC | string, P | 9 | 1 | static assertion failed: get<name>: no blade of this name, with this orientation, is in the blade set |
| GCC | subscript | 12 | 2 | call to consteval function ‘cdf::name_in<cdf::rga3, cdf::blade_set<6>{…}>("e1")’ is not a constant expression; the second error is "call to non-‘constexpr’ function ‘void cdf::blade_is_not_in_this_blade_set()’" |
| Clang | string, R | 22 | 1 | no matching function for call to 'get' |
| Clang | string, P | 8 | 1 | static assertion failed: get<name>: no blade of this name, with this orientation, is in the blade set |
| Clang | subscript | 13 | 1 | call to consteval function 'cdf::name_in<cdf::rga3, blade_set<6>{{{9, 10, 12, 6, 5, 3}}, {{-1, -1, -1, 1, -1, 1}}}>::name_in<3UL>' is not a constant expression |

Detection (`named_detect.cpp`): `requires { get<"e1">(line); }` is false, and `requires { line["e1"]; }`, `get<"e41">`, and `line["e41"]` are true, on both compilers (packed answer 14). The fixture is in form R there.

## Reproduction

From the repository root, serially, on a quiet machine:

```bash
S=docs/agent/records/spikes/spike-cdf
tools/timed test - spike-f:run_named -- python3 -I $S/run_named.py            # 52 pinned compiles, 18 s
tools/timed test - spike-f:run_named-diag -- python3 -I $S/run_named.py diag  # diagnostics, checks, assembly, 3 s
python3 -I $S/summarize.py f
```

The generated units are written to `out/named/gen/` during a run and were deleted afterward, since `gen_named.py` regenerates them. No randomness is involved, so there are no seeds.

## Open questions

1. Orientation: should `get<"e14">` on a Line that stores e41 be rejected (as here), or return −c by value? A rejected name keeps CONVENTIONS 1.6 visible (C8); a negated value is friendlier but cannot be written through.
2. Name syntax for n ≥ 10: "e1,10"? "e(1)(10)"? Needs a CONVENTIONS 1.6 decision before WU4's parser.
3. GCC's raw diagnostic for a missing name is 34 lines, because it walks both `get` overloads (const and non-const). A single overload taking a forwarding reference might halve it. Not tested.
