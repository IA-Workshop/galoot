# 0232 Tools unit rulings

Date: 2026-11-10. The owner replied "Accept recommendations" to the report on the pool and `tools/mutate` unit (plan 0036). Each item quotes the options and the recommendation accepted.

1. **N3, a MAKEFLAGS FIFO path through a symbolic link.** Options: (a) leave it, since `tools/jobpool env` prints the resolved path and nothing uses such a path; (b) resolve with `realpath -m` and add a test, in a later tools unit. Accepted: (a).
2. **GCC "unjudged" where Clang says "killed"** for a guard reached through a `consteval` call or a `constinit` variable in a header (a known limit, decision 0231). Options: (a) leave it and read the Clang row when a spec meets the case; (b) extend the GCC rule to read the second error, with fixtures. Accepted: (a).
3. **D2 to D4, untested details in `tools/mutate`** (the cut of an object's output at the end of its job, the bound "up to the next error" beyond two errors, a reused variable named `output`). Options: (a) fold them into the next unit that touches `tools/mutate`; (b) a unit of their own now. Accepted: (a).
4. **The shared pool** keeps running the old tool until its idle stop; no action (decision 0228 item 9).
5. **Cleanup of merged worktrees and branches** (decision 0195 item 9, earlier approved): run now, each branch verified in `develop` by ancestry or by content before removal.
