#!/usr/bin/env bash
# Builds and runs probe.cpp once per line of a case file, on each compiler
# named in CXXS (default: g++ clang++), each compile through tools/timed with
# this directory's campaign log, and prints one line per build: compiler,
# case name, options, and what the program printed (or "compile error").
# A case line is a name, a tab, and the compiler options.
# From the worktree root:
#   bash docs/agent/records/research/2026-11-09-gcc-constexpr-init-bug/run.sh <case-file>
set -u
here=$(cd "$(dirname "$0")" && pwd)
cases=$1
out=${OUT_DIR:-$(mktemp -d)}
export TIMED_LOG="$here/runs.tsv"
read -r -a cxxs <<< "${CXXS:-g++ clang++}"
while IFS=$'\t' read -r name flags; do
  [[ -z $name || $name == \#* ]] && continue
  for cxx in "${cxxs[@]}"; do
    read -r -a opts <<< "$flags"
    if tools/timed probe - "gcc-init:$cxx:$name" -- \
      "$cxx" -std=c++23 "${opts[@]}" "$here/probe.cpp" -o "$out/a" \
      > "$out/log" 2>&1; then
      result=$("$out/a")
      status=$?
    else
      result="compile error: $(grep -m1 -E 'error' "$out/log")"
      status=-
    fi
    printf '%s\t%s\t%s\t%s (exit %s)\n' "$cxx" "$name" "$flags" "$result" "$status"
  done
done < "$cases"
[[ -z ${OUT_DIR:-} ]] && rm -r -- "$out"
