#!/usr/bin/env bash
# run_probe.sh <name> <src> <hdr old|new> <cxx> [extra flags...]: compiles (and, when it links, runs) one probe through tools/timed.
set -u
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/a38dc699-8d3a-42c2-bed5-9976e4a506c8/scratchpad/delta
name=$1 src=$2 hdr=$3 cxx=$4; shift 4
F="-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -fno-diagnostics-color"
cd /home/brianm/projects/utils/math-core/galoot/.claude/worktrees/delta-wu8b || exit 9
out=$S/probes/out/$name-$cxx-$hdr
mkdir -p $S/probes/out
tools/timed probe - "delta-wu8b:probe-$name:$cxx:$hdr" -- $cxx $F "$@" -I$S/$hdr/include -I$S/$hdr/test $S/probes/$src -o $out.bin >$out.txt 2>&1
rc=$?
echo "== $name $cxx $hdr compile exit $rc, errors $(grep -c 'error' $out.txt), lines $(wc -l <$out.txt)"
if [[ $rc -eq 0 && " $* " != *" -fsyntax-only "* ]]; then
  tools/timed probe - "delta-wu8b:run-$name:$cxx:$hdr" -- $out.bin >$out.run.txt 2>&1
  echo "   run exit $?"; cat $out.run.txt
fi
