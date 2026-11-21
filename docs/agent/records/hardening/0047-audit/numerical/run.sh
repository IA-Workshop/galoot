#!/usr/bin/env bash
# Builds and runs the reproducers of the numerical audit register, writing
# each case's command, exit status, and output (the first 40 lines, which hold
# the diagnostic or the program's own lines) to <case>.out beside this script.
# Object files and binaries go to build/audit-numerical/ in the worktree,
# which is not tracked.
#
#   docs/agent/records/hardening/0047-audit/numerical/run.sh [case...]
#
# With no case, every case runs. Run from anywhere; paths are absolute from
# the script's own place. The p09 cases compile large algebras and run under
# memguard.sh with a per-job memory cap and a time limit.
set -u
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/../../../../../.." && pwd)
out_dir="$root/build/audit-numerical"
memguard=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common/memguard.sh
mkdir -p "$out_dir"

std=(-std=c++23 -I"$root/include" -Wall -Wextra)
ubsan=(-fsanitize=undefined -g)

# record <case> <command...>: runs the command, keeps its output and status.
record() {
  local name=$1
  shift
  {
    printf '$'
    printf ' %q' "${@/#$root\//}"
    printf '\n'
    local log="$out_dir/$name.log" status
    "$@" > "$log" 2>&1
    status=$?
    head -n 40 "$log"
    printf 'exit status: %s\n' "$status"
    printf 'error lines: %s\n' "$(grep -c 'error' "$log")"
    grep -m 3 -o "error: .*\|static assertion failed.*\|assertion failed: .*" "$log"
  } | sed "s|$root/||g" > "$here/$name.out"
}

# build_run <case> <compiler> <source> <flags...>: compiles, then runs.
build_run() {
  local name=$1 cxx=$2 src=$3
  shift 3
  local bin="$out_dir/$name"
  {
    printf '$ %s %s' "$cxx" "${src##*/}"
    printf ' %q' "${@/#$root\//}"
    printf '\n'
    if "$cxx" "${std[@]}" "$@" "$here/$src" -o "$bin" 2>&1 | head -n 40 &&
      [ "${PIPESTATUS[0]}" -eq 0 ]; then
      printf -- '--- run\n'
      "$bin" 2>&1 | head -n 40
      printf 'exit status: %s\n' "${PIPESTATUS[0]}"
    else
      printf 'compile exit status: nonzero\n'
    fi
  } | sed "s|$root/||g" > "$here/$name.out"
}

# compile_only <case> <compiler> <source> <flags...>: a compile-time case.
compile_only() {
  local name=$1 cxx=$2 src=$3
  shift 3
  record "$name" "$cxx" "${std[@]}" -fsyntax-only "$@" "$here/$src"
}

case_p01() {
  for cxx in g++ clang++; do
    build_run "p01.run.$cxx" "$cxx" p01_narrow_unsigned.cpp "${ubsan[@]}" \
      -DGALOOT_CHECKING=0
    compile_only "p01.constant.$cxx" "$cxx" p01_narrow_unsigned.cpp \
      -DCONSTANT_CASE
  done
}

case_p02() {
  for cxx in g++ clang++; do
    build_run "p02.unchecked.$cxx" "$cxx" p02_bits_unchecked.cpp "${ubsan[@]}" \
      -D_GLIBCXX_ASSERTIONS -DGALOOT_CHECKING=0
    build_run "p02.checked.$cxx" "$cxx" p02_bits_unchecked.cpp \
      -DGALOOT_CHECKING=1
  done
}

# GCC only: Clang's library here provides no std::float16_t.
case_p03() {
  build_run p03.table.g++ g++ p03_float16_check_floor.cpp -DGALOOT_CHECKING=1
  record p03.checked-half.g++ "$out_dir/p03.table.g++" half
  record p03.checked-float.g++ "$out_dir/p03.table.g++" float
}

case_p04() {
  for cxx in g++ clang++; do
    build_run "p04.$cxx" "$cxx" p04_rotation_edges.cpp -O2 -DGALOOT_CHECKING=1
  done
}

case_p05() {
  for cxx in g++ clang++; do
    for n in 1 2 3 4 5; do
      compile_only "p05.case$n.$cxx" "$cxx" p05_coefficient_edges.cpp -DCASE=$n
    done
  done
}

case_p06() {
  for cxx in g++ clang++; do
    compile_only "p06.rows.$cxx" "$cxx" p06_change_extreme_rows.cpp -DCASE=1
    build_run "p06.guard.$cxx" "$cxx" p06_change_extreme_rows.cpp -DCASE=2 \
      "${ubsan[@]}"
  done
}

case_p07() {
  for cxx in g++ clang++; do
    for n in 1 2 3 4 5; do
      compile_only "p07.case$n.$cxx" "$cxx" p07_diagnostics.cpp -DCASE=$n
    done
  done
}

# wide <compiler> <n> <op>: one sparse use of an algebra of n generators,
# under memguard (a 16 GiB cap per job) and a 600-second limit, timed by
# tools/timed into p09.timed.tsv beside this script (wall time, peak memory).
wide() {
  local cxx=$1 n=$2 op=$3 name="p09.n$2.op$3.$1"
  MEMGUARD_MAX_GB=16 MEMGUARD_MIN_GB=12 record "$name" "$memguard" \
    "$out_dir/$name.mem" -- timeout 600 "$root/tools/timed" --log \
    "$here/p09.timed.tsv" probe - "$name" -- "$cxx" "${std[@]}" \
    -fsyntax-only -DWIDE_N="$n" -DWIDE_OP="$op" "$here/p09_wide_algebras.cpp"
  tail -n 1 "$out_dir/$name.mem" | sed "s|^|memguard: |" >> "$here/$name.out"
}

# The Clang threshold of the term-list capacity, and whether raising the
# step limit lifts it (the note Clang prints suggests -fconstexpr-steps).
case_p09b() {
  for n in 21 22; do
    wide clang++ "$n" 4
  done
  MEMGUARD_MAX_GB=16 MEMGUARD_MIN_GB=12 record p09.n24.op4.clang++.steps \
    "$memguard" "$out_dir/p09.n24.op4.clang++.steps.mem" -- timeout 600 \
    "$root/tools/timed" --log "$here/p09.timed.tsv" probe - \
    p09.n24.op4.clang++.steps -- clang++ "${std[@]}" -fsyntax-only \
    -fconstexpr-steps=100000000 -DWIDE_N=24 -DWIDE_OP=4 \
    "$here/p09_wide_algebras.cpp"
}

case_p09() {
  local ns=(${P09_N:-17 24 32}) ops=(${P09_OPS:-1 2 3 4 5 6})
  for cxx in clang++ g++; do
    for n in "${ns[@]}"; do
      for op in "${ops[@]}"; do
        wide "$cxx" "$n" "$op"
      done
    done
  done
}

cases=("$@")
if [ ${#cases[@]} -eq 0 ]; then
  mapfile -t cases < <(declare -F | awk '{print $3}' | sed -n 's/^case_//p')
fi
for c in "${cases[@]}"; do
  "case_$c"
done
