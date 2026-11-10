#!/usr/bin/env bash
# Reruns the reviewer's mutant probes of the part C review by hand, outside
# tools/mutate: each mutant is applied to a copy of include/ in a scratch
# directory, and the test sources are compiled with the gcc-debug flags
# against that copy. The checkout's own files are never edited.
#
# Usage, from the root of a worktree at b1be3e9 whose gcc-debug tree is
# configured and has built galoot_doctest_main, with the pool exported
# (eval "$(tools/jobpool env)"):
#   bash docs/agent/records/reviews/2026-11-10-wu17-part-c-blind/probes.sh <scratch dir>
set -eu
root=$(pwd)
here=$root/docs/agent/records/reviews/2026-11-10-wu17-part-c-blind
out=$(realpath "$1")
mkdir -p "$out"
doctest=$root/build/gcc-debug/test/CMakeFiles/galoot_doctest_main.dir/__/include/ext/doctest/doctest.cpp.o
flags=(-DDOCTEST_CONFIG_SUPER_FAST_ASSERTS -isystem "$root/include/ext" -Wall
  -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off
  -D_GLIBCXX_ASSERTIONS -O0 -g -std=c++23)

# make_copy <name> <file under include/> <anchor> <replacement>
make_copy() {
  rm -rf "${out:?}/inc_$1"
  cp -r "$root/include" "$out/inc_$1"
  python3 -I - "$out/inc_$1/${2#include/}" "$3" "$4" <<'EOF'
import sys
path, anchor, replacement = sys.argv[1:]
text = open(path).read()
assert text.count(anchor) == 1, f"anchor occurs {text.count(anchor)} times"
open(path, "w").write(text.replace(anchor, replacement))
EOF
}

# run <mutant or orig> <checking 0 or 1> <test source under test/ or here>
run() {
  local inc=$root/include
  [[ $1 != orig ]] && inc=$out/inc_$1
  local name
  name=$(basename "$3" .cpp)
  tools/timed build gcc-debug "scratch:$name mutant:$1" -- g++ \
    -DGALOOT_CHECKING="$2" -I"$inc" -I"$root/test" "${flags[@]}" \
    -o "$out/${1}_${name}_${2}" "$3" "$doctest"
  echo "$1 $name checking=$2: $("$out/${1}_${name}_${2}" 2>&1 |
    grep -E '^\[doctest\] test cases|^mul ' || true)"
}

# first_stage_all, as gap.toml writes it
make_copy first_stage_all include/galoot/core/transform.hpp \
  '        detail::transform_middle<Alg, V, X, false>(v.coefficients,' \
  '        detail::transform_middle<Alg, V, X, true>(v.coefficients,'
# double_negation: a first term of sign +1 assigned as -(-value)
make_copy double_negation include/galoot/core/mv.hpp \
  $'  if constexpr (C == 1) {\n    if constexpr (First) {\n      out = value;' \
  $'  if constexpr (C == 1) {\n    if constexpr (First) {\n      out = static_cast<T>(-static_cast<T>(-value));'
# neg_as_sub: a first term of sign -1 assigned as T{} - value
make_copy neg_as_sub include/galoot/core/mv.hpp \
  '      out = static_cast<T>(-value);' \
  '      out = static_cast<T>(T{} - value);'

# The degenerate transform's counts, the code and first_stage_all
for m in orig first_stage_all; do
  for c in 0 1; do
    tools/timed build gcc-debug "scratch:degenerate_probe mutant:$m" -- g++ \
      -DGALOOT_CHECKING=$c -I"$([[ $m == orig ]] && echo "$root/include" ||
        echo "$out/inc_$m")" -I"$root/test" -std=c++23 -Wall -Wextra -Werror \
      -o "$out/probe_${m}_$c" "$here/degenerate_probe.cpp"
    echo "$m checking=$c: $("$out/probe_${m}_$c")"
  done
done

# core.rounding against the gap mutants of gap.toml, from its own edits
python3 -I - "$root" "$out" <<'EOF'
import os, shutil, sys, tomllib
root, out = sys.argv[1:]
spec = tomllib.load(open(f"{root}/docs/agent/records/mutations/"
                         "0038-cost-evidence/gap.toml", "rb"))
for m in spec["mutant"]:
    if m["name"] == "first_stage_all":
        continue
    d = f"{out}/inc_{m['name']}"
    shutil.rmtree(d, ignore_errors=True)
    shutil.copytree(f"{root}/include", d)
    path = os.path.join(d, m["file"].removeprefix("include/"))
    text = open(path).read()
    for e in m["edits"]:
        assert text.count(e["anchor"]) == 1
        text = text.replace(e["anchor"], e["replacement"])
    open(path, "w").write(text)
EOF
for m in orig product_kernel_zero_term sign_multiplied cancelling_pair; do
  run "$m" 1 "$root/test/core/rounding_test.cpp"
done
for t in core/rounding_test core/transform_rounding_test core/product_test \
  core/transform_test; do
  run double_negation 1 "$root/test/$t.cpp"
done
run neg_as_sub 1 "$root/test/core/rounding_test.cpp"
for c in 0 1; do
  run double_negation $c "$root/test/presets/rga3/op_counts_test.cpp"
done
