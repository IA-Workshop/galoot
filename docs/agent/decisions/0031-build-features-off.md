# 0031 Unity builds, precompiled headers, modules, and TSan

- Status: accepted
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, WU1), under C18; a decision record takes effect when recorded, and the owner may reverse it.

## Context

Research note 0002, F10 and F11; STYLE S5 and S6; plan 0002, section 1, departure 1.

## Decision

- Unity builds and precompiled headers are off. Both hide missing includes (S6) and merge the per-file costs that P1 records. Note 0002 measured large CPU savings for a table shared across files, so they may return later as opt-in, measured presets under C3.
- C++ modules wait until a decision adopts them; `CMAKE_CXX_SCAN_FOR_MODULES` is OFF. The core must keep working from headers alone (C13), so a module could only wrap the headers.
- No TSan preset. S5 asks for a thread sanitizer for tests that run code in more than one thread, and no test does. The preset is added, with this record superseded, when the first threaded test exists. DoD-26 records TSan as not applicable until then.
- The asan presets build at `-O1 -g` with `-fno-omit-frame-pointer`, a level stated in each preset, because sanitized `-O0` code is slow and `-O2` can fold away the faults a test means to reach (inferred; not measured in WU1).

## Consequences

Build cost per test program is visible in `compile.tsv`. The first measured need for unity, PCH, modules, or TSan reopens this record.
