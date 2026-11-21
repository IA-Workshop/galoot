#!/usr/bin/env bash
# W10 rows of plan 0044 section 7 item 6, under the shared timing lock.
# The control ran this script with "--before 41d8029", the out directory
# $S/w10-control and the lock owner "... W10 control ...".
set -u
wt=/home/brianm/projects/utils/math-core/galoot-wt/header-hygiene
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/gate
pe=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common
D=docs/agent/records/gates/2026-11-21-header-hygiene-1-0-1-gate
cd "$wt" || exit 1
cond() {
  { date -u +%FT%TZ; uptime; tools/jobpool status; free -m;
    top -b -n 1 -w 200 | sed -n 1,25p; } > "$S/out/conditions-w10-$1.txt" 2>&1
}
LOCK_OWNER="galoot header-hygiene-1-0-1 gate W10 rows $(date -u +%FT%TZ)" \
  bash $pe/lock.sh take 3600 || { echo "lock not taken"; exit 1; }
echo "$(date -u +%FT%TZ) lock taken; pool: $(tools/jobpool status | head -1)"
cond start
tools/compiles cost $D/units.toml $S/w10 --before c95fee5 --cpu 4 --repeats 3
echo "compiles exit $?"
cond end
bash $pe/lock.sh release
echo "$(date -u +%FT%TZ) lock released; pool: $(tools/jobpool status | head -1)"
