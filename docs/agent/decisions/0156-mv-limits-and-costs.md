# 0156 Limits and costs of the multivector type (WU8a)

- Status: accepted
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), cpp-expert role, implementing WU8a of plan 0016, which assigns this record to its implementers; WU8b appends its own section.

## Context

Plan 0016 criterion 10 asks for the largest n for which `mv<Alg, full<n>, double>` builds with the direct repeated-mask check, the cost of the counted constructor per type, the per-use costs of a type, a construction, and a `get`, and the header cost before and after (W10), all track-only (C14). Reading 16 says the canonical-order fast path of the repeated-mask check is built only if measurement shows that the direct check lowers a ceiling a present use reaches.

## Measurements

From `docs/agent/records/performance/mv/README.md` (revision `0acef07`):

- `mv<R(n,0,0), full<n>, double>` builds at the default limits for n up to 10 on GCC 16.2.1 and 8 on Clang 23.1.4, in all three configurations of decision 0103; the pair comparison of the repeated-mask check is what stops n = 11 and n = 9.
- A new four-blade type costs about 2 M retired instructions; its first construction 6 to 8 M more on GCC and 2 to 3 M on Clang; `get` 0.3 to 0.4 M per use above a read by position.
- `core.hpp` alone costs 0.45 % (GCC) and 0.53 % (Clang) more after WU8a; `blade_set.hpp` alone 0.70 % and 0.69 % more, because it includes `metric.hpp`.

## Options weighed

1. Keep the direct O(K²) check. Its ceilings, 1,024 and 256 blades, are above every present use: the largest result set of a plan that builds has 64 blades.
2. Test canonical order first and compare pairs only otherwise, as reading 16 describes. It would raise the ceiling for sets in canonical order, which no present use needs, and adds a second path to verify.

## Decision

1. Option 1. The direct check stays; the fast path is not built (C12, S16). The limits are stated in `mv`'s `///` comment.
2. The costs above are recorded and no bound is proposed.

## Consequences

- A layout of more than 256 blades on Clang, or 1,024 on GCC, fails in `distinct_masks` with the compiler's step or operation limit. If a use reaches that (for example, a full algebra of nine generators as one type on Clang), the fast path is the remedy, by a new decision with its own measurement.
- The cost per type and per first construction makes a program that declares many types pay more than with the aggregate (plan 0016 risk 6); no aggregate exists to compare with now.

## WU8b, 2026-11-01

Recorded by Claude (agent, Opus 5.5), cpp-expert role, implementing WU8b of plan 0016. Measurements in `docs/agent/records/performance/mv/README.md`, section WU8b (headers of `7ee7d32`).

1. Limits (criterion 21): `apply` builds wherever its plan or map builds at the default limits, on GCC 16.2.1 and Clang 23.1.4, in the three configurations of decision 0103, up to the plan ceilings of decision 0113 (Clang: `full<5>` products, the dense `full<4>`, maps to `full<7>` and complements to `full<8>`; GCC: `full<6>` products, 4,096 terms in one fold, the dense `full<5>`, maps to `full<9>`). The ceilings are stated in `apply`'s comment. No plan Clang builds exceeds 1,024 terms, so the expansion is not chunked (decision 0154 item 5).
2. P2 (criterion 22): the fixed use of decision 0113 through `apply` grows by 13 M instructions (GCC) and 15 M (Clang) from n = 4 to 16, against 12 M and 13 M for the plans alone; the cause is the algebra's `metric_like` check and the multivector types' layout checks per n, as before. The question of decision 0107 item 4 stays the owner's.
3. Costs, recorded and not bounded (C14): `core.hpp` alone +1.2 % (GCC) and +0.9 % (Clang); a new product's kernel and result type 83 M instructions on GCC at -O0 (44 M at -O2) and 32 to 34 M on Clang, above its plan; a conversion 8 to 18 M, a truncation 6 to 13 M, an existing product applied again 3 to 6 M.
4. No change is made for these costs. If the kernel's cost on GCC at -O0 matters to a user, the candidates are fewer constant evaluations per term (each term reads the kernel's array five times as a template argument) and one instantiation of the term function per coefficient rather than per term and flag; neither is measured.
