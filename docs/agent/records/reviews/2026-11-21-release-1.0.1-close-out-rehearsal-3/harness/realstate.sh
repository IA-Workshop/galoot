#!/usr/bin/env bash
# realstate.sh: read-only snapshot of what the close-out script could change in the real repository.
REAL=/home/brianm/projects/utils/math-core/galoot
cd "$REAL" || exit 1
echo "\$ git status (short, untracked all)"; git status --short --untracked-files=all; echo "(end status)"
echo "\$ git tag -l"; git tag -l
echo "\$ git rev-parse master main"; git rev-parse master main
echo "\$ git worktree list"; git worktree list
echo "--- extra"
echo "HEAD $(git symbolic-ref -q HEAD)"
git for-each-ref refs/heads/1.0.1 refs/replace refs/tags --format='%(refname) %(objectname)'
ls -d /home/brianm/projects/utils/math-core/galoot-master 2>/dev/null || echo "galoot-master absent"
ls .git | grep -E 'MERGE|TAG_EDITMSG' || echo "no MERGE_* or TAG_EDITMSG in .git"
echo "index sha256 $(sha256sum .git/index | cut -c1-16)"
