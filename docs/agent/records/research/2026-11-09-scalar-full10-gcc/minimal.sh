#!/usr/bin/env bash
# The cases of minimal.cpp, in the order run. Run from the worktree root.
set -u
dir=docs/agent/records/research/2026-11-09-scalar-full10-gcc
run=$dir/run.sh
src=$dir/minimal.cpp
for keys in 1024 2048 4096; do
  "$run" minimal-k$keys-assert g++ "$src" -O0 -D_GLIBCXX_ASSERTIONS -DKEYS=$keys
  "$run" minimal-k$keys-plain g++ "$src" -O2 -DKEYS=$keys
done
"$run" minimal-k4096-assert-ops-2p28 g++ "$src" -O0 -D_GLIBCXX_ASSERTIONS -DKEYS=4096 -fconstexpr-ops-limit=268435456
"$run" minimal-k4096-clang-assert clang++ "$src" -O0 -D_GLIBCXX_ASSERTIONS -DKEYS=4096
