#!/usr/bin/env bash
# state.sh <clone>: what a run could change: every local ref and its object,
# HEAD, the working tree's status, the worktrees, and merge state.
set -u
C=$1
echo "HEAD $(git -C "$C" symbolic-ref -q HEAD || echo detached) $(git -C "$C" rev-parse HEAD)"
git -C "$C" for-each-ref refs/heads refs/tags refs/stash refs/replace --format='%(refname) %(objectname)'
echo "status:"; git -C "$C" status --short --untracked-files=all
echo "worktrees:"; git -C "$C" worktree list --porcelain | grep -E '^(worktree|HEAD|branch)'
echo "prune:"; git -C "$C" worktree prune --dry-run -v
ls "$C/.git" | grep -E 'MERGE|ORIG_HEAD|TAG_EDITMSG|worktrees' || true
