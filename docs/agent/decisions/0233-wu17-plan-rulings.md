# 0233 WU17 plan rulings

Date: 2026-11-10. The owner replied "Accept recommendations" to the report on plan 0038 (WU17, M7). Each item quotes the options and the recommendation accepted.

1. **O1, D8 (collected plans in 1.0).** Not asked now. Asked at the M7 gate with research note 0027.
2. **O2, reading of DoD-24.** Options: (a) counts of the kernels as specified, with collected counts recorded beside them; (b) wait for D8. Accepted: (a). A later expected-value change is recorded if D8 admits collected plans.
3. **O3, Clang's out-of-line calls** in `transform(line, motor)` and `compose`. Options: (a) record the departure with its measured cost; (b) a core unit forcing inlining; (c) amend DoD-24. Accepted: (a); (b) only if a benchmark shows a cost beyond the criterion.
4. **O4, Terathon.** Options: (a) no Terathon code, compare with recomputed published counts and the in-repo form; (b) fetch and time it. Accepted: (a).
5. **O5, DoD-22 build mode.** Options: (a) pinned serial builds under the lock; (b) shared-pool builds. Accepted: (a).
6. **O6, refit scope.** Options: (a) a verify refresh, a full remeasure only on a trigger; (b) a full remeasure. Accepted: (a).
7. **O7, the evaluation counter.** Options: (a) WU17 does not use it and the M7 snippets review deletes it; (b) use it for a Clang-only count. Accepted: (a).
8. **O8, benchmarks.** Options: (a) built in every preset, run only under a `bench` label, smoke tests everywhere; (b) a new cache option. Accepted: (a).
9. **O9, compacting `runs.tsv`.** Options: (a) none in WU17; (b) compact at close. Accepted: (a).
10. **O10, the counting test as a CTest.** Options: (a) a CTest read as a structure check; (b) record the counts only. Accepted: (a).
