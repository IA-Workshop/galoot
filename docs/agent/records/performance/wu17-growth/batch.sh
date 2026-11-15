#!/usr/bin/env bash
# The locked run of the growth grid, from the root of the checkout: writes
# the units from generate.py, reads the job pool and the load average, takes
# the shared timing lock, runs tools/compiles cost pinned to CPU 4 with three
# repeats, releases the lock, reads both again (appending each reading to
# <out>/protocol.log), and writes <out>/growth.tsv. <out> is out, or the
# directory named by the second argument, relative to this one.
#
# Usage: docs/agent/records/performance/wu17-growth/batch.sh <before-revision> [<out>]
set -u
here=docs/agent/records/performance/wu17-growth
lock=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common/lock.sh
out=$here/${2:-out}
mkdir -p "$out"
log=$out/protocol.log
reading() {
  echo "$1 $(date -u +%FT%TZ) pool: $(tools/jobpool status | head -n 1)" \
    "load: $(cat /proc/loadavg)" >> "$log"
}
python3 -I "$here/generate.py" > "$here/units.toml" || exit 1
reading "before"
LOCK_OWNER="galoot wu17 E3 growth grid" bash "$lock" take 3600 || exit 1
trap 'bash "$lock" release; reading "lock released"' EXIT
reading "lock taken"
tools/compiles cost "$here/units.toml" "$out" --before "$1" --cpu 4 \
  --repeats 3
status=$?
bash "$lock" release
trap - EXIT
reading "lock released, exit $status"
[ "$status" = 0 ] && python3 -I "$here/analyze.py" "$out"
exit $status
