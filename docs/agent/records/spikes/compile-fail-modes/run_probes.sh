#!/usr/bin/env bash
# For each probe, compiler, and preset flag set: exit status and the
# diagnostics (error/warning lines) with -c and with -fsyntax-only.
set -u
dir=$1
probes="error_attribute warning_attribute return_type uninitialized maybe_uninitialized array_bounds dynamic_bounds inline_asm always_inline unused_static undefined_static stringop use_after_free dangling free_nonheap null_deref template_end fe_and_codegen constexpr_unused"
warn="-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS"
san="-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer"
printf 'probe\tcompiler\tflags\texit_c\texit_s\tdiagnostics only with -c\n'
for p in $probes; do
  for cxx in g++ clang++; do
    for set in "debug:-O0 -g" "release:-O2" "asan:-O1 -g $san"; do
      name=${set%%:*}; opt=${set#*:}
      c=$($cxx $warn $opt -std=c++23 -o $dir/out.o -c $dir/$p.cpp 2>&1); ec=$?
      s=$($cxx $warn $opt -std=c++23 -fsyntax-only $dir/$p.cpp 2>&1); es=$?
      extra=$(diff <(grep -oE '(error|warning): .*' <<<"$s" | sort -u) <(grep -oE '(error|warning): .*' <<<"$c" | sort -u) | grep '^>' | sed 's/^> //' | tr '\n' '|')
      printf '%s\t%s\t%s\t%s\t%s\t%s\n' $p $cxx $name $ec $es "${extra:--}"
    done
  done
done
