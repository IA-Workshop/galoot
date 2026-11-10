#!/usr/bin/env bash
# One locked batch of drive.py run at one environment size, from the root of
# the checkout: reads the job pool and the load average, takes the shared
# timing lock, runs, releases it, and reads both again, appending each
# reading to out/protocol.log.
#
# Usage: docs/agent/records/performance/wu17-bench/batch.sh <env-bytes>
set -u
here=docs/agent/records/performance/wu17-bench
lock=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common/lock.sh
log=$here/out/protocol.log
env=$1
reading() {
  echo "env $env $1 $(date -u +%FT%TZ) pool: $(tools/jobpool status | head -n 1)" \
    "load: $(cat /proc/loadavg)" >> "$log"
}
reading "before"
LOCK_OWNER="galoot wu17 E2 rerun (inline reference), env $env" bash "$lock" take 3600 || exit 1
trap 'bash "$lock" release; reading "lock released"' EXIT
reading "lock taken"
python3 -I $here/drive.py run $here/out --cpu 4 --env "$env" --repeats 11
status=$?
bash "$lock" release
trap - EXIT
reading "lock released, exit $status"
exit $status
