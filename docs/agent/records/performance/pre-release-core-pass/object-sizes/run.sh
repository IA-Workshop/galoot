#!/usr/bin/env bash
# The code size of E1's probe (records/performance/wu17-asm/probe.cpp)
# against the headers at a revision before the forced inlining and at the
# revision checked out, at -O0 and at -O2 on each compiler, with E1's flags
# otherwise (checking off, the S4 warnings, no contraction). Writes one row
# per object to out/sizes.tsv: compiler, level, side, revision, and the
# text, data and bss sizes `size` reports. Run from the repository's root:
#   bash <this directory>/run.sh <revision before> [scratch directory]
# Each compile goes through tools/timed into runs.tsv beside this script.
set -euo pipefail
here=$(realpath --relative-to=. "$(dirname "${BASH_SOURCE[0]}")")
before=$1
scratch=${2:-build/pre-release-core-pass-sizes}
probe=docs/agent/records/performance/wu17-asm/probe.cpp
mkdir -p "$scratch/before" "$here/out"
rm -rf "$scratch/before/include"
git archive "$before" include | tar -x -C "$scratch/before"
export TIMED_LOG="$PWD/$here/runs.tsv"
after=$(git rev-parse --short=12 HEAD)
before_short=$(git rev-parse --short=12 "$before")
flags=(-DGALOOT_CHECKING=0 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
  -Werror -ffp-contract=off -std=c++23 -c)
printf 'compiler\tlevel\tside\trevision\ttext\tdata\tbss\n' > "$here/out/sizes.tsv"
for compiler in gcc clang; do
  case $compiler in
    gcc) cxx=g++ ;;
    clang) cxx=clang++ ;;
  esac
  for level in O0 O2; do
    for side in before after; do
      case $side in
        before) include=$scratch/before/include revision=$before_short ;;
        after) include=include revision=$after ;;
      esac
      object=$scratch/probe.$compiler.$level.$side.o
      tools/timed compile - "pre-release-core-pass:size:$compiler:$level:$side" -- \
        "$cxx" "${flags[@]}" "-$level" "-I$include" "$probe" -o "$object"
      read -r text data bss < <(size "$object" | awk 'NR == 2 {print $1, $2, $3}')
      printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\n' "$compiler" "$level" "$side" \
        "$revision" "$text" "$data" "$bss" >> "$here/out/sizes.tsv"
    done
  done
done
