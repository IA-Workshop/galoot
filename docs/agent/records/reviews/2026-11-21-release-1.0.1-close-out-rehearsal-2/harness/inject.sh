#!/usr/bin/env bash
# inject.sh <name>: a fresh clone, one injected fault, then the script copy
# with --dry-run and without, the clone's state before and after, and, if the
# run released, what it released.
set -uo pipefail
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-2
name=$1
C=$S/clones/$name
X=$S/scripts/close-out-$name.sh
XO=$S/scripts/close-out-old-$name.sh
W=$S/$name-master
T=75b9a030b5d484b5e32bd88ca2e6e13e48717ff0
RC=578aa15bdb6a1f30c2e9de491a757eb6bb9aea8c
RUNS=docs/agent/records/performance/runs.tsv
# Every commit and tag in the clones, the script's own included, carries this
# identity, so that no log holds the owner's.
export GIT_AUTHOR_NAME=rehearsal GIT_AUTHOR_EMAIL=rehearsal@example.invalid
export GIT_COMMITTER_NAME=rehearsal GIT_COMMITTER_EMAIL=rehearsal@example.invalid
old=; case $name in runlog-failed-beside-pass|runlog-row-missing|r-between) old=old ;; esac
"$S/bin/mk.sh" "$name" $old || exit 1
cd "$C" || exit 1
commit() { git commit -q -am "rehearsal: $1"; }
# retree <commit> <change...>: replace <commit>, by `git replace`, with a commit
# that differs only in its tree; <change> edits a scratch index read from it.
retree() {
  local c=$1; shift
  export GIT_INDEX_FILE=$S/idx.$name
  git read-tree "$c"; "$@"
  local tree new; tree=$(git write-tree); unset GIT_INDEX_FILE; rm -f "$S/idx.$name"
  new=$(git cat-file commit "$c" | sed "1s/^tree .*/tree $tree/" | git hash-object -t commit -w --stdin)
  git replace "$c" "$new"
  echo "replaced $(git rev-parse --short "$c") (tree $(git rev-parse --short "$c^{tree}") now) by $(git rev-parse --short "$new")"
}
runs_edit() {  # runs_edit <commit> <awk program>: the run log of <commit>, edited, into the scratch index
  local b; b=$(git show "$1:$RUNS" | awk -F'\t' 'BEGIN{OFS=FS} '"$2" | git hash-object -w --stdin)
  git update-index --cacheinfo "100644,$b,$RUNS"
}
add_file() { local b; b=$(echo "// rehearsal" | git hash-object -w --stdin); git update-index --add --cacheinfo "100644,$b,$1"; }
rm_file() { git update-index --force-remove "$1"; }
hook() { cat > ".git/hooks/$1"; chmod +x ".git/hooks/$1"; }
dryargs=(--dry-run) runargs=()

echo "=== injection: $name"
case $name in
  # The first rehearsal's injections.
  not-on-main) git switch -q -c side main ;;
  detached-head) git switch -q --detach main ;;
  uncommitted) echo "rehearsal" >> AGENTS.md ;;
  staged-only) echo "rehearsal" >> AGENTS.md; git add AGENTS.md ;;
  untracked) echo "rehearsal" > rehearsal-untracked.txt ;;
  tag-exists) git tag 1.0.1 main ;;
  outside-records) echo "// rehearsal" >> include/galoot/core.hpp; commit "outside docs/agent" ;;
  governance) echo "rehearsal" >> docs/agent/governance/WORKFLOW.md; commit "governance" ;;
  main-at-T-parent) git reset -q --hard "$T^" ;;
  matrix-row-removed) runs_tmp=$(git show "main:$RUNS" | awk -F'\t' 'BEGIN{OFS=FS} $6=="gate:all@578aa15" && !d {d=1; next} 1'); printf '%s\n' "$runs_tmp" > $RUNS; commit "remove one matrix row" ;;
  matrix-row-failed) runs_tmp=$(git show "main:$RUNS" | awk -F'\t' 'BEGIN{OFS=FS} $6=="gate:all@578aa15" && !d {d=1; $8=1} 1'); printf '%s\n' "$runs_tmp" > $RUNS; commit "one matrix row exit 1" ;;
  readme-status-committed) sed -i 's/This is the 1.0.1 release/This is the 1.0.2 release/' README.md; commit "README status" ;;
  readme-url-committed) sed -i 's|GIT_REPOSITORY "https://github.com/IA-Workshop/galoot.git"|GIT_REPOSITORY "https://example.invalid/galoot.git"|' README.md; commit "README URL" ;;
  readme-tag-committed) sed -i 's/^  GIT_TAG "1.0.1"$/  GIT_TAG "1.0.0"/' README.md; commit "README tag" ;;
  changelog-committed) sed -i 's/^## 1.0.1 (2026-11-21)$/## Unreleased/' CHANGELOG.md; commit "CHANGELOG" ;;
  cmake-committed) sed -i 's/project(galoot VERSION 1.0.1 LANGUAGES CXX)/project(galoot VERSION 1.0.2 LANGUAGES CXX)/' CMakeLists.txt; commit "CMake version" ;;
  readme-status-hidden) git update-index --assume-unchanged README.md; sed -i 's/This is the 1.0.1 release/This is the 1.0.2 release/' README.md ;;
  readme-url-hidden) git update-index --assume-unchanged README.md; sed -i 's|GIT_REPOSITORY "https://github.com/IA-Workshop/galoot.git"|GIT_REPOSITORY "https://example.invalid/galoot.git"|' README.md ;;
  readme-tag-hidden) git update-index --assume-unchanged README.md; sed -i 's/^  GIT_TAG "1.0.1"$/  GIT_TAG "1.0.0"/' README.md ;;
  readme-tag-closed-hidden) git update-index --assume-unchanged README.md; sed -i 's/^  GIT_TAG "1.0.1"$/  GIT_TAG "1.0.1")/' README.md ;;
  readme-placeholder-hidden) git update-index --assume-unchanged README.md; echo '<galoot-repository-url>' >> README.md ;;
  changelog-hidden) git update-index --assume-unchanged CHANGELOG.md; sed -i 's/^## 1.0.1 (2026-11-21)$/## Unreleased/' CHANGELOG.md ;;
  cmake-hidden) git update-index --assume-unchanged CMakeLists.txt; sed -i 's/project(galoot VERSION 1.0.1 LANGUAGES CXX)/project(galoot VERSION 1.0.2 LANGUAGES CXX)/' CMakeLists.txt ;;
  conventions-hidden) git update-index --assume-unchanged docs/agent/governance/CONVENTIONS.md; sed -i 's/^Status: version 1.18,/Status: version 1.19,/' docs/agent/governance/CONVENTIONS.md ;;
  master-moved) c=$(git commit-tree 'master^{tree}' -p master -m "rehearsal: master moved"); git update-ref refs/heads/master "$c" ;;
  w-exists) mkdir "$W" ;;
  origin-other) git remote set-url origin https://example.invalid/galoot.git ;;
  master-in-worktree) git worktree add -q "$S/$name-other-wt" master ;;
  branch-named-tag) git branch 1.0.1 main ;;
  flag-typo) dryargs=(--dryrun) runargs=(--dryrun) ;;
  race-before-merge)
    # During `git worktree add`, once, in W only: a commit outside docs/agent
    # reaches main by plumbing, as another session's commit would.
    hook post-checkout <<HOOK
#!/usr/bin/env bash
[ "\$(git rev-parse --show-toplevel)" = "$W" ] && [ ! -e "$S/race-done" ] || exit 0
touch "$S/race-done"
export GIT_INDEX_FILE=$S/idx.race
git read-tree refs/heads/main
b=\$(echo "// unchecked" | git hash-object -w --stdin)
git update-index --add --cacheinfo 100644,\$b,include/galoot/rehearsal_unchecked.hpp
t=\$(git write-tree); rm -f $S/idx.race; unset GIT_INDEX_FILE
c=\$(git commit-tree \$t -p refs/heads/main -m "rehearsal: unchecked commit outside docs/agent")
git update-ref refs/heads/main \$c
echo "rehearsal: post-checkout hook moved main to \$c"
HOOK
    ;;
  # New injections for the changed lines.
  arg-empty) dryargs=("") runargs=("") ;;
  r-between)
    # T replaced by a commit whose tree also holds a file outside docs/agent,
    # and the same file committed on main, so that T and main agree outside
    # docs/agent and only the check against R can see it.
    retree "$T" add_file include/galoot/rehearsal_between.hpp
    echo "// rehearsal" > include/galoot/rehearsal_between.hpp; git add include/galoot/rehearsal_between.hpp
    git commit -q -m "rehearsal: the file T now holds" ;;
  assume-unchanged-edit) git update-index --assume-unchanged README.md; sed -i 's/This is the 1.0.1 release/This is the 1.0.2 release/' README.md ;;
  skip-worktree-edit) git update-index --skip-worktree CMakeLists.txt; sed -i 's/VERSION 1.0.1 /VERSION 1.0.2 /' CMakeLists.txt ;;
  pushurl-other) git remote set-url --push origin https://example.invalid/galoot.git ;;
  pushurl-second) git remote set-url --add --push origin https://github.com/IA-Workshop/galoot.git
                  git remote set-url --add --push origin https://example.invalid/galoot.git ;;
  pushinsteadof) git config url.https://example.invalid/.pushInsteadOf https://github.com/ ;;
  w-dangling-link) ln -s "$S/$name-nowhere" "$W" ;;
  w-link-to-dir) mkdir "$S/$name-dir"; ln -s "$S/$name-dir" "$W" ;;
  w-file) echo rehearsal > "$W" ;;
  runlog-failed-beside-pass) retree "$T" runs_edit "$T" '$2=="334ec448a7a6" && $6=="gate:runlog@334ec44" {$8=1} 1' ;;
  runlog-row-missing) retree "$T" runs_edit "$T" '$2=="334ec448a7a6" && $6=="gate:runlog@334ec44" {next} 1' ;;
  runlog-extra-failed) retree "$T" runs_edit "$T" '{print} $2=="334ec448a7a6" && $6=="gate:runlog@334ec44" {$8=1; print}' ;;
  trace-failed) retree "$T" runs_edit "$T" '$2=="334ec448a7a6" && $6=="gate:trace@334ec44" {$8=1} 1' ;;
  matrix-row-extra) runs_tmp=$(git show "main:$RUNS" | awk -F'\t' 'BEGIN{OFS=FS} {print} $6=="gate:all@578aa15" && !d {d=1; print}'); printf '%s\n' "$runs_tmp" > $RUNS; commit "one matrix row twice" ;;
  rm-cmake) git rm -q CMakeLists.txt; git commit -q -m "rehearsal: remove CMakeLists.txt" ;;
  rm-readme) git rm -q README.md; git commit -q -m "rehearsal: remove README.md" ;;
  rm-changelog) git rm -q CHANGELOG.md; git commit -q -m "rehearsal: remove CHANGELOG.md" ;;
  rm-conventions) git rm -q docs/agent/governance/CONVENTIONS.md; git commit -q -m "rehearsal: remove CONVENTIONS.md" ;;
  show-fails-reached)
    # R and T replaced by commits whose trees lack CHANGELOG.md, and the file
    # removed on main: steps 1 and 2's earlier lines pass and git show fails.
    retree "$RC" rm_file CHANGELOG.md; retree "$T" rm_file CHANGELOG.md
    git rm -q CHANGELOG.md; git commit -q -m "rehearsal: remove CHANGELOG.md" ;;
  branch-named-prev) git branch 1.0.0 main ;;
  tag-named-master) git tag master main ;;
  *) echo "no such injection" >&2; exit 2 ;;
esac

M0=$(git rev-parse refs/heads/main) OM=$(git rev-parse refs/heads/master)
"$S/bin/state.sh" "$C" > "$S/logs/state-$name-before.txt" 2>&1
if [ -n "$old" ]; then
  echo "--- \$ close-out-old-$name.sh --dry-run   (the first version, 1655ef33, for comparison; dry run only)"
  "$XO" --dry-run 2>&1; echo "exit=$?"
fi
echo "--- \$ close-out-$name.sh ${dryargs[*]@Q}"
"$X" "${dryargs[@]}" 2>&1; echo "exit=$?"
echo "--- \$ close-out-$name.sh ${runargs[*]@Q}"
"$X" "${runargs[@]}" 2>&1; echo "exit=$?"
"$S/bin/state.sh" "$C" > "$S/logs/state-$name-after.txt" 2>&1
if diff -u "$S/logs/state-$name-before.txt" "$S/logs/state-$name-after.txt" > "$S/logs/state-$name-diff.txt"; then
  echo "--- clone state unchanged by both runs (refs, HEAD, status, worktrees)"
else
  echo "--- clone state CHANGED:"; cat "$S/logs/state-$name-diff.txt"
fi
[ "$(git rev-parse refs/heads/master)" = "$OM" ] || "$S/bin/rel.sh" "$C" "$M0" "$OM"
{ [ -e "$W" ] || [ -L "$W" ]; } && echo "--- W after the runs: $(ls -ld "$W" | cut -c1-10) $( [ -d "$W" ] && ls -A "$W" | wc -l) entries"
exit 0
