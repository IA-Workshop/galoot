# 0060 A mutation run must check the build before reading the tests

- Date: 2026-10-31
- Unit: WU3, metric (plan 0005), Claude (agent, Opus 5.5)

## What happened

To show that the metric tests can fail, a script applied a mutation to `metric.hpp`, rebuilt `core_metric_test` with its output discarded, and ran `ctest -L '^core\.metric$'`. Two mutations reported 7 of 7 passing. Both had broken the build: a `static_assert` in the test file rejected the mutated header, so Ninja stopped, and CTest ran the program left from the previous successful build. The same happened again in the review follow-up, where the build failure was the intended result and the stale "100% passed" line beside it would have read as a survivor.

## Cause

CTest runs whatever executable is on disk and does not know that the last build of it failed. A test program that does not compile is invisible to a CTest selection that does not also build it.

## What detects it now

Every mutation run prints the build's exit status (`tools/timed build ...` reports it) before the test result, and a mutation counts as killed when either the build fails or a test fails. Reading only the CTest summary is not enough. A build preset whose test step depends on a fresh build (`cmake --workflow`) avoids the trap for ordinary runs, but a hand-run `ctest` after a failed build does not.
