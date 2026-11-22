#!/usr/bin/env bash
# realstate.sh: read-only snapshot of what the close-out script could change in the real repository.
REAL=/home/brianm/projects/utils/math-core/galoot
cd "$REAL" || exit 1
echo "HEAD $(git symbolic-ref -q HEAD) $(git rev-parse HEAD)"
git for-each-ref refs/heads/main refs/heads/master refs/tags refs/heads/1.0.1 refs/replace --format='%(refname) %(objectname)'
echo "status:"; git status --short --untracked-files=all
echo "worktrees: $(git worktree list --porcelain | grep -c '^worktree ')"
git worktree list --porcelain | grep -x 'branch refs/heads/master' || echo "no worktree has master"
ls -d /home/brianm/projects/utils/math-core/galoot-master 2>/dev/null || echo "galoot-master absent"
ls .git | grep -E 'MERGE|TAG_EDITMSG' || echo "no MERGE_* or TAG_EDITMSG in .git"
