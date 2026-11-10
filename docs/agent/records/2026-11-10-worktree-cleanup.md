# Worktree cleanup, 2026-11-10

Removed the merged worktrees under `.claude/worktrees/` and their branches, as
decision 0232 item 5 and decision 0195 item 9 allow. History was not
rewritten; `master`, the job pool, and builds were not touched. A removed
branch can be recovered with `git branch <name> <tip>` while the commit is
still in the object store or the reflog.

## Checks made for each worktree

1. Resolved the absolute path.
2. Looked for tracked changes and untracked files that are not ignored. The
   only changes found were additions to `docs/agent/records/performance/runs.tsv`;
   no `agent-cost.tsv` changes. A worktree whose added rows are not all in
   develop's `runs.tsv` was left in place.
3. Checked that the branch's commits are in develop, by one of:
   - ancestor: the tip is an ancestor of develop;
   - patch: every `git cherry develop <tip>` line is `-`, so each commit has a
     patch-identical commit in develop;
   - content: for `+` lines, each commit has a develop commit with the same
     subject (a cherry-pick that conflicted in `test/CMakeLists.txt` or the
     run log), the test files at the tip equal develop's or are an earlier
     version of develop's (develop's copy adds the later tests; changed lines
     are comment rewording and later refactors), and every `runs.tsv` row on
     the branch is in develop. The `rga3` test files moved to
     `test/presets/rga3/` and were compared there.

## Removed

| Worktree | Branch | Tip | Verified by |
| --- | --- | --- | --- |
| core-duals-tests | feature/core-duals-tests | e6d40aa83c6e38be69a2f3568eda94b1b348a540 | content |
| core-format--tests | feature/core-format--tests | 45122d7acace4228e70415073d4252546f876bbc | content |
| core-transform--math | feature/core-transform--math | 960bdb4f4d96fe85650423bd5fa00dccf9d6065a | content (two of three commits by patch) |
| core-transform--rounding--tests | feature/core-transform--rounding--tests | 61d731ec7ef965930e8c49d04dfd8cf098e6f4bf | content |
| core-transform--tests | feature/core-transform--tests | 558f8cda68379dcafce2e472477bdb318821c8b0 | content |
| evidence-tools--tests | feature/evidence-tools--tests | bf3e011a84e97bab2ac6ec6a2733a5266ecdec2d | content |
| gcc-wrong-code-scan | records/gcc-wrong-code-scan | 4506b8783581446c4ed97f35343129c84acf34b8 | patch |
| hci-wu15 | records/review-wu15-hci | 0c2929e98db2edfe673813e35a01778dff8459b1 | patch |
| hci-wu16 | records/review-wu16-hci | 5a8aa9a1ea1eab303be16c4f654fb1cc4d413020 | patch |
| logician-mut | (detached) | c86cb5825135a540347da4161b6d6617cf917ce3 | ancestor |
| pool-runlog-repairs--tests | feature/pool-runlog-repairs--tests | 1ac84b7e7a81ff4dfa61b5727bcc08610d093b52 | content |
| pool-surplus-fix--tests | feature/pool-surplus-fix--tests | 950fad115fc305f066063531aa67cdbed1dcb729 | patch |
| presets-rga3--tests | feature/presets-rga3--tests | ceac04244ee39821a67d60aba06330191101061b | patch; its 4 uncommitted `runs.tsv` rows are in develop |
| review-pool-delta | records/review-pool-delta | 5f25ce4f23ecc01f6804cffad93a826d73bcd814 | patch |
| review-pool-runlog | records/review-pool-runlog | 55e2d48c9669f5a53306e4fabca29c422cfe682f | patch |
| review-presets-rga3 | records/review-presets-rga3 | f6b8b5287792dcb15bf12b2ee7ce94cda145cff2 | patch |
| review-tools-delta | records/review-tools-delta | 7682430d7cbc1ff18737314574123c378935687a | patch |
| review-wu13a | records/review-wu13a | 22a390935a9ba52d155292439308cea5da04bdfc | content (review files identical) |
| review-wu13a-delta | (detached) | 01835b78e25477baf82d44001eb1b2adf4f58c87 | ancestor |
| review-wu13b | records/review-wu13b | 2396137490e2252ed480fc6ded47e53c50c6e4ff | content (review files identical) |
| review-wu13b-delta | (detached) | c98eee2a5a58036c073dffa764a1dfa482f5c00d | ancestor |
| review-wu15 | records/review-wu15 | 694008c60a91cd2a73b7266ea9ac71c999a07f17 | patch |
| review-wu16 | records/review-wu16 | 86eefddb1e155a8814ffd107df701ce014db36d7 | patch |
| rga3--tests | feature/rga3--tests | ebb11eecd32f08507e6cb479f12a486510428953 | content (files now under `test/presets/rga3/`) |
| rotation-normalize--tests | feature/rotation-normalize--tests | 53c3464859d5f27f70f09b34a3c7625eff9a99f4 | patch |
| scalar-full10-gcc | records/scalar-full10-gcc | e3aafb59e5334f8b5ed9bb3e8c60f8b80f174fdd | ancestor |

Branches were deleted with `-d` where git saw them as merged (scalar-full10-gcc)
and `-D` otherwise, after the checks above.

## Left in place

Each has uncommitted `docs/agent/records/performance/runs.tsv` rows that are
not in develop's copy. Deleting the worktree would lose them.

| Worktree | Branch | Tip | Uncommitted rows not in develop |
| --- | --- | --- | --- |
| review-evidence-tools | records/review-evidence-tools | dc2d1d99368fea5e5f8e6805c172ee1083ebec63 | 28 of 28 |
| review-rotation-delta | records/review-rotation-delta | 6f856f6a58d2833e1f500e0225d136bfecbe919e | 17 of 17 |
| review-rotation-normalize | records/review-rotation-normalize | a8196c5c7010e6973dad931e69a3999a7b6ada1a | 50 of 50 |
| rotation-normalize | feature/rotation-normalize (an ancestor of develop) | e86103ad8c2031c30b9defc181e845e6073d7714 | 1 of 1 |

The owner decides whether to append these rows to develop's run log or discard
them.
