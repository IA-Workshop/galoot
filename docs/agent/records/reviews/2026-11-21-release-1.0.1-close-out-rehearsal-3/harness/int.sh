#!/usr/bin/env bash
# int.sh <kind>: kind is int-pid, term-pid or int-group. A fresh clone whose
# pre-merge-commit hook (in W only) writes a marker and sleeps 6 s; the script
# copy started as a job of its own (set -m, so SIGINT is not ignored in it);
# once the marker exists, the signal goes to the script's process alone
# (int-pid, term-pid: kill -s INT|TERM <pid>) or to its whole process group,
# as a terminal's Ctrl-C would (int-group: kill -s INT -- -<pgid>). Then the
# printed lines, the exit status, the state left, and the matching recovery.
set -uo pipefail
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-3
kind=$1 name=$1
C=$S/clones/$name X=$S/scripts/close-out-$name.sh W=$S/$name-master MK=$S/$name.marker
export GIT_AUTHOR_NAME=rehearsal GIT_AUTHOR_EMAIL=rehearsal@example.invalid
export GIT_COMMITTER_NAME=rehearsal GIT_COMMITTER_EMAIL=rehearsal@example.invalid
"$S/bin/mk.sh" "$name" || exit 1
cd "$C" || exit 1
M0=$(git rev-parse refs/heads/main) OM=$(git rev-parse refs/heads/master)
cat > .git/hooks/pre-merge-commit <<H
#!/usr/bin/env bash
[ "\$(git rev-parse --show-toplevel)" = "$W" ] || exit 0
echo "rehearsal: pre-merge-commit hook sleeping 6 s"; touch "$MK"; sleep 6; echo "rehearsal: pre-merge-commit hook done"
H
chmod +x .git/hooks/pre-merge-commit; echo "(hook pre-merge-commit installed: marker, then sleep 6)"
r() { echo "\$ $*"; "$@" 2>&1; echo "exit=$?"; }
"$S/bin/state.sh" "$C" > "$S/logs/state-$name-before.txt" 2>&1
echo "--- \$ close-out-$name.sh --dry-run"; "$X" --dry-run 2>&1; echo "exit=$?"
case $kind in int-pid) sig=INT target=pid;; term-pid) sig=TERM target=pid;; int-group) sig=INT target=group;; esac
set -m
"$X" > "$S/logs/$name.out" 2>&1 &
pid=$!
set +m
for i in $(seq 1 200); do [ -e "$MK" ] && break; sleep 0.05; done
[ -e "$MK" ] || { echo "marker never appeared"; }
echo "--- \$ close-out-$name.sh   (pid $pid, pgid $(ps -o pgid= -p $pid | tr -d ' '))"
echo "script process: $(ps -o pid=,pgid=,comm= -p $pid); SigIgn $(grep SigIgn /proc/$pid/status | cut -f2) (bit 2 = SIGINT ignored: $(( 0x$(grep SigIgn /proc/$pid/status | cut -f2) & 2 ? 1 : 0 ))), SigCgt $(grep SigCgt /proc/$pid/status | cut -f2) (INT caught: $(( 0x$(grep SigCgt /proc/$pid/status | cut -f2) & 2 ? 1 : 0 )), TERM caught: $(( 0x$(grep SigCgt /proc/$pid/status | cut -f2) & 0x4000 ? 1 : 0 )))"
echo "process group at the signal (pid pgid ppid command):"; ps -eo pid=,pgid=,ppid=,args= | awk -v g=$pid '$2 == g' | sed 's/^/  /'
t0=$(date +%s.%N)
if [ $target = pid ]; then echo "(sending SIG$sig to pid $pid only)"; kill -s $sig $pid
else echo "(sending SIG$sig to process group $pid)"; kill -s $sig -- -$pid; fi
wait $pid; e=$?
echo "(the script ended $(echo "$(date +%s.%N) - $t0" | bc | cut -c1-4) s after the signal)"
echo "--- printed by the script (stdout and stderr):"; cat "$S/logs/$name.out"
echo "exit=$e"
"$S/bin/state.sh" "$C" > "$S/logs/state-$name-after.txt" 2>&1
echo "--- state after the run (diff against before)"; diff "$S/logs/state-$name-before.txt" "$S/logs/state-$name-after.txt"
echo "master parents: $(git rev-list --parents -n1 refs/heads/master)"
if [ -e "$W" ]; then echo "--- W is present; git -C W status:"; git -C "$W" status 2>&1 | head -4; else echo "--- W is absent"; fi
echo "--- \$ close-out-$name.sh --dry-run   (a rerun before the recovery)"; "$X" --dry-run 2>&1; echo "exit=$?"
rm -f .git/hooks/pre-merge-commit; echo "(the hook removed)"
echo "--- recovery, from the header comment, paths adapted"
if [ "$(git rev-parse refs/heads/master)" = "$OM" ]; then
  echo "(entry: the merge stopped)"
  r git -C "$W" merge --abort
  r git worktree remove --force "$W"
  "$S/bin/state.sh" "$C" > "$S/logs/state-$name-recovered.txt" 2>&1
  diff -q "$S/logs/state-$name-before.txt" "$S/logs/state-$name-recovered.txt" >/dev/null && echo "--- the clone is back to its state before the run" || { echo "--- differs:"; diff "$S/logs/state-$name-before.txt" "$S/logs/state-$name-recovered.txt"; }
  r "$X" --dry-run; r "$X"
else
  echo "(entry: the merge was made but the worktree was not removed)"
  r git worktree remove --force "$W"
  r git rev-list --parents -n1 master
  echo "\$ git diff master^2 master --stat   (must print nothing)"; out=$(git diff 'master^2' master --stat 2>&1); printf '%s' "$out"; echo "[${#out} bytes] exit=$?"
  r git tag -a 1.0.1 master -m "galoot core 1.0.1, CONVENTIONS 1.18"
  r "$X" --dry-run
fi
"$S/bin/rel.sh" "$C" "$M0" "$OM"
[ -e "$W" ] && echo "W still present" || echo "W absent"
exit 0
