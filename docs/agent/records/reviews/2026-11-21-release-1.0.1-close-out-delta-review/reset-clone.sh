#!/usr/bin/env bash
# reset the scratch clone to its starting state (scratch only)
S=<scratch>
cd "$S/clone" || exit 1
[ "$(pwd -P)" = "$S/clone" ] || exit 1
rm -f .git/hooks/post-checkout .git/hooks/pre-merge-commit .git/hooks/post-merge
git config --unset tag.gpgSign 2>/dev/null; git config --unset gpg.program 2>/dev/null
git config --unset remote.origin.pushurl 2>/dev/null
git config --remove-section url.https://example.invalid/ 2>/dev/null
git worktree remove --force --force "$S/clone-master" 2>/dev/null
rm -rf "$S/clone-master"; git worktree prune
git checkout -q main 2>/dev/null; git update-ref refs/heads/main 704f152c64fd3f01e39a66640a254443f3421c0a; git reset -q --hard main
git branch -f master 2383e6df5e9dd9e79832a8dbb3d8af9207a5913f 2>/dev/null
git tag -d 1.0.1 >/dev/null 2>&1
git branch -D side 1.0.1 >/dev/null 2>&1
git clean -fdq
echo "reset: main=$(git rev-parse --short main) master=$(git rev-parse --short master) tags=$(git tag -l | tr '\n' ' ') worktrees=$(git worktree list | wc -l)"
