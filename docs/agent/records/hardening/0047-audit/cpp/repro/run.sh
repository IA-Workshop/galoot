#!/usr/bin/env bash
# Compiles and runs every reproducer of the cpp audit register on GCC and
# Clang and prints what each gave. Usage: run.sh <galoot include dir> <scratch dir>
# Each line of output names the reproducer, the compiler, and the flags.
# Paths under the include directory are printed relative to it.
# A failing compile prints its count of errors (diagnostics, not lines).
set -u
inc=$1; out=$2; here=$(cd "$(dirname "$0")" && pwd)
mkdir -p "$out"
strip() { sed -e "s|$inc/||g" -e "s|$here/||g" -e "s|$out/||g"; }
# An error is a line that starts with a location and "error:". A count of the
# lines that contain "error" is not a count of errors: it takes in Clang's
# closing "N errors generated.", GCC's note "... resulted in errors seen
# above", and every line of context that prints "<expression error>", and so
# reads too high.
diagnostic='^[^[:space:]]+: (fatal )?error: '
fail() {  # fail <file> <compiler> [flags...]: the count of errors and the first three
  local f=$1 cxx=$2; shift 2
  local log count
  log=$("$cxx" -std=c++23 -fsyntax-only -I"$inc" "$@" "$here/$f" 2>&1)
  count=$(grep -cE "$diagnostic" <<<"$log")
  echo "== $f, $cxx $*: $count error$([ "$count" = 1 ] || echo s)"
  grep -E "$diagnostic" <<<"$log" | head -3 | strip | cut -c1-240
}
run() {  # run <file> <compiler> [flags...]: build, run, output, exit status
  local f=$1 cxx=$2; shift 2
  echo "== $f, $cxx $*"
  if "$cxx" -std=c++23 -I"$inc" "$@" "$here/$f" -o "$out/bin" 2>"$out/build.log"; then
    "$out/bin" 2>&1 | strip
    echo "exit status ${PIPESTATUS[0]}"
  else
    echo "build failed:"; grep -E "$diagnostic" "$out/build.log" | head -3 | strip | cut -c1-240
  fi
}
main() {
  for cxx in g++ clang++; do
    fail r01_one_blade_explicit_fail.cpp "$cxx"
    fail r02_many_blades_explicit_fail.cpp "$cxx"
  done
  "$here/r03_checking_mixed/run.sh" "$inc" "$out" 2>&1 | strip
  for cxx in g++ clang++; do
    run r04_bits_unchecked_ub.cpp "$cxx" -O0 -fsanitize=undefined
    run r05_canonical_name_unchecked.cpp "$cxx" -O0
    run r06_is_null_unchecked.cpp "$cxx" -O0 -D_GLIBCXX_ASSERTIONS
    run r07_char_scalar_format.cpp "$cxx"
    run r08_unterminated_names.cpp "$cxx"
    fail r09_get_derived_fail.cpp "$cxx"
    fail r10_scale_derived_fail.cpp "$cxx"
    run r11_repeated_mask_layouts.cpp "$cxx"
    fail r12_orthogonal_change_overflow_fail.cpp "$cxx" -DW=79
    fail r12_orthogonal_change_overflow_fail.cpp "$cxx" -DW=78
    run r12_orthogonal_change_overflow_fail.cpp "$cxx" -DW=79 -DPROBE
    run r12_orthogonal_change_overflow_fail.cpp "$cxx" -DW=78 -DPROBE
    run r14_rotation_angle_not_finite.cpp "$cxx" -DGALOOT_CHECKING=1
    fail r15_translation_bool_fail.cpp "$cxx"
    run r16_empty_layout.cpp "$cxx" -Wall -Wextra -Werror
    run r19_int_scalar_overflow.cpp "$cxx" -O0 -fsanitize=undefined
    fail r21_tab_separator_fail.cpp "$cxx"
  done
  run r13_bitint_change_truncates.cpp clang++
  run r18_float16_rounding_floor.cpp g++ -DGALOOT_CHECKING=1 -DSCALE=0.1f
  run r18_float16_rounding_floor.cpp g++ -DGALOOT_CHECKING=1 -DSCALE=0.05f
  for cxx in g++ clang++; do
    for gen in 21 22; do fail r17_wide_algebra_capacity.cpp "$cxx" -DGEN=$gen; done
    for gen in 19 20; do fail r20_wide_vectors_wedge.cpp "$cxx" -DGEN=$gen; done
  done
  for gen in 30 31; do fail r20_wide_vectors_wedge.cpp g++ -DGEN=$gen; done
  # The checking mode's spellings: each value, whether it builds, and whether
  # the program then checks (exit 134, an abort) or not (exit 40).
  printf '#include <galoot/core/bits.hpp>\nint main(int c, char**) { return galoot::core::antigrade(1, static_cast<unsigned>(c) + 40U); }\n' > "$out/spelling.cpp"
  for v in 1 0 true false 01 0x1 '1-0' ON '' 2; do
    for cxx in g++ clang++; do
      if "$cxx" -std=c++23 -I"$inc" "-DGALOOT_CHECKING=$v" "$out/spelling.cpp" -o "$out/spelling" 2>"$out/spelling.log"; then
        "$out/spelling" >/dev/null 2>&1; echo "== GALOOT_CHECKING=[$v], $cxx: builds, exit status $?"
      else
        echo "== GALOOT_CHECKING=[$v], $cxx: $(grep -m1 -E "$diagnostic" "$out/spelling.log" | strip | cut -c1-160)"
      fi
    done
  done
}
# The shell's report of an aborted program names the script's path, a line and
# a process number. They are replaced, and paths are stripped before a line is
# cut, so that two runs give the same output wherever the repository is.
main 2>&1 | sed -E -e "s|$here/||g" \
  -e 's/(run\.sh: line) [0-9]+: +[0-9]+ (Aborted)/\1 N: \2/'
