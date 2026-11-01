# 0096 Compile-fail tests compile with -c, not -fsyntax-only

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), build-expert for plan 0010, whose approach names `-fsyntax-only` and asks for it to be measured against `-c` on both compilers.

## Context

A compile-fail test (decision 0095) runs the compiler on a source that must not compile. `-fsyntax-only` stops after the front end; `-c` also runs code generation and writes an object. The plan asks whether a constraint or `static_assert` failure still appears with `-fsyntax-only`, which diagnostics appear only in code generation, and what each mode costs.

## Measurement

Every one of the 46 compile-fail sources at `b95e88e`, compiled with the exact command of its old object library from `compile_commands.json` of each of the six compiler presets, once with `-o <scratch>.o -c` and once with `-fsyntax-only`, each through `tools/measure tu` pinned to CPU 4 (sibling idle: at most 0.01 s busy during any compile). Run log rows: `syntax-only-vs-c`, one per preset. Scripts, the baseline `compile_commands.json` files, and outputs are in `docs/agent/records/spikes/compile-fail-modes/` (`compare_modes.py`, `out/modes-<preset>.tsv`, `out/mode-costs.txt`).

- Output: in all 276 pairs the exit status (1) and the whole diagnostic text are the same.
- Cost, summed over the 46 sources:

| Preset | `-c` instructions | `-fsyntax-only` | `-c` wall (pinned) | `-fsyntax-only` | `-c` peak RSS | `-fsyntax-only` |
| --- | --- | --- | --- | --- | --- | --- |
| gcc-debug | 26.91 G | 25.80 G (−4.1%) | 3.96 s | 3.77 s | 69,448 KiB | 65,172 KiB |
| clang-debug | 22.12 G | 21.32 G (−3.6%) | 4.04 s | 3.92 s | 99,052 KiB | 96,692 KiB |
| gcc-release | 26.55 G | 26.21 G (−1.3%) | 3.86 s | 3.82 s | 65,380 KiB | 65,384 KiB |
| clang-release | 22.07 G | 21.45 G (−2.8%) | 4.06 s | 3.98 s | 97,520 KiB | 96,800 KiB |
| gcc-asan | 28.19 G | 27.06 G (−4.0%) | 4.11 s | 3.94 s | 72,028 KiB | 68,012 KiB |
| clang-asan | 22.30 G | 21.46 G (−3.8%) | 4.07 s | 3.98 s | 99,484 KiB | 96,836 KiB |

Instructions are user-space instructions retired on the pinned performance core; peak RSS is the largest of the 46 compiles. One run of each.

Both compilers skip code generation once the front end has reported an error, so a source that fails as the tests intend costs and prints the same either way; the 1 to 4 percent is the work of setting up the back end (inferred).

## Diagnostics that appear only in code generation

Probe sources, one per diagnostic, compiled with each preset's flags (`-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror` and the `-O` level and sanitizers of debug, release, and asan), with `-c` and with `-fsyntax-only`. Each row is a diagnostic `-c` gives and `-fsyntax-only` does not (run log row `codegen-only-probes`; `run_probes.sh`, `probes/`, and `out/probes.tsv` in the same directory):

| Diagnostic | GCC 16.2.1 | Clang 23.1.4 |
| --- | --- | --- |
| Call to a function declared `[[gnu::error]]` | all three | all three |
| Call to a function declared `[[gnu::warning]]`, under `-Werror` | all three | all three |
| `-Wreturn-type`: control reaches end of non-void function | all three | none (front end) |
| `-Wuninitialized`: a variable used uninitialized | all three | none (front end) |
| `-Warray-bounds`: an index past the end, constant or implied by a branch | release only | none (Clang's front end reports the constant index and not the other) |
| `-Wstringop-overflow` / `-Warray-bounds` on `memcpy` past a buffer | debug and asan | none (front end) |
| `-Wuse-after-free` | all three | not diagnosed |
| `-Wfree-nonheap-object` | all three | none (front end) |
| `-Wunused-function`, `-Wunused-variable` for unused `static` names | all three | none (front end) |
| Failed `always_inline` inlining | all three (the front end already reports the missing definition) | none |
| Inline assembly the assembler rejects | all three (`as` reports it) | all three (integrated assembler) |

A source with both a failing `static_assert` and a call to a `[[gnu::error]]` function prints only the `static_assert` with `-c`, on both compilers: once the front end fails, code generation does not run. So a code-generation diagnostic can make `-c` fail where `-fsyntax-only` succeeds, and never the reverse. No current compile-fail test relies on one.

## Options

1. `-fsyntax-only`, as the plan suggested. Saves 1 to 4 percent of the compiler's instructions. A future test whose rejection is one of the diagnostics above would see its source compile and fail, which is safe but would need the mode changed back. `tools/measure` takes the source from the argument after `-c` and the object from `-o`, so its compile rows would hold `unavailable` for both. Every compile-fail row of a tree would then have the same preset, source, and object, which the gate copy step (decision 0080, second amendment) refuses as a repeated compile, and the record would no longer say which source a cost belongs to. Keeping the rows would mean teaching `tools/measure` a second way to find the source and a key other than the object.
2. `-c`, with an object named per test. The same output on every current source, the diagnostics above kept, and compile rows with their source and a distinct object, so the compile record keeps every compile-fail unit as before.

## Decision

Option 2. The object goes to `<module>_<name>_compile_fail.o` in the test directory of the build tree, which the compiler writes only when the source compiles, a case in which the test fails anyway.

## Consequences

- A compile-fail test costs a few percent more compiler work than it would with `-fsyntax-only`; the measured CTest time of the group is in plan 0010's outcome.
- A rejection that only code generation reports can be tested with no change to the helper.
