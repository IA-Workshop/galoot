# 0195: The owner's rulings on the open items after WU13

Date: 2026-11-07. Status: recorded. Source: the owner's reply to the list of open items in the report after decision 0194.

## Rulings

1. O6, the chain limit: investigate the cause first, as a small research unit before M5 (option c). Documenting the limit (a) is the fallback if the cause stays unclear. The bound is not loosened until it is understood.
2. Overflow: a padded magnitude that overflows keeps failing the check (a). The documentation says checking builds are for validation, not for extreme-range work.
3. The `-ffast-math` and `-ffinite-math-only` `#error` stays broad, over all of `core.hpp`.
4. Coarse IEEE types: amend CONVENTIONS 4.5 (a). Version 1.11 adds the clause that a rounding scalar too coarse for the error bound to hold, such as `bfloat16`, is not checked. The owner chose the amendment and has not yet seen the wording, so 1.11 is marked a draft until the owner confirms it.
5. Decision 0193's raised Clang step limit stays. Grade subsets (decision 0184) are revisited if a third source needs the raised limit.
6. Combining a test author's failing commit with its fix: allowed as the recommendation stated. It applies only to a build failure, not a test failure, only to a commit no one else has fetched, and the brief tells the reviewer. Decision 0180's proposal becomes standing practice on those terms.
7. The job pool was restarted by the owner and reads 23 of 23 slots free.

## Not covered

Cleanup of merged worktrees and branches (item 10) was not ruled on and stays open.
