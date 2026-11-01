# 0090 The blade set interface

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), implementing WU4 under plan 0008.

## Context

Plan 0008 fixes the content of `include/galoot/core/blade_set.hpp` in outline: `oriented_blade` (a mask and a sign), `blade_set<K>` as a structural aggregate usable as a class-type template argument (decision 0021, STYLE S7), the canonical result order of CONVENTIONS 1.7, `full<N>`, `of_grade<N, k>`, `merge`, `index_of` (a compile-time error when the mask is absent), the parser `blades<N>("e41 e42 e43 e23 e31 e12")` of CONVENTIONS 1.6, and canonical names from masks. It asks for rejections "with messages in the algebra's terms (the catch-all form for functions, the plain static assertions in consteval code, as established)". It names `bits.hpp` and `metric.hpp` as dependencies.

## Options and decisions

1. Spelling of the parser. The plan writes `blades<N>("...")`, a function whose argument is the list. By the language rules (inferred; no compile was made to show it): the size K of the result is the number of names, and a function's return type cannot depend on the value of a function argument in C++23, so `blades<N>("...")` cannot return a `blade_set<K>`. The options were (a) a function returning a fixed-capacity set with a count, which is not a `blade_set<K>` and would carry unused slots into every type built on it; (b) the list as a template argument, a structural string type deduced from the literal: `blades<N, "...">`; (c) one template argument per name, `blades<N, "e41", "e42", ...>`, as in Spike C (note 0010). Chosen: (b), a departure from the plan's spelling. It keeps the plan's single string, and the list's characters are a template argument, so each check can be a `static_assert` with a literal message, which both compilers print as the first error line. (c) would multiply template arguments per type and make the diagnostics list every name.
2. Form of every constructor. `full<N>`, `of_grade<N, K>`, `merge<A, B>`, `index_of<S, Mask>`, and `blades<N, Names>` are variable templates initialized by a `consteval` function in `detail` that holds the checks as `static_assert`s and returns a placeholder `blade_set<0>` after a failed check, so a misuse gives exactly one error (form P of note 0010, applied in consteval code). A variable template is computed once per argument list (STYLE S7, PERFORMANCE P3). `merge` and `index_of` are templates rather than functions over values for the same reason as the parser: the size of a merge depends on its inputs, and `index_of` must fail to compile for an absent mask, which a function parameter cannot do with a `static_assert`.
3. Catch-all overloads. No public function in this unit rejects a misuse that its template arguments or operand types reveal, so the catch-all of decision 0033 has no place here, as in decision 0060 item 7. `canonical_name(mask)` is an ordinary `constexpr` function usable at run time; its one precondition (no index above 9) is checked by `GALOOT_ASSERT`, as `from_gram` checks its values.
4. Dependencies. The header includes `bits.hpp` (mask type, `grade`, `reorder_sign`) and `assert.hpp`, but not `metric.hpp`. A blade set does not depend on the metric: which blades exist and their canonical order depend only on n. The generator count is `std::size_t` and its bound is `mask_bits`, the bound `metric<N>` uses (decisions 0036, 0051, 0062), so a set and a metric of one algebra take the same N. Including `metric.hpp` with no use would break STYLE S6 ("includes what it uses"). A departure from the plan's dependency list.
5. The sign is `int`, not `std::int8_t` as in the design study, so it multiplies the `int` results of `reorder_sign` and the other sign functions in `bits.hpp` without conversions under `-Wconversion`. The grade parameter of `of_grade` is `int`, the grade type of decision 0051.
6. Canonical order. `canonical_less(a, b)` compares grades and, within a grade, decides by the lowest generator in which the masks differ: the mask that holds it has the lexicographically smaller index list. `full<N>` and `of_grade<N, K>` enumerate index lists directly in lexicographic order (the next combination), O(2^n · n), rather than sorting all masks, which would cost O(4^n) comparisons with an insertion sort, about 10^9 at n = 16 (inferred, not measured). `merge` inserts into a list kept in canonical order, O(K²) for K blades.
7. `size()` is a non-static member, as for `std::array`, because callers hold a set value; clang-tidy flags a static member called through an instance.
8. Canonical names. `canonical_name(mask)` returns a small value type `blade_name` with a `text()` view, so it works in constant evaluation and at run time without allocation.
9. Bound of compact names. `max_compact_generators = 9`, a named constant for the recorded choice "one decimal digit per index" (STYLE S12); a `static_assert` keeps the literal 9 in the messages in step with it, as `bits.hpp` does for 32.

## Consequences

- Later units (the plan generator, `mv`) take blade sets as `template <blade_set S>` and read `S.blades`. A user names a set with `blades<4, "e41 e42 e43 e23 e31 e12">`; `blades<rga3, ...>` with an algebra tag is not provided, because no unit uses it yet.
- `index_of` costs one variable template instantiation per (set, mask) pair. If a later unit needs many lookups, a `constexpr` position function over values can serve it, with the compile-time error moved to the caller's concept (`has_blade`, decision 0033).
- Test cost (track-only): `blade_set_test.cpp` compiles in about 0.7 s (GCC) and 1.1 s (Clang) at -O0, dominated by the exhaustive sweeps in the test, not by the header (Clang time trace, plan 0008 record).

## Note after the W5 review (2026-10-31)

Added after the W5 review (`docs/agent/records/reviews/2026-10-31-wu4-blade-sets.md`); the record above is unchanged.

- Item 1: the reviewer compiled the planned spelling. A consteval function returning `blade_set<count_names(text)>` fails on both compilers: GCC says "not a constant expression", and Clang "non-type template argument is not a constant expression". The "inferred" there is now checked by the reviewer. The reviewer also found no other C++23 spelling that keeps `blades<N>(...)`, and would keep `blades<N, "...">`.
- Item 2: `make_merge` and `make_index_of` return no placeholder after a failed check, because their result type does not depend on the check. Each misuse still gives exactly one error, as the review confirmed on both compilers.
- Measured limits (finding N5; the reviewer's measurement, repeated by the author with `-std=c++23 -fsyntax-only` and default limits):
  - Clang 23.1.4 builds `full<13>` and `of_grade<32, 3>` (4,960 blades), and fails on its step limit for `full<14>`, `of_grade<16, 8>` (12,870), and `of_grade<32, 4>` (35,960).
  - GCC 16.2.1 builds `full<15>`, `of_grade<16, 8>`, and `of_grade<32, 4>`, and fails `full<16>` on `-fconstexpr-ops-limit`.
  - So the plan generator and `mv` do not rely on `full<N>` above n = 13. The `///` comments of `full` and `of_grade` state these limits.
- The Consequences bullet on test cost stands. The header's own cost to every includer was not stated there, and the plan's first figures for it were wrong (finding B1).
  - Measured by the reviewer at identical flags, both kinds of core added, from 2976dca to f9441f7: `#include <galoot/core.hpp>` alone rises from 0.488 G to 0.721 G instructions on GCC (+48%, 59 to 70 MB) and from 0.378 G to 0.548 G on Clang (+45%).
  - Most of the rise is `<string_view>`: the base plus `<string_view>` alone costs 0.665 G on GCC and 0.518 G on Clang. The blade set code itself costs about 0.03 G.
  - If that matters, `canonical_name` and the parser could work on character arrays and pointers instead of `std::string_view`. That is a follow-up for the owner to weigh, not done here.

## Note, 2026-10-31 (plan 0009)

`merge` has a much lower size limit than `full` and `of_grade`, because it inserts each blade into a list kept in canonical order, O(K²) for K blades (finding A6 of `docs/agent/records/reviews/2026-10-31-unreviewed-fix-passes.md`). Measured by the author on the header of `ddb7d2b`, with `-std=c++23 -fsyntax-only` at default limits, each compile logged in `runs.tsv` as `merge-bound:<compiler>:full<k>`:

- Clang 23.1.4 merges `full<7>` with itself (128 blades each) and fails on its step limit for `full<8>`.
- GCC 16.2.1 merges `full<9>` (512 each) and fails `full<10>` (1,024 each) on `-fconstexpr-ops-limit`.

These agree with the reviewer's figures. The `///` comment of `merge` states them. A merge of large sets would need a sort of each input and a linear merge instead of the insertion, which no present use needs.

## Note, 2026-11-01 (plan 0009, W5 review finding F1)

The `merge` limit in the previous note held only for what was measured: a canonical set merged with itself. The insertion's cost depends on the order and overlap of the inputs, not only on their sizes (review `docs/agent/records/reviews/2026-11-01-blade-set-merge-fix.md`).

Measured again by the author on the header of `e6f6601`, whose code `28ef08e` leaves unchanged apart from comments and `[[nodiscard]]`, with `-std=c++23 -fsyntax-only` at default limits, each compile logged in `runs.tsv` as `merge-bound:<compiler>:<shape><n>`. The disjoint inputs are the even-grade and odd-grade halves of `full<n>`, merged in both orders; "reversed" is `full<n>` listed in reverse, merged with itself.

| Shape | Clang 23.1.4 | GCC 16.2.1 |
| --- | --- | --- |
| `full<7>` with itself | passes | passes |
| `full<7>` reversed, with itself | passes | passes |
| Two disjoint sets of 64 blades | passes | passes |
| Two disjoint sets of 128 blades | fails on its step limit | passes |
| `full<8>` with itself, and reversed | fails on its step limit | passes |
| `full<9>` with itself | fails | passes |
| Two disjoint sets of 256 blades | fails | passes |
| `full<9>` reversed, with itself | fails | fails on `-fconstexpr-ops-limit` |
| Two disjoint sets of 512 blades | fails | fails on `-fconstexpr-ops-limit` |

This agrees with the reviewer's figures.

- For one of the GCC 512 + 512 failures, GCC's first error names the bounds check of `std::array<bool, 1024>`, not the operation limit. With `-fconstexpr-ops-limit=4294967296` both orders compile and give 1,024 blades. So that error is how GCC reports the exhausted limit inside the subscript check, not an access out of range (inferred from that result).
- The `///` comment of `merge` states these cases.
