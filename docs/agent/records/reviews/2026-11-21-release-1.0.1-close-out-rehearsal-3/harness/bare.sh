#!/usr/bin/env bash
# bare.sh <clone> <name>: a bare repository in scratch holding what the real
# remote holds today (main c95fee51, master 2383e6df, tag 1.0.0 95a088ec),
# built by pushing those refs from the clone under the remote name "scratch".
set -euo pipefail
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-3
C=$1 B=$S/remotes/$2.git
[ ! -e "$B" ] || { echo "bare: $B exists" >&2; exit 1; }
git init -q --bare "$B"
git -C "$C" remote add scratch "$B"
echo "\$ git push scratch c95fee51...:refs/heads/main 2383e6df...:refs/heads/master refs/tags/1.0.0:refs/tags/1.0.0"
git -C "$C" push scratch c95fee51248839a81c7ab28e7de610376bcba603:refs/heads/main \
  2383e6df5e9dd9e79832a8dbb3d8af9207a5913f:refs/heads/master refs/tags/1.0.0:refs/tags/1.0.0 2>&1
echo "\$ git ls-remote $B   (before the printed push)"
git ls-remote "$B"
