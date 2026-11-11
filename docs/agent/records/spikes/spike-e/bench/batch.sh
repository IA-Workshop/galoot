#!/usr/bin/env bash
# One locked batch of drive.py, from the root of the checkout: reads the job
# pool and the load average, takes the shared timing lock, runs, releases
# it, and reads both again, appending each reading to out/protocol.log.
#
# Usage: docs/agent/records/spikes/spike-e/bench/batch.sh calibrate
#        docs/agent/records/spikes/spike-e/bench/batch.sh run <env-bytes>
#        docs/agent/records/spikes/spike-e/bench/batch.sh sweep
#        docs/agent/records/spikes/spike-e/bench/batch.sh stalls
set -u
here=docs/agent/records/spikes/spike-e/bench
lock=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common/lock.sh
mkdir -p "$here/out"
log=$here/out/protocol.log
what=$1
case $what in
  calibrate) args=(calibrate "$here/out/calibration" --cpu 4) ;;
  run) args=(run "$here/out" --cpu 4 --env "$2" --repeats 11) ;;
  sweep) args=(sweep "$here/out/sweep" --cpu 4 --repeats 5) ;;
  stalls) args=(stalls "$here/out" --cpu 4) ;;
  *) echo "usage: batch.sh calibrate|run <env>|sweep|stalls" >&2; exit 2 ;;
esac
label="$what ${2:-}"
reading() {
  echo "$label $1 $(date -u +%FT%TZ) pool: $(tools/jobpool status | head -n 1)" \
    "load: $(cat /proc/loadavg)" >> "$log"
}
reading "before"
LOCK_OWNER="galoot spike E part F benchmarks, $label" bash "$lock" take 3600 || exit 1
trap 'bash "$lock" release; reading "lock released (trap)"' EXIT
reading "lock taken"
python3 -I $here/drive.py "${args[@]}"
status=$?
bash "$lock" release
trap - EXIT
reading "lock released, exit $status"
exit $status
