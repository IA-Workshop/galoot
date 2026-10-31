# 0006 Toolchain: platform-supplied compilers only

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D6). Recorded by Claude (agent).
- Owner amendment: agree, use the platform supplied compilers only.

## Context

The design study names GCC 15+ and Clang 19+. Installed are GCC 16.2.1 and Clang 23.1.4; no older compilers are present.

## Decision

The supported matrix is Linux x86-64 with the platform-supplied GCC and Clang (at present GCC 16.2.1, Clang 23.1.4) and libstdc++. No custom-built or side-installed toolchains. MSVC is unsupported. No untested minimum version is claimed.

## Consequences

The matrix changes when the platform's compilers change, recorded as a new decision.
