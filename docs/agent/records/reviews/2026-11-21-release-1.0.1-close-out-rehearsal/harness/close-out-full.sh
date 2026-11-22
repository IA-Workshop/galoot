#!/usr/bin/env bash
# Closes out Galoot core 1.0.1 from the main checkout on `main`: checks that
# `main` holds the audited records tip T with only records after it, that the
# version strings and the README's FetchContent block name 1.0.1 and the
# repository, that the matrix and the closing gates passed, and that `master`
# has not moved since 1.0.0; then merges `main` into `master` with --no-ff in
# a temporary worktree and tags 1.0.1 on that merge commit.
# Nothing is pushed: the push and the check after it are printed at the end.
# `close-out-1.0.1.sh --dry-run` runs every check and stops before the merge.
set -euo pipefail
cd /tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal/clones/full
T=75b9a030b5d484b5e32bd88ca2e6e13e48717ff0  # the records tip T
R=578aa15             # the release commit R, as the matrix selection names it
MATRIX_ROWS=33        # rows of gate:all@R: 3 per preset, eleven presets
V=1.0.1
PREV=1.0.0
CONV_VERSION=1.18     # the CONVENTIONS version this release implements
URL=https://github.com/IA-Workshop/galoot.git
RUNS=docs/agent/records/performance/runs.tsv
W=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal/full-master

fail() { echo "close-out: $*" >&2; exit 1; }
[ "$(git branch --show-current)" = main ] || fail "not on main"
[ -z "$(git status --short)" ] || fail "working tree not clean"
if git rev-parse -q --verify "refs/tags/$V" >/dev/null; then fail "tag $V exists"; fi

# 1. main holds T, and nothing but records changed after it.
git merge-base --is-ancestor "$T" main || fail "main does not hold T"
[ -z "$(git diff --stat "$T" main -- . ':!docs/agent')" ] || fail "non-records change after T"
[ -z "$(git diff --name-only "$T" main -- docs/agent/governance)" ] || fail "governance changed after T"

# 2. The version strings and the README's FetchContent block at main.
grep -qF "project(galoot VERSION $V LANGUAGES CXX)" CMakeLists.txt || fail "CMake version"
grep -qF "This is the $V release of the core." README.md || fail "README status line"
grep -qF "GIT_REPOSITORY \"$URL\"" README.md || fail "README repository URL"
grep -qxF "  GIT_TAG \"$V\"" README.md || fail "README tag"
if grep -qF '<galoot-repository-url>' README.md; then fail "README placeholder left"; fi
grep -qF "## $V " CHANGELOG.md || fail "CHANGELOG.md has no entry for $V"
[ "$(git remote get-url origin)" = "$URL" ] || fail "origin is not $URL"
c=$(sed -n 's/^Status: version \([0-9.]*\),.*/\1/p' docs/agent/governance/CONVENTIONS.md)
[ "$c" = "$CONV_VERSION" ] || fail "CONVENTIONS is $c, not $CONV_VERSION"

# 3. The matrix at R passed, and the closing gates ran on T's parent's tree.
#    runs.tsv columns: 2 revision, 4 step, 6 selection, 8 exit.
git show "main:$RUNS" | awk -F'\t' -v s="gate:all@$R" -v n="$MATRIX_ROWS" '
  $6 == s { rows++; if ($8 != 0) bad++ }
  END { if (rows != n || bad) { print "matrix rows " rows+0 ", failed " bad+0; exit 1 } }' \
  || fail "matrix rows"
P=$(git rev-parse --short=12 "$T^")
for step in check trace; do
  n=$(git show "$T:$RUNS" | awk -F'\t' -v c="$P" -v s="$step" '$2 == c && $4 == s && $8 == 0' | wc -l)
  [ "$n" -ge 1 ] || fail "no passing $step row at $P"
done

# 4. master has not moved since the last release: it is the --no-ff merge
#    commit that the previous tag names, and its tree is its second parent's,
#    which main holds.
[ "$(git rev-parse master)" = "$(git rev-parse "$PREV^{commit}")" ] || fail "master moved since $PREV"
git merge-base --is-ancestor master^2 main || fail "main does not hold master^2"
[ -z "$(git diff master master^2)" ] || fail "master's tree is not its second parent's"
[ ! -e "$W" ] || fail "$W exists"

if [ "${1:-}" = "--dry-run" ]; then echo "dry run: every check passed; nothing merged or tagged"; exit 0; fi

# 5. Merge and tag.
git worktree add "$W" master
git -C "$W" merge --no-ff main -m "Release galoot core $V"
git worktree remove "$W"
[ -z "$(git diff main master --stat)" ] || fail "master's tree differs from main's"
git tag -a "$V" master -m "galoot core $V, CONVENTIONS $CONV_VERSION"
echo "done: master is at $(git rev-parse --short master), tag $V -> $(git rev-parse --short "$V^{commit}"). Nothing was pushed."
echo "To publish:   git push origin main master $PREV $V"
echo "Then check:   git -c credential.helper= ls-remote $URL refs/heads/master refs/tags/$PREV refs/tags/$V"
