#!/usr/bin/env bash
# Closes out Galoot core 1.0.1 from the main checkout on `main`: checks that
# `main` holds the audited records tip T with only records after it and no
# change outside docs/agent since the release commit R, that the version
# strings and the README's FetchContent block name 1.0.1 and the repository,
# that the matrix and the closing gates passed, and that `master` has not
# moved since 1.0.0; then merges the checked commit into `master` with
# --no-ff in a temporary worktree and tags 1.0.1 on that merge commit.
# Nothing is pushed: the push and the check after it are printed at the end.
#
#   close-out-1.0.1.sh --dry-run   runs every check and stops before the merge
#   close-out-1.0.1.sh             runs every check, then merges and tags
#
# Run it while no other session is committing to this repository.
#
# If it stops during step 5, a rerun refuses, because the worktree exists or
# `master` has moved. From /home/brianm/projects/utils/math-core/galoot:
#   the merge stopped:
#     git -C /home/brianm/projects/utils/math-core/galoot-master merge --abort
#       (skip this if git says no merge is under way)
#     git worktree remove --force /home/brianm/projects/utils/math-core/galoot-master
#     then run the script again
#   the merge was made but the worktree was not removed:
#     git worktree remove --force /home/brianm/projects/utils/math-core/galoot-master
#     check that  git diff main master --stat  prints nothing, then
#     git tag -a 1.0.1 master -m "galoot core 1.0.1, CONVENTIONS 1.18"
#   only the tag failed:
#     the same git tag command
#   a comparison after the merge failed, or to undo the merge:
#     git worktree remove --force /home/brianm/projects/utils/math-core/galoot-master
#       (if it is still there)
#     git branch -f master 1.0.0
#     then find out what moved `main` before running the script again
set -euo pipefail
cd /home/brianm/projects/utils/math-core/galoot
T=75b9a030b5d484b5e32bd88ca2e6e13e48717ff0   # the records tip T
RC=578aa15bdb6a1f30c2e9de491a757eb6bb9aea8c  # the release commit R
R=578aa15             # R as the matrix's selections name it
MATRIX_ROWS=33        # rows of gate:all@R: 3 per preset, eleven presets
V=1.0.1
PREV=1.0.0
CONV_VERSION=1.18     # the CONVENTIONS version this release implements
URL=https://github.com/IA-Workshop/galoot.git
RUNS=docs/agent/records/performance/runs.tsv
W=/home/brianm/projects/utils/math-core/galoot-master

STEP5=
fail() {
  echo "close-out: $*" >&2
  [ -z "$STEP5" ] || echo "close-out: stopped in step 5; the comment at the top of $0 says how to recover" >&2
  exit 1
}
[ $# -le 1 ] || fail "usage: close-out-$V.sh [--dry-run]"
case "${1:-}" in ''|--dry-run) ;; *) fail "usage: close-out-$V.sh [--dry-run]";; esac

[ "$(git branch --show-current)" = main ] || fail "not on main"
[ -z "$(git status --short)" ] || fail "working tree not clean"
if git show-ref --verify -q "refs/tags/$V"; then fail "tag $V exists"; fi
if git show-ref --verify -q "refs/heads/$V"; then fail "a branch named $V exists"; fi
case "$RC" in "$R"*) ;; *) fail "R is not the start of RC";; esac

# Every check below reads M, the commit `main` names now, and step 5 merges
# M itself, so a commit that reaches `main` later is neither checked nor
# released.
M=$(git rev-parse --verify 'refs/heads/main^{commit}')

# 1. M holds R and T; nothing outside the records changed after R, and
#    nothing but records, and no governance, after T.
git merge-base --is-ancestor "$RC" "$T" || fail "T does not hold R"
git merge-base --is-ancestor "$T" "$M" || fail "main does not hold T"
[ -z "$(git diff --stat "$RC" "$M" -- . ':!docs/agent')" ] || fail "non-records change after R"
[ -z "$(git diff --stat "$T" "$M" -- . ':!docs/agent')" ] || fail "non-records change after T"
[ -z "$(git diff --name-only "$T" "$M" -- docs/agent/governance)" ] || fail "governance changed after T"

# 2. The version strings and the README's FetchContent block, as committed
#    at M.
cmake=$(git show "$M:CMakeLists.txt")
readme=$(git show "$M:README.md")
changelog=$(git show "$M:CHANGELOG.md")
conv=$(git show "$M:docs/agent/governance/CONVENTIONS.md")
grep -qF "project(galoot VERSION $V LANGUAGES CXX)" <<<"$cmake" || fail "CMake version"
grep -qF "This is the $V release of the core." <<<"$readme" || fail "README status line"
grep -qF "GIT_REPOSITORY \"$URL\"" <<<"$readme" || fail "README repository URL"
grep -qxF "  GIT_TAG \"$V\"" <<<"$readme" || fail "README tag"
if grep -qF '<galoot-repository-url>' <<<"$readme"; then fail "README placeholder left"; fi
awk -v h="## $V " 'index($0, h) == 1 { found = 1 } END { exit !found }' <<<"$changelog" \
  || fail "CHANGELOG.md has no entry for $V"
[ "$(git remote get-url origin)" = "$URL" ] || fail "origin is not $URL"
[ "$(git remote get-url --push origin)" = "$URL" ] || fail "origin's push URL is not $URL"
c=$(sed -n 's/^Status: version \([0-9.]*\),.*/\1/p' <<<"$conv")
[ "$c" = "$CONV_VERSION" ] || fail "CONVENTIONS is $c, not $CONV_VERSION"

# 3. The matrix at R passed, and the run-log check and the trace gate passed
#    on T's parent's tree.
#    runs.tsv columns: 2 revision, 4 step, 6 selection, 8 exit.
runs_m=$(git show "$M:$RUNS")
awk -F'\t' -v s="gate:all@$R" -v n="$MATRIX_ROWS" '
  $6 == s { rows++; if ($8 != 0) bad++ }
  END { if (rows != n || bad) { print "matrix rows " rows+0 ", failed " bad+0; exit 1 } }' \
  <<<"$runs_m" || fail "matrix rows"
P=$(git rev-parse --short=12 "$T^")
runs_t=$(git show "$T:$RUNS")
for gate in check:runlog trace:trace; do
  step=${gate%%:*}
  sel="gate:${gate##*:}@"
  awk -F'\t' -v c="$P" -v s="$step" -v q="$sel" '
    $2 == c && $4 == s && index($6, q) == 1 { rows++; if ($8 != 0) bad++ }
    END { exit !(rows >= 1 && !bad) }' <<<"$runs_t" || fail "no passing $sel row at $P"
done

# 4. master has not moved since the last release: it is the --no-ff merge
#    commit that the previous tag names, its tree is its second parent's,
#    which M holds, and no worktree has it checked out.
[ "$(git rev-parse refs/heads/master)" = "$(git rev-parse "refs/tags/$PREV^{commit}")" ] \
  || fail "master moved since $PREV"
git merge-base --is-ancestor refs/heads/master^2 "$M" || fail "main does not hold master^2"
[ -z "$(git diff refs/heads/master refs/heads/master^2)" ] || fail "master's tree is not its second parent's"
[ ! -e "$W" ] && [ ! -L "$W" ] || fail "$W exists"
if git worktree list --porcelain | grep -qx 'branch refs/heads/master'; then
  fail "master is checked out in a worktree"
fi

if [ "${1:-}" = "--dry-run" ]; then
  echo "dry run: every check passed at main = $(git rev-parse --short "$M"); nothing merged or tagged"
  exit 0
fi

# 5. Merge M and tag the merge.
STEP5=1
trap 'echo "close-out: stopped in step 5; the comment at the top of $0 says how to recover" >&2' ERR
git worktree add "$W" master
git -C "$W" merge --no-ff --no-stat "$M" -m "Release galoot core $V"
git worktree remove "$W"
[ "$(git rev-parse refs/heads/master^2)" = "$M" ] || fail "master's second parent is not the checked commit"
[ -z "$(git diff "$M" refs/heads/master --stat)" ] || fail "master's tree differs from the checked commit's"
git tag -a "$V" refs/heads/master -m "galoot core $V, CONVENTIONS $CONV_VERSION"
trap - ERR
STEP5=
echo "done: master is at $(git rev-parse --short refs/heads/master), tag $V -> $(git rev-parse --short "refs/tags/$V^{commit}"). Nothing was pushed."
if [ "$(git rev-parse 'refs/heads/main^{commit}')" != "$M" ]; then
  echo "note: main moved during the run. The release is the checked commit $(git rev-parse --short "$M"); main now holds more, which the push below would also send."
fi
echo "To publish:   git push --atomic origin main master $PREV $V"
echo "Then check:   git -c credential.helper= ls-remote $URL refs/heads/master refs/tags/$PREV refs/tags/$V"
