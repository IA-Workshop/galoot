#!/usr/bin/env bash
# Runs every reproducer of the merged register's own read and prints what
# each gave. Each is a small compile of one to four files, called with g++
# and clang++ directly, outside the test suite and the job pool.
#   run.sh <galoot repository> <scratch dir>
# The repository is needed for its include directory and, in m03, for the
# headers of the 1.0.0 tag and of the 1.0.1 candidate.
set -u
repo=$1; out=$2; here=$(cd "$(dirname "$0")" && pwd)
inc="$repo/include"
py=/home/brianm/projects/utils/math-core/math-tools/toolset/env/py/.venv/bin/python
mkdir -p "$out"
strip() { sed -e "s|$inc/||g" -e "s|$here/||g" -e "s|$out/||g"; }

echo "#### m01: uint16 coefficient path (N01)"
for cxx in g++ clang++; do
  echo "== $cxx, run time, -fsanitize=undefined"
  $cxx -std=c++23 -I"$inc" -Wall -Wextra -fsanitize=undefined -g \
    "$here/m01_uint16_coefficient.cpp" -o "$out/m01" && "$out/m01" 2>&1 | strip
  echo "== $cxx, constant evaluation"
  $cxx -std=c++23 -I"$inc" -fsyntax-only -DCONSTANT_CASE \
    "$here/m01_uint16_coefficient.cpp" 2>&1 |
    grep -E 'error|note: value' | head -3 | cut -c1-200 | strip
done

echo "#### m02: GALOOT_FORCE_INLINE mixed (H0-29)"
"$here/m02_force_inline_mixed/run.sh" "$inc" "$out/m02" 2>&1 | strip

echo "#### m03: two versions in one program (H0-30)"
"$here/m03_two_versions/run.sh" "$repo" "$out/m03" 2>&1 | strip

echo "#### m04: forced inlining into a narrower target (H0-31)"
for cxx in g++ clang++; do
  for fi in 1 0; do
    log=$($cxx -std=c++23 -O2 -march=x86-64-v3 -I"$inc" \
      -DGALOOT_FORCE_INLINE=$fi -c "$here/m04_target_mismatch_fail.cpp" \
      -o /dev/null 2>&1)
    echo "== $cxx GALOOT_FORCE_INLINE=$fi: exit $?, $(grep -c 'error' <<<"$log") error lines"
    grep -m2 'error' <<<"$log" | cut -c1-200 | strip
  done
done

echo "#### m05: the rounding check under other environments (H0-32, D09, O01)"
"$here/m05_run.sh" "$inc" "$out/m05" 2>&1 | strip
for cxx in g++ clang++; do
  $cxx -std=c++23 -O2 -I"$inc" -DGALOOT_CHECKING=1 -fassociative-math \
    -fno-signed-zeros -fno-trapping-math "$here/m05_check_environment.cpp" \
    -o "$out/m05b"
  echo "== $cxx -O2 -fassociative-math -fno-signed-zeros -fno-trapping-math, without -frounding-math"
  for family in r3 r4; do
    echo "   $("$out/m05b" $family 1 1 0 near)"
  done
done

echo "#### m06: special values in the rounding check (H0-33)"
for cxx in g++ clang++; do
  $cxx -std=c++23 -O2 -Wall -Wextra -I"$inc" -DGALOOT_CHECKING=1 \
    "$here/m06_check_special_values.cpp" -o "$out/m06"
  for c in nan-x inf-v huge-x subnormal-x tiny-nonversor; do
    echo "== $cxx $c: $("$out/m06" $c 2>&1 | head -1 | strip)"
  done
done

echo "#### m07: rga3::rotation campaign (O01)"
for cxx in g++ clang++; do
  $cxx -std=c++23 -O2 -Wall -Wextra -I"$inc" -DGALOOT_CHECKING=1 \
    "$here/m07_rotation_campaign.cpp" -o "$out/m07"
  echo "== $cxx: $("$out/m07")"
done

echo "#### m08: the margin inequality (D06)"
"$py" -I "$here/m08_margin_identity.py"
