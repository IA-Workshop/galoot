#!/usr/bin/env bash
# Every library case of the record, grouped (runs.tsv has the order in which
# they ran). Run from the worktree root after eval "$(tools/jobpool env)",
# with a scratch directory for the run-time check as the argument.
set -u
dir=docs/agent/records/research/2026-11-09-scalar-full10-gcc
run=$dir/run.sh
src=$dir/product.cpp
# The flags of the gcc-debug tree (core.metric_type_width), without -o and -c.
debug=(-DGALOOT_CHECKING=1 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
  -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -O0)
# The flags of the gcc-release tree: -O2, no libstdc++ assertions.
release=(-DGALOOT_CHECKING=1 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
  -Werror -ffp-contract=off -O2)
"$run" debug-n9 g++ "$src" "${debug[@]}" -DGENERATORS=9
"$run" debug-n10 g++ "$src" "${debug[@]}"
"$run" release-n9 g++ "$src" "${release[@]}" -DGENERATORS=9
"$run" release-n10 g++ "$src" "${release[@]}"
"$run" debug-n10-loop-2p24 g++ "$src" "${debug[@]}" -fconstexpr-loop-limit=16777216
"$run" debug-n10-depth-4096 g++ "$src" "${debug[@]}" -fconstexpr-depth=4096
"$run" debug-n10-ops-2p26 g++ "$src" "${debug[@]}" -fconstexpr-ops-limit=67108864
"$run" debug-n10-ops-2p27 g++ "$src" "${debug[@]}" -fconstexpr-ops-limit=134217728
"$run" release-n10-ops-2p26 g++ "$src" "${release[@]}" -fconstexpr-ops-limit=67108864
"$run" debug-n10-ops-2p27p1 g++ "$src" "${debug[@]}" -fconstexpr-ops-limit=134217729
# The plan alone, without mv.
plan=$dir/plan.cpp
"$run" plan-debug-n9 g++ "$plan" "${debug[@]}" -DGENERATORS=9
"$run" plan-debug-n10 g++ "$plan" "${debug[@]}"
"$run" plan-debug-n10-ops-2p27 g++ "$plan" "${debug[@]}" -fconstexpr-ops-limit=134217728
# Clang, an independent constant evaluator, on the plan alone at n = 10.
"$run" plan-clang-debug-n10 clang++ "$plan" "${debug[@]}"
"$run" plan-clang-debug-n10-steps-2p30 clang++ "$plan" "${debug[@]}" -fconstexpr-steps=1073741824
"$run" plan-clang-debug-n9 clang++ "$plan" "${debug[@]}" -DGENERATORS=9
"$run" plan-clang-debug-n8 clang++ "$plan" "${debug[@]}" -DGENERATORS=8
"$run" plan-clang-debug-n7 clang++ "$plan" "${debug[@]}" -DGENERATORS=7
# The collect phase at n = 9, for the growth from 9 to 10.
"$dir/bisect.sh" collect-n9 'collected<|collect_result' 1000000 33554432 -DGENERATORS=9
# The thresholds at n = 10: the collect phase, then the whole unit.
"$dir/bisect.sh" collect2 'collected<|collect_result' 16777216 67108864
"$dir/bisect.sh" unit2 '.' 67108864 134217728
# The plan's phases at run time, under libstdc++'s assertions and the
# sanitizers; the binary goes to $1, a scratch directory.
out=${1:-/tmp}
tools/timed --log "$dir/runs.tsv" compile - scalar-full10:runtime-check-gcc2 -- \
  g++ -I include -std=c++23 -DGALOOT_CHECKING=1 -Wall -Wextra -Wpedantic \
  -Wconversion -Wshadow -Werror -D_GLIBCXX_ASSERTIONS -O0 -g \
  -fsanitize=address,undefined -fno-sanitize-recover=all \
  -o "$out/runtime_check" "$dir/runtime_check.cpp" &&
  tools/timed --log "$dir/runs.tsv" run - scalar-full10:runtime-check-gcc2 -- \
    "$out/runtime_check"
# The same unit against the kernel-init unit's headers (feature/core-kernel-init
# at 1668070, include/ of 6d00878), extracted to $1/kernel-init-include by
#   git archive 1668070 include | tar -x -C "$out/kernel-init-include"
# and compiled at the default limit and at 2^27 operations.
