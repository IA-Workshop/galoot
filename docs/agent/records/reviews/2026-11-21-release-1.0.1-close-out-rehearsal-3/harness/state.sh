#!/usr/bin/env bash
# state.sh <clone>: every local ref and its object, HEAD, status, worktrees, merge state.
set -u
C=$1
echo "HEAD $(git -C "$C" symbolic-ref -q HEAD || echo detached) $(git -C "$C" rev-parse HEAD)"
git -C "$C" for-each-ref refs/heads refs/tags refs/replace --format='%(refname) %(objectname)'
echo "status:"; git -C "$C" status --short --untracked-files=all
echo "worktrees:"; git -C "$C" worktree list --porcelain | grep -E '^(worktree|HEAD|branch)'
echo "prune:"; git -C "$C" worktree prune --dry-run -v
ls "$C/.git" | grep -E 'MERGE|TAG_EDITMSG|^worktrees' || true
