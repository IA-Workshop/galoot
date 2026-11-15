# Plan 0040: public operations compiled alone, and whether each calls into the library

The surveys behind decision 0244's account of which public operations carry `GALOOT_ALWAYS_INLINE`. Each compiles one public operation as the only function of a unit, with the structure test's flags (checking off, `-ffp-contract=off`), and runs `test/tools/kernels_inline.py` on the assembly, so that a call or a tail jump into the library is found. Run by Claude (agent, Opus 5.5, implementer 1 of plan 0040) on 2026-11-11, each through `tools/timed` (rows `public-ops-survey:*` and `survey:*` of `records/performance/runs.tsv`). Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4 with libstdc++ 16, on x86-64.

## The first survey: 61 cases

`first_survey.py`: 61 operations on the rga3 preset's types (motor, flector, line, point, plane) and on `full<4>`, at `-O2` with the S4 warnings. It was rewritten here from the session's text after its scratch copy was overwritten; it writes the same 122 units, byte for byte, as the surviving ones of both runs.

- `out/first-721afe7.tsv`: at `721afe7`, the chain forced and no public operation. 27 of 122 compiles call: `geometric_product`, `geometric_antiproduct`, `wedge` and `antiwedge` and their operators on motors, flectors and full multivectors on both compilers, and `antidot` of full multivectors on GCC.
- `out/first-d47e8f9.tsv`: at `d47e8f9`, with the eleven public functions forced: all 122 inline.

This survey is the evidence for the eleven attributes of `d47e8f9`. It did not reach the projections of full multivectors or motors, or any algebra but rga3. The blind review's delta review found calls there (its D1), which the next survey reaches.

## The second survey: 320 cases, three algebras, three levels

`survey.py`: the delta review's survey script (its record `records/reviews/2026-11-11-pre-release-core-pass-delta-review/survey/`), unchanged: 17 unary operations on 8 types, 15 binary operations on 12 pairs, `transform` on 8 pairs, changes of basis, conversion, `truncate` and `part`. The types are rga3's `full<4>`, motor, line, point, plane and flector; conformal(3)'s `full<5>`, even multivectors, vectors and bivectors; and R(3,0,0)'s `full<3>`, rotors and vectors. Run at `390b55e` (`project` and `antiproject` forced as well) from a `git archive` copy of its `include/`, at `-O2`, `-Os` and `-O3`:

```
python3 -I survey.py <include dir> test/tools/kernels_inline.py out/<level>-390b55e.tsv -<level>
```

Each row: compiler, level, case, `inline`, `CALL` or `ERROR`, and the calls. The six `ERROR` rows per level are `compose` of full multivectors, which the library rejects by design (an operand holds blades of both parities).

| Level | Compiler | rga3 cases that call | conformal(3) | R(3,0,0) | inline |
| --- | --- | ---: | ---: | ---: | ---: |
| `-O2` | GCC | 0 | 12 | 0 | 305 |
| `-O2` | Clang | 0 | 14 | 0 | 303 |
| `-Os` | GCC | 7 | 13 | 0 | 297 |
| `-Os` | Clang | 22 | 40 | 3 | 252 |
| `-O3` | GCC | 0 | 1 | 0 | 316 |
| `-O3` | Clang | 0 | 11 | 0 | 306 |

At `-O2` no rga3 and no R(3,0,0) case calls on either compiler. The calls left are on conformal(3):

- GCC (12): `bulk` and the duals that reach it on full and even multivectors, `dot` of full and of even multivectors, and `project` and `antiproject` of full multivectors, which call `bulk`. Against the branch point (the delta review's `O2-8058589.tsv`, the same 320 cases), three of them are new: `dot` of even and of full multivectors, and `left_bulk_dual` of an even multivector; the other nine called at the branch point too. Six that called at the branch point now inline.
- Clang (14): the unary operations, `bulk` and three duals, `+`, `-` and `dot`, all on conformal(3) `full<5>`; each called at the branch point too, and Clang has no case that calls now and did not then. 64 that called at the branch point now inline.

At `-O3` no rga3 case calls; GCC calls `bulk` of a conformal(3) full multivector, and Clang 11 conformal(3) `full<5>` operations. At `-Os` both compilers call rga3 operations, among them functions of the structure test's probe on Clang (`Line + Line`, the conversion `Full{motor}`, the unary operations of full multivectors), as the blind review found at `721afe7`: the structure tests are run at `-O2` only.

What these surveys support is what decision 0244 claims: the rga3 operations the probe reaches, at `-O2` with the test's flags, on both compilers. They show, and the claim leaves out, calls on conformal(3) multivectors of all or of even blades at `-O2` and `-O3`, and many at `-Os`.
