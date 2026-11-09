#!/usr/bin/env bash
# Builds and runs the whole test suite of an instrumented checkout (made by
# instrument.py --kernels) under one GCC preset, with init_probe.hpp
# force-included, each step through tools/timed with this directory's
# campaign log, and leaves the probe's log and CTest's output in <out>.
# -Werror is left out of the preset's flags, so that a warning the
# instrumentation provokes does not stop the build.
# Usage: bash scan.sh <instrumented checkout> <gcc-debug|gcc-release> <out>
set -u
here=$(cd "$(dirname "$0")" && pwd)
tree=$1
preset=$2
out=$3
mkdir -p "$out"
cd "$tree" || exit 2
eval "$(tools/jobpool env)"
export TIMED_LOG="$here/runs.tsv"
flags="-Wall -Wextra -Wpedantic -Wconversion -Wshadow -ffp-contract=off"
flags+=" -D_GLIBCXX_ASSERTIONS -include $here/init_probe.hpp"
tools/timed configure "$preset" instrumented -- \
  cmake --preset "$preset" -DCMAKE_CXX_FLAGS="$flags" > "$out/configure.log" 2>&1
tools/timed build "$preset" instrumented:all -- \
  cmake --build --preset "$preset" -- -k 0 > "$out/build.log" 2>&1
echo "build exit $?"
rm -f -- "$out/probe.log"
GALOOT_INIT_PROBE_LOG="$out/probe.log" tools/timed test "$preset" \
  instrumented:all -- ctest --preset "$preset" > "$out/test.log" 2>&1
echo "test exit $?"
grep -E "tests passed|tests failed" "$out/test.log"
grep -E "^\s+[0-9]+ - " "$out/test.log"
[[ -f $out/probe.log ]] && wc -l < "$out/probe.log"
