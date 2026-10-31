# 0033 Catch-all form for misuse diagnostics

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner ("proceed as recommended"), accepting the recommendation after note 0010. Recorded by Claude (agent).

## Context

Spike C (note 0010) found that C++26 computed `static_assert` messages are unusable under `-std=c++23 -Wpedantic -Werror` on both compilers, and that a `void`-returning catch-all prints a follow-on error before its message. An `auto` return plus a placeholder return after the assertion (form P) gives one error with the message first, but makes `requires` and `is_convertible_v` report the misuse as well-formed. Form D (`= delete`) answers `requires` correctly but gives the raw failure text.

## Decision

Public functions get a constrained catch-all of form P. Conversion functions get the plain catch-all. Galoot's own concepts (`same_algebra`, `ring_scalar`, `has_blade`) test the data and never ask whether a call expression is well-formed. A feature-test macro is not used to select C++26 messages. Each catch-all has a compile-fail test on both compilers that requires the literal message (C9).

## Consequences

WU15 implements this form. A user who probes a Galoot function with `requires` gets a wrong answer for misuse; the documentation says so.
