# Simplification survey: candidates and baselines (plan 0048, unit D0)

Records of the first part of unit D0 of [plan 0048](../../plans/0048-aggressive-simplification.md), made on 2026-11-21 by an agent (Claude Opus 5.5, cpp-expert role). Everything here was measured at `01dce7f` on `main`. Its `include/` tree is identical to `c95fee5`, the revision the plan surveyed (`git diff --stat c95fee5 01dce7f -- include test tools` is empty), so line numbers here and in the plan agree. Nothing under `include/`, `test/` or `tools/` was changed.

The second part of D0, the comment table `records/docs/comment-moves.tsv` for plan 0049 (decision 0253, "Hand-overs"), is in [records/docs/](../docs/README.md).

## Files

| File | Holds |
| --- | --- |
| [candidates.tsv](candidates.tsv) | Every site plan 0048 expects to remove or rewrite, by unit of its section 7, with file, line range, what replaces it, and how many test files name it |
| [restated-targets.md](restated-targets.md) | The plan's section 5.7 totals restated from these measurements, with the open items, for the owner before phase 1 |
| [knowledge-base-reading.md](knowledge-base-reading.md) | The committed knowledge-base notes on complexity reduction, read in full against the plan |
| `baseline/` | The outputs of `measure.sh` at `01dce7f` |
| `measure.sh` | Reruns every baseline below in about four seconds |
| `inventory.py` | The public interface, from Clang's view of the headers |
| `entities.py` | The line range of every namespace-scope entity, public and `detail` |
| `check_candidates.py` | Checks each row of `candidates.tsv` against `entities.py` and sums the lines each unit opens |
| `duplication_by_site.py` | The plan's duplication measure, with each duplicated token traced to a unit |
| `clones_lizard.py` | lizard's clone detector at several minimum lengths |
| `comment_census.py` | The knowledge-base comment census, crossed with the plan's position classes |

To rerun everything, from the repository root:

```sh
docs/agent/records/simplification/measure.sh /tmp/d0-rerun
```

Run that way, the outputs go to `/tmp/d0-rerun`. With no argument the script overwrites `baseline/`, which is right only when the baseline is meant to move.

## How the measurements ran

- No build of the test suite and no compile of a test source ran. The job pool was not used and its state was not read, since nothing ran through it. Nothing was timed, so the timing lock was not taken.
- The tools ran directly in the shell, single-threaded, in seconds:
  - `clang-query` (Clang 23.1.5), parsing one translation unit that includes `core.hpp`, `core/format.hpp` and `presets/rga3.hpp`;
  - `g++` 16.2.1 in its comment-stripping mode;
  - Python 3.14 scripts;
  - lizard 1.26.1 through `uvx --from lizard==1.26.1`, already in uv's cache (a fresh machine fetches it from PyPI; nothing is installed).
- The knowledge-base exercise scripts were run read-only from their worktree to reproduce the notes' figures.
- The plan's survey scripts (`records/performance/simplification-survey/`) were rerun on this tree. Their outputs are byte-identical to the committed ones: `lines.tsv`, `comments.txt`, `duplication.txt`, `messages.txt`.

## Lines per header

`baseline/lines.tsv`, from the plan's `metrics.py`. A line is blank, a comment line (its first non-space characters are `//`), or code. The 73 code lines that also carry a trailing comment count as code. There are no block comments, only 70 `/*name*/` markers on unnamed parameters, inside code lines. GCC's own comment removal (`baseline/code-lines-gcc.tsv`) gives the same code count for every header.

| Header | Lines | Code | Comment | of which `///` | of which `//` | Blank |
| --- | --- | --- | --- | --- | --- | --- |
| `core/assert.hpp` | 143 | 45 | 86 | 49 | 37 | 12 |
| `core/basis_change.hpp` | 1,285 | 764 | 438 | 177 | 261 | 83 |
| `core/bits.hpp` | 186 | 65 | 98 | 74 | 24 | 23 |
| `core/blade_product.hpp` | 301 | 141 | 132 | 82 | 50 | 28 |
| `core/blade_set.hpp` | 645 | 405 | 185 | 101 | 84 | 55 |
| `core/dual.hpp` | 679 | 367 | 265 | 148 | 117 | 47 |
| `core/format.hpp` | 272 | 160 | 90 | 43 | 47 | 22 |
| `core/linear.hpp` | 482 | 247 | 194 | 104 | 90 | 41 |
| `core/metric.hpp` | 318 | 163 | 118 | 77 | 41 | 37 |
| `core/mv.hpp` | 1,342 | 778 | 462 | 263 | 199 | 102 |
| `core/plan.hpp` | 949 | 529 | 366 | 236 | 130 | 54 |
| `core/product.hpp` | 457 | 274 | 151 | 95 | 56 | 32 |
| `core/transform.hpp` | 845 | 449 | 345 | 146 | 199 | 51 |
| `core/unary.hpp` | 132 | 48 | 68 | 35 | 33 | 16 |
| `core.hpp` | 21 | 15 | 3 | 0 | 3 | 3 |
| `presets/rga3.hpp` | 13 | 5 | 5 | 0 | 5 | 3 |
| `presets/rga3/motions.hpp` | 194 | 75 | 105 | 84 | 21 | 14 |
| `presets/rga3/objects.hpp` | 146 | 31 | 97 | 67 | 30 | 18 |
| **Total** | **8,410** | **4,561** | **3,208** | **1,781** | **1,427** | **641** |

These are the plan's section 3.1 figures, unchanged. By position (`baseline/comments.txt`) the comments are:

| Position | Lines |
| --- | --- |
| Prologue | 702 |
| `///` on public declarations | 1,338 |
| `///` on `detail` | 443 |
| `//` elsewhere | 725 |

Those are also the plan's figures.

## Public names and declarations

`baseline/inventory.tsv`, from `inventory.py`. It is one row per declaration, with its kind, name, location, whether it only rejects a call, and its signature.

**Method.** A declaration is public when its semantic context is `::galoot::core` or `::galoot::presets::rga3` and it is not in a `detail` namespace. That is the scope of plan 0047's manifest (its section 5.3), so these counts can be compared with the manifest once it exists. Counting rules:
- Each declaration a user can name counts once.
- A template counts as one declaration, not two: the `FunctionTemplateDecl` and the `FunctionDecl` it describes are one thing to a user.
- Each overload counts.
- A forward declaration and its definition count once (`mv` is declared at `mv.hpp:117` and defined at `:507`).
- Implicit instantiations, which the preset's layout definitions cause while the headers are parsed, are not declarations and are not counted.
- A name is distinct per namespace. No name appears in both.

**Counts at `01dce7f`:**

| Tier | Declarations | Distinct names | Of which only reject a call |
| --- | --- | --- | --- |
| Namespace scope (the manifest's scope) | 153 (152, plus one partial specialization, `sum_layout<Alg, A, A>`) | 110 | 37 |
| Public members of those classes (constructors, member functions, data members, nested aliases) | 34 | 24 | 5 (`mv`'s rejecting constructors) |
| Namespace `std` (the `std::formatter` partial specialization in `format.hpp`) | 1 | 1 | 0 |

The interface a user reads is therefore 188 declarations. Of these, 42 exist only to turn a misuse into a message, which matches the plan's "about 42" item by item. The 110 names include 25 ending in `_layout`: 20 are the result layouts of core operations and 5 the preset's object layouts.

**Why the earlier figures differ.** The plan's 97 and 263 and the research cycle's 110 and 293 come from one AST matcher, run twice. The first run gave the preset's namespace as `::galoot::rga3`, which matched nothing (knowledge-base commit `74ef874`). The corrected run added the preset's 13 names and 30 rows. So 110 is the right number of names, and 97 is the core and `format.hpp` alone. The declaration counts differ from 153 because that matcher lists what the AST holds, not what a user declares. The 140 extra rows of the 293 are:

| Extra rows in the 293 | Rows |
| --- | --- |
| Pattern declarations of templates, listed beside their template: 86 function, 25 variable, 5 alias, 5 class (one of them the pattern of `mv`'s forward declaration) | 121 |
| Implicit instantiations made by the preset's definitions: 6 class (`metric<4>` and `blade_set<K>` for each of five layout sizes), 10 variable (the five `blades<"...">` of the preset's layouts, each listed twice), 2 function (`R<3,0,1>`, `rigid<3>`) | 18 |
| `mv`'s forward declaration, counted beside its definition | 1 |
| **Total** | **140** |

The 293 reproduces exactly from the notes' matcher (`src/api.query` of the exercise) on this tree. So does the 153, from `TRAVERSAL=IgnoreUnlessSpelledInSource python3 -I docs/agent/records/simplification/inventory.py`. That traversal reaches neither instantiations nor pattern rows, and it gives the same namespace-scope and member rows as the default traversal, though not the `std` row.

## Duplication

Three measures, all rerun on this tree (`baseline/duplication-by-site.txt`, `baseline/clones-lizard.txt`).

| Measure | What it counts | Value |
| --- | --- | --- |
| The plan's type 1 | Tokens in an exact window of 40 tokens that occurs twice anywhere in the headers. Comments removed, every string literal masked as one token | 12.6% of 40,192 tokens |
| The plan's type 2 | The same with every identifier that is not one of 22 keywords replaced by one symbol, window 60: any two stretches of the same syntactic shape | 25.6% |
| lizard 1.26.1, `-Eduplicate` | Tokens in a repeated run of at least 70 tokens per copy (for a pair), with identifiers and keywords renamed consistently within a scope (the first new name is `v0`, the next `v1`), constants written as `1`, minus as plus; renaming stops where more than 6 of the last 31 tokens are constants | 2.78%, five clusters, 235 lines |

**How much each depends on its threshold.**

| Window or minimum length (tokens) | Type 1 | Type 2 | lizard |
| --- | --- | --- | --- |
| 20 | 34.3% | 68.4% | |
| 30 | | | 12.03% |
| 40 | 12.6% | 38.4% | |
| 50 | | | 7.19% |
| 60 | 4.8% | 25.6% | |
| 70 | | | 2.78% |
| 80 | 0.8% | 18.5% | |
| 100 | 0.0% | 14.4% | 1.94% |
| 150 | 0.0% | 6.9% | 0.00% |

None of the three has a natural threshold. Each figure means something only beside its threshold, and only as a trend on one tree.

**What each sees.** lizard finds the deliberate wrapper shapes:
- `product.hpp`: a seven-copy and a four-copy cluster;
- `dual.hpp` and `linear.hpp`: a pair each;
- `mv.hpp`: `truncate` against `part`.

lizard misses the two message families (16 spellings each), because the literals that distinguish them stop its renaming. Type 2 sees those families, and also `#include` blocks (9.9% of its duplicated tokens) and the uniform one-line wrappers.

**What it means for the plan's savings.**
- 64.4% of the type-2 duplicated tokens lie inside the line ranges of `candidates.tsv`:

  | Unit | Share of the type-2 duplicated tokens |
  | --- | --- |
  | E1 | 23.2% |
  | E2 | 20.0% |
  | B4 | 12.9% |
  | C1b | 12.1% |
  | B8 | 3.7% |

  So the ruled option C (E1, E2) is where the plan's own duplication figure can fall.
- 35.6% lies outside every site: include lists, forwarding wrappers, the `_layout` declarations, the preset's objects. No unit is meant to remove it, so 25.6% cannot approach zero. The plan's units remove the copies, not the shapes, so a lower bound of about 9% (that 35.6%) is likely.
- lizard's 2.78% says the remaining duplication is mostly deliberate. It also says that a duplication measure is a weak proxy for what a reader must hold in mind.

`restated-targets.md` restates the target on these terms.

## The candidate list

[candidates.tsv](candidates.tsv): 147 rows, one site each, in the order of plan 0048's section 7. Columns:

| Column | Holds |
| --- | --- |
| `unit` | The unit of plan 0048 section 7 that opens the site. C0 is a spike on a scratch branch and opens no site; C1a only adds primitives |
| `class` | The plan's class (a) to (d) |
| `file`, `lines` | The header and the line range at `01dce7f`. For a named entity, the range is the declaration's own source range from Clang, without its comment: the `///` block above a deleted entity goes with it. For a part of a function, the lines of that part |
| `entity` | The entity's name; `entities.py` finds its range again at a later revision |
| `action` | `delete`; `merge` (with the other rows of the unit); `rewrite` (the declaration stays, the body changes); `wrap` (becomes a two-line wrapper); `move`; `keep` (a site the unit reads and must not lose) |
| `replaced_by` | One line on what replaces it |
| `test_files` | How many test files name the entity, or the variable template holding its table, through `detail::` (a rerun: `grep -rlE 'detail::<name>\b' test`) |
| `stays` | `yes` where plan 0048 section 6.2 keeps the name and its shape |
| `generator` | `pinned` where the code is on the path that `product_plan` or `unary_map` evaluates, whose exact step and operation counts `core.plan_cost_model` pins; `yes` where it is other constant-evaluated table code; `no` otherwise |
| `condition` | What the site waits for, and any finding against the plan |

`python3 -I docs/agent/records/simplification/check_candidates.py baseline/entities.tsv` confirms that every named range is the compiler's range for that entity: 147 rows, none mismatched. It also prints the lines each unit opens (`baseline/candidates-check.txt`). That is an upper bound on what the unit can delete, which `restated-targets.md` sets against the plan's estimates.

**For plan 0047's H0 and H1 to H4** (plan 0047 section 9.2 items 2 and 3):
- Write tests at the public boundary for the behavior of every row here.
- Do not add a test that names an entity in the `entity` column unless `stays` is `yes`.
- Where a test must name one, label it `internal` as item 3 says.
- The rows with a nonzero `test_files` already have such tests. Plan 0048 retires or changes them only by a recorded decision that names the public-level test covering the behavior:
  - `blade_image` (1 file)
  - `add_checked` and `multiply_checked` (1 and 2)
  - `map_term` and `typed_map` (2 and 1)
  - `defects_of`, `typed_defects_of`, `map_defects_of` (1 each)
  - `dropped_blade_count` (2)
  - `make_dropped_positions` through `dropped_positions` (1)
  - `make_scalar_part_kernel` through `scalar_part_kernel` (1)

**Readings and findings recorded in the rows:**
- **A1, `blade_image(const basis_change<N>&, mask_t)` (`basis_change.hpp:794-798`) is not dead.** The plan says it has no caller in headers or tests. `test/core/basis_change_identities_test.cpp:222` passes a `core::basis_change<N>` to `images_of`, which calls `core::detail::blade_image<capacity>(rows, a)` at `:75`, and overload resolution picks this overload. A1 can delete it only after that test is changed to pass `widened(change)` (W9), or not at all. `change_add` (`:304-310`) has no caller anywhere, as the plan says.
- **B9 is smaller than the plan says.** Of the 67 `NOLINT` lines in the headers, 23 are `modernize-avoid-c-style-cast`, not "most". They sit in seven headers, not nine. With the explanation above each, they hold about 45 comment lines, not about 90. The other 44 markers are for other checks (14 for `readability-use-anyofallof`, 7 for `performance-enum-size`, the rest for checks such as `readability-redundant-parentheses` and `cppcoreguidelines-macro-usage`). B9 does not touch them.
- **B2's "five builders" are not named in the plan.** The rows list the six loops of that shape in the four headers the plan names. Two rows say which are the reading.
- **B5's wrapper changes run-time behavior in one case.** At run time without checking, an overflowing `multiply_checked` today returns `x * y`, which is undefined. The wrapper over the recording form would return 0. In a constant evaluation nothing changes, since `GALOOT_ASSERT` always checks there.
- **K1 to K8 are listed per header prologue only.** The block-level table of comments is `records/docs/comment-moves.tsv`, the second part of D0.
- **Phase 4 moves wait for a ruling on D3.** Under decision 0253, a contract moves out of a header only after its receiving page passes review. Those pages are in plan 0049's D3, which is not yet authorized. Deletions that move nothing do not wait.
- **Not listed.** Section 5.6's 2.0 candidates are not listed, since the owner ruled no 2.0 batch (decision 0253 item 18). Nor are C1a's primitives, which add code and remove none.
