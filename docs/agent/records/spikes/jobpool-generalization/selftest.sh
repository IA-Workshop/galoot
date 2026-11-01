#!/usr/bin/env bash
# Checks jobpool against a pool of its own: a unit and FIFO named after this
# shell's pid, so that it never touches a pool already running on the machine.
# Needs systemd user services and python3; the Ninja checks also need ninja
# and are skipped without it. Takes about half a minute.
#
# Usage: selftest.sh [path/to/jobpool]
set -u

tool=$(readlink -f "${1:-$(dirname "$0")/jobpool}")
export JOBPOOL_CONFIG=/dev/null
export JOBPOOL_UNIT=jobpool-selftest-$$
export JOBPOOL_FIFO=${XDG_RUNTIME_DIR:-/run/user/$(id -u)}/jobpool-selftest-$$.fifo
export JOBPOOL_POLL_SECONDS=0.3
export JOBPOOL_IDLE=0
export JOBPOOL_REPAIR=off
unset MAKEFLAGS JOBPOOL_SLOTS JOBPOOL_RESERVE JOBPOOL_HELD
work=$(mktemp -d)
failures=0
checks=0

cleanup() {
  systemctl --user stop "$JOBPOOL_UNIT.service" 2>/dev/null
  rm -rf -- "$work"
  rm -f -- "$JOBPOOL_FIFO.lock"
}
trap cleanup EXIT

jp() { python3 -I "$tool" "$@"; }

check() {
  local what=$1
  shift
  checks=$((checks + 1))
  if "$@"; then
    echo "ok    $what"
  else
    echo "FAIL  $what"
    failures=$((failures + 1))
  fi
}

free() {
  python3 -I -c '
import fcntl, os, struct, sys, termios
fd = os.open(sys.argv[1], os.O_RDWR | os.O_NONBLOCK)
print(struct.unpack("i", fcntl.ioctl(fd, termios.FIONREAD, b"\0" * 4))[0])' "$JOBPOOL_FIFO"
}

# Takes $1 bytes from the pool, then sleeps $2 seconds and returns them. It
# replaces the shell it runs in, so that `hold ... &` leaves its pid in $!.
hold() {
  exec python3 -I -c '
import os, sys, time
fd = os.open(sys.argv[1], os.O_RDWR)
got = b"".join(os.read(fd, 1) for _ in range(int(sys.argv[2])))
time.sleep(float(sys.argv[3]))
os.write(fd, got)' "$JOBPOOL_FIFO" "$1" "$2"
}

# Takes $1 bytes from the pool and dies by SIGKILL without returning them.
killed_holding() {
  python3 -I -c '
import os, signal, sys
fd = os.open(sys.argv[1], os.O_RDWR)
os.read(fd, int(sys.argv[2]))
os.kill(os.getpid(), signal.SIGKILL)' "$JOBPOOL_FIFO" "$1" &
  wait $! 2>/dev/null
}

wait_for() {
  local seconds=$1
  shift
  local end=$((SECONDS + seconds))
  until "$@"; do
    ((SECONDS < end)) || return 1
    sleep 0.1
  done
}

equals() { [[ $1 == "$2" ]] || { echo "      expected '$2', got '$1'"; return 1; }; }
contains() { [[ $1 == *"$2"* ]] || { echo "      expected '$2' in: $1"; return 1; }; }
not_running() { ! systemctl --user is-active --quiet "$JOBPOOL_UNIT.service"; }

# Runs a Ninja build of eight one-second jobs, under the command prefix given
# as arguments, with each job logging its start and end to the file $log.
ninja_build() {
  local dir
  dir=$(mktemp -d -p "$work")
  {
    echo "rule job"
    echo "  command = echo s \$\$(date +%s.%N) >> $log; sleep 1; echo e \$\$(date +%s.%N) >> $log; touch \$out"
    for i in 1 2 3 4 5 6 7 8; do echo "build out$i: job"; done
  } >"$dir/build.ninja"
  "$@" ninja -C "$dir" >/dev/null
}

# Prints the most jobs that ran at once according to the log file $1.
most_at_once() {
  sort -k2 -n "$1" | awk '$1 == "s" { n++; if (n > m) m = n } $1 == "e" { n-- } END { print m }'
}

[[ $JOBPOOL_UNIT == jobpool-selftest-* ]] || { echo "refusing to run on unit $JOBPOOL_UNIT"; exit 2; }

echo "Settings"
cores=$(cat /sys/devices/system/cpu/cpu[0-9]*/topology/thread_siblings_list 2>/dev/null | sort -u | wc -l)
check "default slots are physical cores less one" contains "$(jp config)" "slots: $((cores - 1)) ($cores physical cores less 1)"
check "reserve changes the default" contains "$(JOBPOOL_RESERVE=4 jp config)" "slots: $((cores - 4))"
printf 'slots = 7\nidle = 90s\n' >"$work/config"
check "config file is read" contains "$(JOBPOOL_CONFIG=$work/config JOBPOOL_IDLE= jp config 2>&1)" "slots: 7"
JOBPOOL_IDLE=bogus jp config >/dev/null 2>&1
check "a bad duration is refused" equals $? 2

echo "Lifecycle"
jp env >/dev/null 2>&1
check "env fails while the pool is not running" equals $? 1
jp status >/dev/null
check "status exits 1 while not running" equals $? 1
check "start" contains "$(jp start 3)" "started: 3 of 3 slots free"
check "ensure leaves a running pool alone" contains "$(jp ensure)" "already running"
check "env names the FIFO" contains "$(jp env)" "--jobserver-auth=fifo:$JOBPOOL_FIFO"
jp status >/dev/null
check "status exits 0 on a healthy pool" equals $? 0

echo "Clients"
hold 1 3 &
holder=$!
wait_for 2 bash -c "[[ \$(python3 -I '$tool' status) == *clients:* ]]"
check "a holder is listed as a client" contains "$(jp status)" "clients: $holder"
check "a non-canonical path still finds the client" contains "$(JOBPOOL_FIFO=${JOBPOOL_FIFO%/*}//./${JOBPOOL_FIFO##*/} jp status)" "clients: $holder"
jp stop >/dev/null 2>&1
check "stop refuses while a client holds the pool" equals $? 1
jp repair >/dev/null 2>&1
check "repair refuses while a client holds the pool" equals $? 1
wait "$holder"
check "slots come back when the client returns them" equals "$(free)" 3

echo "Lost and surplus slots"
killed_holding 2
check "a client killed with SIGKILL loses its slots" equals "$(free)" 1
out=$(jp status)
check "status reports the lost slots" contains "$out" "2 slots are lost"
jp status >/dev/null
check "status exits 3 with lost slots" equals $? 3
check "repair restores them" equals "$(jp repair)" "restored 2 slots"
check "the pool is whole again" equals "$(free)" 3
python3 -I -c 'import os, sys; os.write(os.open(sys.argv[1], os.O_RDWR), b"++")' "$JOBPOOL_FIFO"
check "status reports surplus slots" contains "$(jp status)" "2 surplus slots"
check "repair drains them" equals "$(jp repair)" "drained 2 surplus slots"
jp stop >/dev/null

echo "Automatic repair"
JOBPOOL_REPAIR=auto jp start 3 >/dev/null
killed_holding 3
check "the service restores lost slots once no client holds the pool" wait_for 5 bash -c "[[ \$(python3 -I -c '
import fcntl, os, struct, sys, termios
fd = os.open(sys.argv[1], os.O_RDONLY | os.O_NONBLOCK)
print(struct.unpack(\"i\", fcntl.ioctl(fd, termios.FIONREAD, b\"\\0\" * 4))[0])' '$JOBPOOL_FIFO') == 3 ]]"
jp stop >/dev/null

echo "run"
check "run starts the pool when it is not running" contains "$(jp run -- sh -c 'echo "$MAKEFLAGS"' 2>/dev/null)" "fifo:$JOBPOOL_FIFO"
check "the command holds one slot while it runs" equals "$(jp run -- python3 -I -c '
import fcntl, os, struct, sys, termios
fd = os.open(sys.argv[1], os.O_RDONLY | os.O_NONBLOCK)
print(struct.unpack("i", fcntl.ioctl(fd, termios.FIONREAD, b"\0" * 4))[0])' "$JOBPOOL_FIFO")" $((cores - 2))
check "and returns it" equals "$(free)" "$((cores - 1))"
check "a nested run takes no second slot" equals "$(jp run -- python3 -I "$tool" run -- python3 -I -c '
import fcntl, os, struct, sys, termios
fd = os.open(sys.argv[1], os.O_RDONLY | os.O_NONBLOCK)
print(struct.unpack("i", fcntl.ioctl(fd, termios.FIONREAD, b"\0" * 4))[0])' "$JOBPOOL_FIFO")" $((cores - 2))
jp run -- sh -c 'exit 7'
check "run passes the exit status through" equals $? 7
jp run -- sh -c 'kill -TERM $$'
check "run reports a command killed by a signal as 128 + signal" equals $? 143
check "the slot comes back after a killed command" equals "$(free)" "$((cores - 1))"
python3 -I "$tool" run -- sleep 5 &
runner=$!
wait_for 2 bash -c "[[ \$(python3 -I '$tool' status) == *clients:* ]]"
kill -TERM "$runner"
wait "$runner"
check "SIGTERM to run reaches the command and the slot comes back" equals "$(free)" "$((cores - 1))"
jp stop >/dev/null

if command -v ninja >/dev/null; then
  echo "Ninja"
  jp start 3 >/dev/null
  log=$work/direct.log
  MAKEFLAGS="-j3 --jobserver-auth=fifo:$JOBPOOL_FIFO" ninja_build env
  check "Ninja started directly runs one job beyond the slots" equals "$(most_at_once "$log")" 4
  log=$work/run.log
  ninja_build jp run --
  check "Ninja under run stays within the slots" equals "$(most_at_once "$log")" 3
  log=$work/two.log
  ninja_build jp run -- &
  ninja_build jp run -- &
  wait
  check "two Ninja builds under run together stay within the slots" equals "$(most_at_once "$log")" 3
  check "all slots are back after the builds" equals "$(free)" 3
  jp stop >/dev/null
else
  echo "skip  Ninja checks (ninja not installed)"
fi

echo "Idle stop"
JOBPOOL_IDLE=2s jp start 2 >/dev/null
hold 0 3 &
holder=$!
sleep 2.5
check "a client keeps the pool past the idle time" systemctl --user is-active --quiet "$JOBPOOL_UNIT.service"
wait "$holder"
check "the pool stops after the idle time without a client" wait_for 5 not_running
check "and removes its FIFO" test ! -e "$JOBPOOL_FIFO"

echo
echo "$((checks - failures)) of $checks checks passed"
((failures == 0))
