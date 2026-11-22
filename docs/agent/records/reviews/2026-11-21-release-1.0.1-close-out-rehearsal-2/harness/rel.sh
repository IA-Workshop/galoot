#!/usr/bin/env bash
# rel.sh <clone> <M> <old master>: what a completed run released.
set -u
C=$1 M=$2 OM=$3
cd "$C" || exit 1
echo "--- release check (M = $M)"
echo "master parents: $(git rev-list --parents -n1 refs/heads/master)"
[ "$(git rev-parse refs/heads/master^1)" = "$OM" ] && echo "master^1 = old master: yes" || echo "master^1 = old master: NO"
[ "$(git rev-parse refs/heads/master^2)" = "$M" ] && echo "master^2 = M: yes" || echo "master^2 = M: NO ($(git rev-parse refs/heads/master^2))"
echo "trees: master $(git rev-parse refs/heads/master^{tree}), M $(git rev-parse "$M^{tree}"), main now $(git rev-parse refs/heads/main^{tree})"
git for-each-ref refs/tags/1.0.1 --format='tag 1.0.1: objecttype %(objecttype), object %(*objectname) (%(*objecttype)), message "%(contents:subject)"'
echo "merge message: $(git log -1 --format=%s refs/heads/master)"
echo "main now: $(git rev-parse refs/heads/main)"
