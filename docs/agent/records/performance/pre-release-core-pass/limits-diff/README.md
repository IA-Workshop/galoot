# The limits diff at the merge of plan 0040

- By Claude (agent, Opus 5.5), the merge-gate runner, on `merge/pre-release-core-pass-gate2` at `3cc90b4` (the core pass's three branches, the review records and `develop` merged; `include/` is that of `feature/pre-release-core-pass--public-ops` at `c4f8bae` with the comment fixes of `--comments2`).
- The check of plan 0041 section 9, step 5, deferred to this merge by the documentation lane's gate: every number and flag name removed from the comments under `include/` since `77cd947` is on `docs/guide/limits.md` or listed as dropped in `../limits-removed.md`; every heading a header's pointer names is a heading of the page; and every figure on the page still holds of the final headers.
- Result: the page needs no change.

## Removed figures against the page

`git diff 77cd947 3cc90b4 -- include` removes 254 lines. From its removed comment lines, 101 distinct numbers and flag names (every number with its thousands separators dropped, every `-f...`, `-O...` and `_GLIBCXX_...` token), and 58 phrases (`full<...>`, `of_grade<...>`, `n = k`, `R(...)`, `conformal(3)`, `rigid(3)`, a count with its unit, a compiler with its version, each flag with its value) were looked up on the page. All 101 tokens are on it. Of the phrases, three are not found verbatim: `4096 terms` and `full<n-1>`, which the page writes across a line break and with a minus sign (lines 166 and 279), and `114 lines`, the length of GCC's error for a one-blade value from an explicit-only source, which part C removed and `limits-removed.md` lists as dropped ("Heading: none").

## Pointers against the headings

The fifteen pointers under `include/` (in `basis_change.hpp`, `blade_set.hpp` three, `dual.hpp`, `linear.hpp` two, `mv.hpp` three, `plan.hpp` two, `product.hpp` two, `transform.hpp`) name six headings: "Compile-time limits: products and plans", "... blade sets", "... duals, dot products and projections", "... transform and compose", "... change of basis", and "Raising the limits". Each is a heading of the page. `cost.py comments` on the page's subsection "Steps and operations of each plan", with both figures files, passes.

## The page's figures against the final headers

Since `77cd947`, the page's revision, `include/` changed by the forced-inlining attribute, the packs of `run_sum` renamed, `detail::inverse_change` deleted, `basis_change`'s `==` deleted, and comments. None of these changes a constant evaluation, so the steps and operations the page counts stand. The library's constants the page states (`max_generators` 16, `mask_bits` 32, `max_compact_generators` 9, `max_form_entry` 32,767 over an `int` `entry_type`, `coefficient_type` `std::int64_t`) and the rounding check's message are unchanged in the headers, and the page's contraction paragraphs (L1, L2 of decision 0243) agree with the `transform` and `apply` comments.

The attribute does change code generation, which the page's "builds" claims at -O0 and -O2 cover and which the earlier measurements took with `-fsyntax-only`. The extreme cases were compiled to objects (`-c`) with the final headers and the presets' warnings, `-std=c++23 -ffp-contract=off`, from the sources in this directory; the rows are in `docs/agent/records/performance/runs.tsv` under `limits-diff:`:

| Case | Flags | Page says | Result |
| --- | --- | --- | --- |
| scalar times full<10>, GCC | -O0 -g `_GLIBCXX_ASSERTIONS` `-fconstexpr-ops-limit=134217728` | builds | exit 0, 9.6 s |
| scalar times full<10>, GCC | -O2 `-fconstexpr-ops-limit=134217728` | builds | exit 0, 5.2 s |
| scalar times full<10>, GCC | -O0 -g `_GLIBCXX_ASSERTIONS`, default limit | fails, possibly as the `<span>` check | fails, as the `<span>` check |
| full<6> times full<6>, `*` and `%`, GCC | -O0 -g `_GLIBCXX_ASSERTIONS`; -O2 | builds | exit 0, 7.8 s and 5.8 s |
| full<5> times full<5>, `*` and `%`, Clang | -O0 -g `_GLIBCXX_ASSERTIONS`; -O2 | builds | exit 0 both |
| scalar times full<8>, Clang | -O0 -g `_GLIBCXX_ASSERTIONS` | fails at the default steps | fails, "hit maximum step limit" |
| scalar times full<7>, Clang | -O0 -g `_GLIBCXX_ASSERTIONS` | (below the limit) | exit 0 |
| antiproduct of two full<5> of conformal(3), Clang | -O0 -g | builds | exit 0 |
| the same, Clang | -O2 | fails | fails, "hit maximum step limit" |
| the same, Clang | -O2 `-fconstexpr-steps=1200000` | builds | exit 0 |
| the same, GCC | -O2 | builds | exit 0 |
| full<7> plus the bivectors, Clang | -O0 -g `_GLIBCXX_ASSERTIONS`; -O2 | builds | exit 0 both |
| full<9> plus the bivectors, GCC | -O0 -g `_GLIBCXX_ASSERTIONS`; -O2 | builds | exit 0 both |

Not rechecked: the measurements that need no code generation (the blade sets, `merge`, the maps, the duals, `transform` and `compose`, the change of basis), which rest on constant evaluation alone, and the multivector type's own sizes, whose constructors carry no attribute.
