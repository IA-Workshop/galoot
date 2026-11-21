# 0257 The deleted `==` hint in the guide has no rejected block; release 1.0.0 is pushed

Date: 2026-11-21. Source: the owner's message of 2026-11-21, "proceed as recommneded. 1.0.0 is pushed", answering the coordinator's report on the halted unit U5 of plan 0044.

## 1. The rejected block of plan 0044's definition of done, item 7

Context: decision 0252 item 8 ruled option (a) of the plan's O3, a guide note on where the deleted `==`'s hint appears and a `cpp rejected` block that a documentation test checks. Unit U5 wrote the note and halted on the block (W8). The documentation harness requires the expected text on every line that holds "error:", and both compilers print the hint `// use same_rows(a, b)` on a note line; a block that passes would need a change to `tools/doc_examples.py` and `test/documentation.cmake`, outside the unit's files.

Options put to the owner: (A) extend the harness with a second marker for text that must appear anywhere in the output, with its own tool test; (B) no block, the clause recorded as met by the header tests `core.basis_change_equality` and `core.basis_change_inequality`, which fail on both compilers if the hint leaves the header; (C) drop the one-error requirement for that block. The coordinator recommended (B), and (A) later if a second such case appears.

Ruled: (B). Item 7's second half is met by the two header tests and the guide paragraph. Plan 0044's gate reads the item so. No harness change is part of 1.0.1.

## 2. Release 1.0.0 is pushed

The owner pushed `master` and the tag `1.0.0`. Checked by the coordinator with `git ls-remote origin`: `refs/heads/master` and the tag's commit are both `2383e6d`. This closes the action of decision 0252 item 12. `main` on the remote is still at `c95fee5`; pushing `main` stays the owner's action.

Status: in force.
