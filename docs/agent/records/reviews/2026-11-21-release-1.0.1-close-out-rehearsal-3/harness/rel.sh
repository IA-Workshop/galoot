#!/usr/bin/env bash
# rel.sh <clone> <M> <old master>: what a completed run released.
set -u
C=$1 M=$2 OM=$3
cd "$C" || exit 1
echo "--- release check (M = $M)"
echo "\$ git rev-list --parents -n1 master: $(git rev-list --parents -n1 refs/heads/master)"
[ "$(git rev-parse refs/heads/master^1)" = "$OM" ] && echo "master^1 = 1.0.0's commit: yes" || echo "master^1 = 1.0.0's commit: NO"
[ "$(git rev-parse refs/heads/master^2)" = "$M" ] && echo "master^2 = M: yes" || echo "master^2 = M: NO ($(git rev-parse refs/heads/master^2))"
echo "parent count: $(git cat-file -p refs/heads/master | grep -c '^parent ')"
echo "trees: master $(git rev-parse refs/heads/master^{tree}), M $(git rev-parse "$M^{tree}")"
git diff --quiet "$M" refs/heads/master && echo "git diff --quiet M master: exit 0 (trees equal)" || echo "git diff M master: DIFFERS"
if git show-ref -q --verify refs/tags/1.0.1; then
  git for-each-ref refs/tags/1.0.1 --format='tag 1.0.1: objecttype %(objecttype), object %(*objectname) (%(*objecttype)), tagger %(taggername), message "%(contents:subject)"'
  [ "$(git rev-parse 'refs/tags/1.0.1^{commit}')" = "$(git rev-parse refs/heads/master)" ] && echo "tag 1.0.1 names master: yes" || echo "tag 1.0.1 names master: NO"
else echo "no tag 1.0.1"; fi
echo "merge message: $(git log -1 --format=%s refs/heads/master)"
echo "main now: $(git rev-parse refs/heads/main)"
