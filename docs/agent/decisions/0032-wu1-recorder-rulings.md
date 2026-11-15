# 0032 Rulings on the WU1 recorder questions and the commit trailer

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner ("continue proceeding with recommendations"), accepting the recommendations given on 2026-10-31. Recorded by Claude (agent). The number 0032 avoids the numbers 0024 to 0031 that `feature/build` uses.

## Decision

1. An optimization level or other option overridden at configure time is visible in the records: the compile and run records gain an `options` column that holds the effective compiler options of the build, so an override never needs its own preset to be traceable.
2. Pinned rows also record the busy time of other processes on the pinned CPU, beside the sibling thread's busy time.
3. A row in the old nine-cell format is read as `--` in every cell it lacks. Old rows are not padded.
4. From now on a commit by an agent carries a `Co-Authored-By` trailer naming the model that did the work, as the agent's own configuration reports it. Commits already made keep their trailers, since history is not rewritten (W7).

## Consequences

Items 1 to 3 are a change to the recorder (`tools/timed`, `tools/measure`) and are made in a small follow-up unit after `feature/build` merges (plan 0003). Item 4 applies at once.
