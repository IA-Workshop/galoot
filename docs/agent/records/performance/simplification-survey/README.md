# Simplification survey (plan 0048)

Measurements of `include/galoot/` at `c95fee5`, made on 2026-11-21 for plan 0048. Read-only on the tree; the compile probes used scratch copies of `include/`.

| File | Holds | Made by |
| --- | --- | --- |
| `lines.tsv` | Lines, blank lines, comment lines, code lines, `static_assert`, template heads, concepts and macros per header | `metrics.py` |
| `comments.txt` | Comment lines by kind (prologue, `///` and `//`, public and `detail`), the pattern scan for citations of records, and for narration of process | `comments.py` |
| `duplication.txt` | Duplicated token windows (exact with strings masked, window 40; identifiers normalized, window 60) and repeated comment sentences | `dup.py` |
| `messages.txt` | Every `static_assert` and `GALOOT_ASSERT` message, and how many remain distinct once the prefix before the first colon is dropped | `msgs.py` |
| `complexity-lizard.csv` | Lines, cyclomatic complexity and length per function, from lizard 1.26.1 (`lizard -l cpp include/galoot --csv`) | lizard |
| `compile-probes.tsv` | Wall time, peak memory and user instructions of single compiles of three test sources against three header variants | `probe.py` |

Header variants of the probes: `base` is the tree; `nocheck` returns at once from `defects_of`, `typed_defects_of` and `map_defects_of` in `plan.hpp`; `nocomment` drops every comment line that holds no `NOLINT`. `strip.py` writes the comment-stripped reading copies.

Run each script from the repository root. `probe.py` reads `GALOOT_ROOT` and expects the variants beside it. The probes ran outside `tools/timed` and the job pool, so they have no row in the run log. GCC 16.2.1, Clang 23.1.4.
