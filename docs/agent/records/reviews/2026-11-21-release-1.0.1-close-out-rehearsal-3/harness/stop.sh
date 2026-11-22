#!/usr/bin/env bash
# stop.sh <kind>: a fresh clone, a fault that stops step 5 (or, for "undo",
# a completed run), the run, the state it leaves, a rerun, then the recovery
# entry of the script's header comment that names that state, typed as
# written with only the paths adapted (cwd = the clone, W = the scratch W),
# and the rerun, or the tag, publish and check, that follows it.
set -uo pipefail
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-3
kind=$1 name=stop-$1
C=$S/clones/$name X=$S/scripts/close-out-$name.sh W=$S/$name-master
export GIT_AUTHOR_NAME=rehearsal GIT_AUTHOR_EMAIL=rehearsal@example.invalid
export GIT_COMMITTER_NAME=rehearsal GIT_COMMITTER_EMAIL=rehearsal@example.invalid
"$S/bin/mk.sh" "$name" || exit 1
cd "$C" || exit 1
M0=$(git rev-parse refs/heads/main) OM=$(git rev-parse refs/heads/master)
hook() { cat > ".git/hooks/$1"; chmod +x ".git/hooks/$1"; echo "(hook $1 installed in the clone)"; }
gate="[ \"\$(git rev-parse --show-toplevel)\" = \"$W\" ] || exit 0"
r() { echo "\$ $*"; "$@" 2>&1; echo "exit=$?"; }
snap() { "$S/bin/state.sh" "$C" > "$S/logs/state-$name-$1.txt" 2>&1; }
back() {  # is the clone back to its state before the run?
  snap recovered
  if diff -q "$S/logs/state-$name-before.txt" "$S/logs/state-$name-recovered.txt" >/dev/null; then echo "--- the clone is back to its state before the run"
  else echo "--- after the recovery, the clone differs from before:"; diff "$S/logs/state-$name-before.txt" "$S/logs/state-$name-recovered.txt"; fi
}
publish() {  # the publish and check lines, master^2 in place of the commit, origin -> scratch
  "$S/bin/bare.sh" "$C" "$name"
  echo "--- publish, as the file's last line prints it, master^2 for the commit, origin -> scratch"
  r git push --atomic scratch 'master^2:refs/heads/main' refs/heads/master refs/tags/1.0.0 refs/tags/1.0.1
  echo "--- check, as the file's last line prints it, the URL -> the bare repository"
  r git -c credential.helper= ls-remote "$S/remotes/$name.git" refs/heads/main refs/heads/master refs/tags/1.0.0 refs/tags/1.0.1
  echo "bare main = M: $([ "$(git -C "$S/remotes/$name.git" rev-parse refs/heads/main)" = "$M0" ] && echo yes || echo NO)"
  echo "bare master = local master: $([ "$(git -C "$S/remotes/$name.git" rev-parse refs/heads/master)" = "$(git rev-parse refs/heads/master)" ] && echo yes || echo NO)"
  echo "bare 1.0.1 = local tag object: $([ "$(git -C "$S/remotes/$name.git" rev-parse refs/tags/1.0.1)" = "$(git rev-parse refs/tags/1.0.1)" ] && echo yes || echo NO)"
}
echo "=== step 5: $kind"
case $kind in
  merge) hook pre-merge-commit <<H
#!/usr/bin/env bash
$gate
echo "rehearsal: pre-merge-commit hook refuses"; exit 1
H
  ;;
  remove) hook post-merge <<H
#!/usr/bin/env bash
$gate
echo rehearsal > rehearsal-untracked.txt; echo "rehearsal: post-merge hook left an untracked file in W"
H
  ;;
  tag) git config tag.gpgSign true; git config gpg.program false; echo "(clone config: tag.gpgSign=true, gpg.program=false)" ;;
  parent-m) hook post-merge <<H
#!/usr/bin/env bash
$gate
c=\$(git commit-tree HEAD^{tree} -p HEAD -p HEAD^2 -m "rehearsal: a second merge of M on master"); git update-ref HEAD \$c
echo "rehearsal: post-merge hook added a commit \$c on master with parents (the merge, M)"
H
  ;;
  tree) hook post-merge <<H
#!/usr/bin/env bash
$gate
export GIT_INDEX_FILE=$S/idx.$name; git read-tree HEAD
b=\$(echo "// rehearsal" | git hash-object -w --stdin); git update-index --add --cacheinfo 100644,\$b,include/galoot/rehearsal_extra.hpp
t=\$(git write-tree); rm -f $S/idx.$name; unset GIT_INDEX_FILE
c=\$(git commit-tree \$t -p HEAD^1 -p HEAD^2 -m "Release galoot core 1.0.1"); git update-ref HEAD \$c; git reset -q --hard HEAD
echo "rehearsal: post-merge hook replaced the merge by \$c, same parents, one more file"
H
  ;;
  undo) echo "(no fault: a completed run, then the undo entry)" ;;
esac
snap before
echo "--- \$ close-out-$name.sh --dry-run"; "$X" --dry-run 2>&1; echo "exit=$?"
echo "--- \$ close-out-$name.sh"; "$X" 2>&1; echo "exit=$?"
snap after
echo "--- state after the run (diff against before)"; diff "$S/logs/state-$name-before.txt" "$S/logs/state-$name-after.txt"
echo "master parents: $(git rev-list --parents -n1 refs/heads/master)"
if [ -e "$W" ]; then echo "--- W is present; git -C W status:"; git -C "$W" status 2>&1 | head -6; else echo "--- W is absent"; fi
echo "--- \$ close-out-$name.sh --dry-run   (a rerun before the recovery)"; "$X" --dry-run 2>&1; echo "exit=$?"
echo "--- recovery, from the header comment, paths adapted"
case $kind in
  merge)
    echo "(entry: the merge stopped)"
    r git -C "$W" merge --abort
    r git worktree remove --force "$W"
    rm .git/hooks/pre-merge-commit; echo "(cause fixed: the hook removed)"
    back
    echo "(then run the script again)"; r "$X" --dry-run; r "$X" ;;
  remove|tag)
    if [ $kind = remove ]; then
      echo "(entry: the merge was made but the worktree was not removed)"
      r git worktree remove --force "$W"
      rm .git/hooks/post-merge; echo "(the hook removed)"
    else
      echo "(entry: only the tag failed: the same checks, the same git tag command, the same publish and check)"
      git config --unset tag.gpgSign; git config --unset gpg.program; echo "(cause fixed: the clone's signing configuration removed)"
    fi
    r git rev-list --parents -n1 master
    echo "(must print: the new commit, then 2383e6d..., then the commit the dry run named as main, e6320526 above)"
    set -- $(git rev-list --parents -n1 master)
    [ $# = 3 ] && [ "$2" = "$OM" ] && [ "${3:0:8}" = e6320526 ] && echo "(it does)" || echo "(it does NOT)"
    echo "\$ git diff master^2 master --stat   (must print nothing)"; out=$(git diff 'master^2' master --stat 2>&1); e=$?; printf '%s' "$out"; echo "[${#out} bytes] exit=$e"
    r git tag -a 1.0.1 master -m "galoot core 1.0.1, CONVENTIONS 1.18"
    publish
    r "$X" --dry-run ;;
  parent-m|tree|undo)
    echo "(entry: a comparison after the merge failed, or to undo the merge)"
    if [ -e "$W" ]; then r git worktree remove --force "$W"; else echo "(W is not there; its removal is skipped, as the comment allows)"; fi
    if git show-ref -q --verify refs/tags/1.0.1; then r git tag -d 1.0.1; else echo "(no tag 1.0.1; its deletion is skipped, as the comment allows)"; fi
    r git branch -f master 1.0.0
    echo "master is now $(git rev-parse master) ($(git cat-file -t master))"
    rm -f .git/hooks/post-merge; echo "(cause fixed: any hook removed)"
    back
    r "$X" --dry-run; r "$X" ;;
esac
"$S/bin/rel.sh" "$C" "$M0" "$OM"
[ -e "$W" ] && echo "W still present" || echo "W absent"
exit 0
