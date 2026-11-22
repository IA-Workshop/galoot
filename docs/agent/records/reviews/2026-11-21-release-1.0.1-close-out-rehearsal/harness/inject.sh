#!/usr/bin/env bash
# inject.sh <name>: a fresh clone, one injected fault, then the script copy
# with --dry-run and without, and the clone's state before and after.
set -uo pipefail
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal
name=$1
C=$S/clones/$name
X=$S/scripts/close-out-$name.sh
T=75b9a030b5d484b5e32bd88ca2e6e13e48717ff0
RUNS=docs/agent/records/performance/runs.tsv
id=(-c user.name=rehearsal -c user.email=rehearsal@example.invalid)
"$S/bin/mk.sh" "$name" || exit 1
cd "$C" || exit 1
commit() { git "${id[@]}" commit -q -am "rehearsal: $1"; }

echo "=== injection: $name"
case $name in
  not-on-main) git switch -q -c side main ;;
  detached-head) git switch -q --detach main ;;
  uncommitted) echo "rehearsal" >> AGENTS.md ;;
  staged-only) echo "rehearsal" >> AGENTS.md; git add AGENTS.md ;;
  untracked) echo "rehearsal" > rehearsal-untracked.txt ;;
  tag-exists) git "${id[@]}" tag 1.0.1 main ;;
  outside-records) echo "// rehearsal" >> include/galoot/core.hpp; commit "outside docs/agent" ;;
  governance) echo "rehearsal" >> docs/agent/governance/WORKFLOW.md; commit "governance" ;;
  main-at-T-parent) git reset -q --hard "$T^" ;;
  matrix-row-removed)
    awk -F'\t' 'BEGIN{OFS=FS} $6=="gate:all@578aa15" && !done {done=1; next} 1' $RUNS > $S/x.$$ \
      && cat $S/x.$$ > $RUNS && rm -f $S/x.$$; commit "remove one matrix row" ;;
  matrix-row-failed)
    awk -F'\t' 'BEGIN{OFS=FS} $6=="gate:all@578aa15" && !done {done=1; $8=1} 1' $RUNS > $S/x.$$ \
      && cat $S/x.$$ > $RUNS && rm -f $S/x.$$; commit "one matrix row exit 1" ;;
  readme-status-committed) sed -i 's/This is the 1.0.1 release/This is the 1.0.2 release/' README.md; commit "README status" ;;
  readme-url-committed) sed -i 's|GIT_REPOSITORY "https://github.com/IA-Workshop/galoot.git"|GIT_REPOSITORY "https://example.invalid/galoot.git"|' README.md; commit "README URL" ;;
  readme-tag-committed) sed -i 's/^  GIT_TAG "1.0.1"$/  GIT_TAG "1.0.0"/' README.md; commit "README tag" ;;
  changelog-committed) sed -i 's/^## 1.0.1 (2026-11-21)$/## Unreleased/' CHANGELOG.md; commit "CHANGELOG" ;;
  cmake-committed) sed -i 's/project(galoot VERSION 1.0.1 LANGUAGES CXX)/project(galoot VERSION 1.0.2 LANGUAGES CXX)/' CMakeLists.txt; commit "CMake version" ;;
  # The same alterations with the file hidden from `git status`, so that the
  # specific check of step 2 is the first one that can see them.
  readme-status-hidden) git update-index --assume-unchanged README.md; sed -i 's/This is the 1.0.1 release/This is the 1.0.2 release/' README.md ;;
  readme-url-hidden) git update-index --assume-unchanged README.md; sed -i 's|GIT_REPOSITORY "https://github.com/IA-Workshop/galoot.git"|GIT_REPOSITORY "https://example.invalid/galoot.git"|' README.md ;;
  readme-tag-hidden) git update-index --assume-unchanged README.md; sed -i 's/^  GIT_TAG "1.0.1"$/  GIT_TAG "1.0.0"/' README.md ;;
  readme-tag-closed-hidden) git update-index --assume-unchanged README.md; sed -i 's/^  GIT_TAG "1.0.1"$/  GIT_TAG "1.0.1")/' README.md ;;
  readme-placeholder-hidden) git update-index --assume-unchanged README.md; echo '<galoot-repository-url>' >> README.md ;;
  changelog-hidden) git update-index --assume-unchanged CHANGELOG.md; sed -i 's/^## 1.0.1 (2026-11-21)$/## Unreleased/' CHANGELOG.md ;;
  cmake-hidden) git update-index --assume-unchanged CMakeLists.txt; sed -i 's/project(galoot VERSION 1.0.1 LANGUAGES CXX)/project(galoot VERSION 1.0.2 LANGUAGES CXX)/' CMakeLists.txt ;;
  conventions-hidden) git update-index --assume-unchanged docs/agent/governance/CONVENTIONS.md; sed -i 's/^Status: version 1.18,/Status: version 1.19,/' docs/agent/governance/CONVENTIONS.md ;;
  master-moved)
    c=$(git "${id[@]}" commit-tree 'master^{tree}' -p master -m "rehearsal: master moved")
    git update-ref refs/heads/master "$c" ;;
  w-exists) mkdir "$S/$name-master" ;;
  origin-other) git remote set-url origin https://example.invalid/galoot.git ;;
  master-in-worktree) git worktree add -q "$S/$name-other-wt" master ;;
  branch-named-tag) git branch 1.0.1 main ;;
  *) echo "no such injection" >&2; exit 2 ;;
esac

"$S/bin/state.sh" "$C" > "$S/logs/state-$name-before.txt" 2>&1
echo "--- \$ close-out-$name.sh --dry-run"
"$X" --dry-run 2>&1 | grep -v '^ .*|' ; echo "exit=${PIPESTATUS[0]}"
echo "--- \$ close-out-$name.sh"
"$X" 2>&1 | grep -v '^ .*|\| create mode \| delete mode \| rename \| mode change \| files\? changed' ; echo "exit=${PIPESTATUS[0]}"
"$S/bin/state.sh" "$C" > "$S/logs/state-$name-after.txt" 2>&1
if diff -u "$S/logs/state-$name-before.txt" "$S/logs/state-$name-after.txt" > "$S/logs/state-$name-diff.txt"; then
  echo "--- clone state unchanged by both runs (refs, HEAD, status, worktrees)"
else
  echo "--- clone state CHANGED:"; cat "$S/logs/state-$name-diff.txt"
fi
[ -e "$S/$name-master" ] && echo "--- W exists after the run: $(ls -A "$S/$name-master" | wc -l) entries"
exit 0
