# 0001 Lessons from WU1, the build skeleton

- Unit: WU1 (plan 0002), branch `feature/build`. Recorded by Claude (agent, build-expert), 2026-10-31.

Each entry says what happened, the cause, and what now detects it (W6).

## 1. Nested perf sessions multiplex a pinned core

- What happened: the first pinned compiles of the doctest runner, made as `taskset -c 2 tools/timed build ... -- ninja -j1 <object>`, recorded every counter as `unavailable`, with warnings that each event ran 38% to 88% of its time.
- Cause: `tools/timed` ran `perf stat` with eight events around Ninja, and the compiler launcher ran a second `perf stat` with eight events around the compiler. Both sessions' events followed the compiler on CPU 2, so sixteen events shared eight counters. The plan's probe ran one session and saw 100%.
- What detects it now: `tools/measure` writes a multiplexed pinned counter as `unavailable` and warns, so the defect cannot hide as a scaled estimate. A pinned `build` step no longer takes counters itself (decision 0028). A pinned step of another name that runs measured compiles would still multiplex; the warning shows it.

## 2. A directory compile definition cannot be replaced per target

- What happened: `add_compile_definitions(GALOOT_CHECKING=...)` at the top level, with `set_property(TARGET ... PROPERTY COMPILE_DEFINITIONS GALOOT_CHECKING=0)` on one test program, gave `-DGALOOT_CHECKING=0 -DGALOOT_CHECKING=1` and a redefinition error under `-Werror`.
- Cause: CMake 4.4.5 adds the directory's definitions at generation in addition to the target's own, so replacing the target property does not remove them (observed; the CMake documentation was not reread to confirm the mechanism).
- What detects it now: the build fails under `-Werror` on any second definition with another value. The mode now travels on `galoot_core`'s build interface (decision 0030).

## 3. compile_commands.json omits the compiler launcher

- What happened: a check of `compile_commands.json` reported that no compile had the launcher, while Ninja's commands showed it on all of them.
- Cause: CMake leaves `CMAKE_CXX_COMPILER_LAUNCHER` out of the compilation database by design, so that tools reading it see the compiler.
- What detects it now: check the launcher with `ninja -C build/<preset> -t commands <target>`, as criterion 10's count does.

## 4. A one-time rewrite of a union-merged log doubles it at the next merge

- What happened: merging `develop` into `feature/build` left `runs.tsv` with two headers and the 252 earlier rows twice, padded and unpadded.
- Cause: the header extension wrote `--` into every earlier row, so every line changed. Against `develop`, which had only appended, the whole file was one conflict, and the union driver keeps both sides. Plan 0002 named this as risk 10 for the header line only.
- What detects it now: `grep -c '^utc_start'` above 1, or more than one field count from `awk -F'\t' '{print NF}' | sort -u`. `docs/agent/snippets/runs_tsv_merge.py` rebuilds the file; it reproduced the committed fix byte for byte. The merge of `feature/build` into `develop`, and each branch merging `develop` afterward, need it once.

## 5. clang-format reflows a long NOLINTNEXTLINE comment

- What happened: `// NOLINTNEXTLINE(check): <reason>` lines longer than 80 columns failed `format-check`. Reflowing them would have moved the code line away from the marker, so the suppression would have applied to a comment.
- Cause: clang-format wraps comments at the column limit.
- What detects it now: `format-check` fails on the long line. The reason goes on the lines above, and the marker line holds only the marker and check names.

## Amendment, 2026-10-31 (cleanup unit 1, plan 0007; W5 delta review finding D6)

Item 4's second detector, "more than one field count from `awk -F'\t' '{print NF}' | sort -u`", no longer detects anything. Rows written before a column existed are never padded (decision 0032, item 3), so a sound `runs.tsv` has rows of 9, 26, and 28 fields, and the check fires on every copy of it. What detects a doubled or mixed record now:

- `grep -c '^utc_start' <record>` above 1, as before.
- `tools/measure read <record> > /dev/null`, which exits 1 naming the line of a repeated header, an older shorter header left in the file, a row longer than the header, a blank line, or an empty file, and is tested by `tools.measure_record`.
