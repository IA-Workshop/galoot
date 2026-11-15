#!/usr/bin/env bash
# Spike C: whether C++26 diagnostic features are usable under -std=c++23 with
# the S4 warnings and -Werror (and, for comparison, under -std=c++26). Runs
# every probe in probes/ once per compiler, serially, each compile under
# tools/timed, and writes out/probes/summary.tsv (exit status and the first
# two diagnostic lines) with the full texts beside it.
#
# Usage, from anywhere:  bash run_probes.sh
set -u
here=$(cd "$(dirname "$0")" && pwd)
root=$(git -C "$here" rev-parse --show-toplevel)
p=$here/probes
out=$here/out/probes
mkdir -p "$out"
warn=(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror
      -ffp-contract=off -fno-diagnostics-color)
summary=$out/summary.tsv
printf 'compiler\tprobe\tflags\texit\tfirst_diagnostics\n' >"$summary"

record() { # name flags... -- compiler args...
  local name=$1 flags=$2 cxx=$3
  shift 3
  local txt=$out/${cxx%++}-$name.txt
  "$root/tools/timed" build - "spike-c:probe:$cxx:$name" -- "$cxx" "$@" \
    2>"$txt" >/dev/null
  local status=$?
  sed -i -e "s|$here/||g" -e "/^timed: /d" "$txt"
  printf '%s\t%s\t%s\t%s\t%s\n' "$cxx" "$name" "$flags" "$status" \
    "$(grep -E 'error|warning' "$txt" | head -2 | paste -sd'|')" >>"$summary"
}

for cxx in g++ clang++; do
  for probe in UGSA_OK UGSA DELETE_REASON_OK DELETE_REASON SA_FALSE_OK SA_FALSE; do
    for std in c++23 c++26; do
      record "cxx26-$probe-$std" "S4 -Werror -std=$std" "$cxx" -std=$std \
        "${warn[@]}" -DPROBE_$probe -fsyntax-only "$p/cxx26.cpp"
    done
    record "cxx26-$probe-bare" "-std=c++23 only" "$cxx" -std=c++23 \
      -DPROBE_$probe -fsyntax-only "$p/cxx26.cpp"
  done
  for std in c++23 c++26; do
    printf '%s\tmacros-%s\t-E -P\t0\t%s\n' "$cxx" "$std" \
      "$("$cxx" -std=$std -E -P "$p/macros.cpp" | grep '^long' | paste -sd' ')" \
      >>"$summary"
  done
  for k in 3 5; do
    record "pragma-K$k" "S4 -Werror" "$cxx" -std=c++23 "${warn[@]}" -DFK=$k \
      -fsyntax-only "$p/pragma.cpp"
    record "pragma-K$k-pedantic-errors" "-pedantic-errors" "$cxx" -std=c++23 \
      -pedantic-errors -fno-diagnostics-color -DFK=$k -fsyntax-only \
      "$p/pragma.cpp"
    for inc in -I -isystem; do
      record "sys-K$k$inc" "S4 -Werror $inc" "$cxx" -std=c++23 "${warn[@]}" \
        "$inc" "$p/sys" -DFK=$k -fsyntax-only "$p/sys_use.cpp"
    done
  done
done
column -t -s $'\t' "$summary"
