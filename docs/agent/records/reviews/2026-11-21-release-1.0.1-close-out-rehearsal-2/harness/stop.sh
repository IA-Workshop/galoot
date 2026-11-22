#!/usr/bin/env bash
# stop.sh <kind>: a fresh clone, a fault that stops step 5 midway, the run,
# the state it leaves, a rerun, then the recovery of the script's header
# comment with only its paths adapted, and the run or tag that follows it.
set -uo pipefail
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-2
kind=$1 name=stop-$1
C=$S/clones/$name X=$S/scripts/close-out-$name.sh W=$S/$name-master
export GIT_AUTHOR_NAME=rehearsal GIT_AUTHOR_EMAIL=rehearsal@example.invalid
export GIT_COMMITTER_NAME=rehearsal GIT_COMMITTER_EMAIL=rehearsal@example.invalid
"$S/bin/mk.sh" "$name" || exit 1
cd "$C" || exit 1
M0=$(git rev-parse refs/heads/main) OM=$(git rev-parse refs/heads/master)
hook() { cat > ".git/hooks/$1"; chmod +x ".git/hooks/$1"; echo "(hook $1 installed in the clone)"; }
gate="[ \"\$(git rev-parse --show-toplevel)\" = \"$W\" ] || exit 0"
run=("$X")
echo "=== step 5 stop: $kind"
case $kind in
  merge) hook pre-merge-commit <<H
#!/usr/bin/env bash
$gate
echo "rehearsal: pre-merge-commit hook refuses"; exit 1
H
  ;;
  identity) run=(env -u GIT_AUTHOR_NAME -u GIT_AUTHOR_EMAIL -u GIT_COMMITTER_NAME -u GIT_COMMITTER_EMAIL -u EMAIL GIT_CONFIG_GLOBAL=/dev/null GIT_CONFIG_NOSYSTEM=1 "$X")
    echo "(the run without any user identity: no identity variables, no global or system configuration)" ;;
  remove) hook post-merge <<H
#!/usr/bin/env bash
$gate
echo rehearsal > rehearsal-untracked.txt; echo "rehearsal: post-merge hook left an untracked file in W"
H
  ;;
  tree) hook post-merge <<H
#!/usr/bin/env bash
$gate
# The merge commit replaced, by plumbing, by one with the same parents and one
# more file; W's index and files brought to it, so the worktree removal passes.
export GIT_INDEX_FILE=$S/idx.$name; git read-tree HEAD
b=\$(echo "// rehearsal" | git hash-object -w --stdin); git update-index --add --cacheinfo 100644,\$b,include/galoot/rehearsal_extra.hpp
t=\$(git write-tree); rm -f $S/idx.$name; unset GIT_INDEX_FILE
c=\$(git commit-tree \$t -p HEAD^1 -p HEAD^2 -m "Release galoot core 1.0.1"); git update-ref HEAD \$c; git reset -q --hard HEAD
echo "rehearsal: post-merge hook replaced the merge by \$c, same parents, one more file"
H
  ;;
  parent) hook post-merge <<H
#!/usr/bin/env bash
$gate
c=\$(git commit-tree HEAD^{tree} -p HEAD -m "rehearsal: a commit on master after the merge"); git update-ref HEAD \$c
echo "rehearsal: post-merge hook added a one-parent commit \$c on master"
H
  ;;
  parent-m) hook post-merge <<H
#!/usr/bin/env bash
$gate
c=\$(git commit-tree HEAD^{tree} -p HEAD -p HEAD^2 -m "rehearsal: a second merge of M on master"); git update-ref HEAD \$c
echo "rehearsal: post-merge hook added a commit \$c on master with parents (the merge, M)"
H
  ;;
  tag) git config tag.gpgSign true; git config gpg.program false; echo "(clone config: tag.gpgSign=true, gpg.program=false)" ;;
esac
"$S/bin/state.sh" "$C" > "$S/logs/state-$name-before.txt" 2>&1
echo "--- \$ close-out-$name.sh --dry-run"; "${run[@]}" --dry-run 2>&1; echo "exit=$?"
echo "--- \$ close-out-$name.sh"; "${run[@]}" 2>&1 | sed 's/(got .*)/(got (withheld))/'; echo "exit=${PIPESTATUS[0]}"
echo "--- state after the run (diff against before)"
"$S/bin/state.sh" "$C" > "$S/logs/state-$name-after.txt" 2>&1
diff "$S/logs/state-$name-before.txt" "$S/logs/state-$name-after.txt"
echo "master parents: $(git rev-list --parents -n1 refs/heads/master)"
if [ -e "$W" ]; then echo "--- W is present; git -C W status:"; git -C "$W" status 2>&1 | head -6; ls "$C/.git/worktrees/"*/ 2>/dev/null | tr '\n' ' '; echo; else echo "--- W is absent"; fi
echo "--- \$ close-out-$name.sh --dry-run   (a rerun before the recovery)"; "$X" --dry-run 2>&1; echo "exit=$?"
echo "--- recovery, from the header comment, paths adapted (cwd = the clone, W = $W)"
r() { echo "\$ $*"; "$@" 2>&1; echo "exit=$?"; }
case $kind in
  merge|identity)
    r git -C "$W" merge --abort
    r git worktree remove --force "$W"
    case $kind in merge) rm .git/hooks/pre-merge-commit; echo "(cause fixed: the hook removed)";; identity) echo "(cause fixed: the run has an identity again)";; esac
    "$S/bin/state.sh" "$C" > "$S/logs/state-$name-recovered.txt" 2>&1
    diff -q "$S/logs/state-$name-before.txt" "$S/logs/state-$name-recovered.txt" >/dev/null && echo "--- the clone is back to its state before the run" || { echo "--- after the recovery, the clone differs from before:"; diff "$S/logs/state-$name-before.txt" "$S/logs/state-$name-recovered.txt"; }
    r "$X" --dry-run; r "$X" ;;
  remove)
    r git worktree remove --force "$W"
    echo "\$ git diff main master --stat   (must print nothing)"; git diff main master --stat; echo "exit=$?"
    rm .git/hooks/post-merge; echo "(the hook removed)"
    r git tag -a 1.0.1 master -m "galoot core 1.0.1, CONVENTIONS 1.18"
    r "$X" --dry-run ;;
  tag)
    git config --unset tag.gpgSign; git config --unset gpg.program; echo "(cause fixed: the clone's signing configuration removed)"
    echo "\$ git diff main master --stat   (must print nothing)"; git diff main master --stat; echo "exit=$?"
    r git tag -a 1.0.1 master -m "galoot core 1.0.1, CONVENTIONS 1.18"
    r "$X" --dry-run ;;
  parent-m) echo "(no stop is expected: no recovery)" ;;
  tree|parent)
    if [ -e "$W" ]; then r git worktree remove --force "$W"; else echo "(W is not there; the remove is skipped, as the comment allows)"; fi
    r git branch -f master 1.0.0
    echo "\$ git log --format='%h %s' $OM..main   (what moved main: nothing)"; git log --format='%h %s' "$OM..main" | head -3 | grep -v "^$" | tail -0; git rev-parse main
    rm .git/hooks/post-merge; echo "(cause fixed: the hook removed)"
    "$S/bin/state.sh" "$C" > "$S/logs/state-$name-recovered.txt" 2>&1
    diff -q "$S/logs/state-$name-before.txt" "$S/logs/state-$name-recovered.txt" >/dev/null && echo "--- the clone is back to its state before the run" || { echo "--- after the recovery, the clone differs from before:"; diff "$S/logs/state-$name-before.txt" "$S/logs/state-$name-recovered.txt"; }
    r "$X" --dry-run; r "$X" ;;
esac
"$S/bin/rel.sh" "$C" "$M0" "$OM"
git diff --quiet "$M0" refs/heads/master && echo "master's tree is M's: yes" || echo "master's tree is M's: NO"
[ -e "$W" ] && echo "W still present" || echo "W absent"; git worktree list --porcelain | grep -c '^worktree ' | sed 's/^/worktrees: /'
exit 0
