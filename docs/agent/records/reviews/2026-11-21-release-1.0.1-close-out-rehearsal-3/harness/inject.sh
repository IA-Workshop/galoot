#!/usr/bin/env bash
# inject.sh <name>: a fresh clone, one injected fault, then the script copy
# with --dry-run and without (or the argument forms of the case), the clone's
# state before and after, and, if the run released, what it released.
set -uo pipefail
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-3
name=$1
C=$S/clones/$name X=$S/scripts/close-out-$name.sh W=$S/$name-master
T=75b9a030b5d484b5e32bd88ca2e6e13e48717ff0
RC=578aa15bdb6a1f30c2e9de491a757eb6bb9aea8c
RUNS=docs/agent/records/performance/runs.tsv
export GIT_AUTHOR_NAME=rehearsal GIT_AUTHOR_EMAIL=rehearsal@example.invalid
export GIT_COMMITTER_NAME=rehearsal GIT_COMMITTER_EMAIL=rehearsal@example.invalid
"$S/bin/mk.sh" "$name" || exit 1
cd "$C" || exit 1
# retree <commit> <change...>: replace <commit>, by `git replace`, with a
# commit that differs only in its tree; <change> edits a scratch index.
retree() {
  local c=$1; shift
  export GIT_INDEX_FILE=$S/idx.$name
  git read-tree "$c"; "$@"
  local tree new; tree=$(git write-tree); unset GIT_INDEX_FILE; rm -f "$S/idx.$name"
  new=$(git cat-file commit "$c" | sed "1s/^tree .*/tree $tree/" | git hash-object -t commit -w --stdin)
  git replace "$c" "$new"
  echo "(git replace: $(git rev-parse --short "$c") now has tree $(git rev-parse --short "$c^{tree}"), via $(git rev-parse --short "$new"))"
}
rm_file() { git update-index --force-remove "$1"; }
hook() { cat > ".git/hooks/$1"; chmod +x ".git/hooks/$1"; echo "(hook $1 installed in the clone)"; }
forms=("--dry-run" "")   # each form is one word list, split on the | character below
argsets=()
echo "=== injection: $name"
case $name in
  args)
    echo "(argument forms only; no fault in the clone)"
    argsets=("''" "'--dry-run' 'x'" "'x' '--dry-run'" "'' ''" "'--dry-run' '--dry-run'" "'--dry-run' ''" "'' '--dry-run'" "'--dry-run'") ;;
  pushurl-second-wrong)
    git remote set-url --add --push origin https://github.com/IA-Workshop/galoot.git
    git remote set-url --add --push origin https://example.invalid/galoot.git ;;
  pushurl-wrong-first)
    git remote set-url --add --push origin https://example.invalid/galoot.git
    git remote set-url --add --push origin https://github.com/IA-Workshop/galoot.git ;;
  url-second)
    git config --add remote.origin.url https://example.invalid/galoot.git ;;
  master-in-worktree) git worktree add -q "$S/$name-other-wt" master; echo "(master checked out in $S/$name-other-wt)" ;;
  rm-cmake|rm-readme|rm-changelog)
    f=$(case $name in rm-cmake) echo CMakeLists.txt;; rm-readme) echo README.md;; rm-changelog) echo CHANGELOG.md;; esac)
    retree "$RC" rm_file "$f"; retree "$T" rm_file "$f"
    git rm -q "$f"; git commit -q -m "rehearsal: remove $f"; echo "(R and T replaced by trees without $f; $f removed on main)" ;;
  rm-conventions)
    f=docs/agent/governance/CONVENTIONS.md
    retree "$T" rm_file "$f"; git rm -q "$f"; git commit -q -m "rehearsal: remove CONVENTIONS.md"
    echo "(T replaced by a tree without CONVENTIONS.md; the file removed on main)" ;;
  rm-runs-at-main) git rm -q "$RUNS"; git commit -q -m "rehearsal: remove the run log"; echo "(the run log removed on main by a commit)" ;;
  rm-runs-at-T) retree "$T" rm_file "$RUNS"; echo "(T replaced by a tree without the run log; main unchanged)" ;;
  race)
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
    forms=("") ;;
  *) echo "no such injection" >&2; exit 2 ;;
esac
[ ${#argsets[@]} -gt 0 ] || argsets=("'--dry-run'" "")
M0=$(git rev-parse refs/heads/main) OM=$(git rev-parse refs/heads/master)
[ "$name" = race ] && argsets=("")
echo "M (main before the runs): $M0"
"$S/bin/state.sh" "$C" > "$S/logs/state-$name-before.txt" 2>&1
for a in "${argsets[@]}"; do
  eval "set -- $a"
  echo "--- \$ close-out-$name.sh $a   (\$# = $#)"
  "$X" "$@" 2>&1; echo "exit=$?"
  if "$S/bin/state.sh" "$C" | diff -q "$S/logs/state-$name-before.txt" - >/dev/null; then echo "(clone state unchanged)"; else echo "(clone state CHANGED)"; fi
done
"$S/bin/state.sh" "$C" > "$S/logs/state-$name-after.txt" 2>&1
if diff -u "$S/logs/state-$name-before.txt" "$S/logs/state-$name-after.txt" > "$S/logs/state-$name-diff.txt"; then
  echo "--- clone state unchanged by every run (refs, HEAD, status, worktrees)"
else
  echo "--- clone state changed:"; cat "$S/logs/state-$name-diff.txt"
fi
[ "$(git rev-parse refs/heads/master)" = "$OM" ] || "$S/bin/rel.sh" "$C" "$M0" "$OM"
exit 0
