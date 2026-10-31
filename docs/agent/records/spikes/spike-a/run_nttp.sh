#!/usr/bin/env bash
# Spike A: value NTTP against tag types. For each compiler and style, compile
# nttp.cpp twice at -O0: once correctly, to read gp's mangled name with nm,
# and once with SPIKE_MISUSE, to keep the diagnostic. Outputs go to
# out/nttp/. Run from anywhere; every compile is serial and under tools/timed.
set -u
here=$(cd "$(dirname "$0")" && pwd)
root=$(git -C "$here" rev-parse --show-toplevel)
out=$here/out/nttp
mkdir -p "$out"
summary=$out/summary.tsv
printf 'compiler\tstyle\tmangled_len\tdemangled_len\tnested_mangled_lens\tdiag_lines\tdiag_bytes\tdiag_max_line\n' >"$summary"

for cxx in g++ clang++; do
  for style in VALUE TAG HYBRID; do
    tag=${cxx%++}-${style,,}
    "$root/tools/timed" build - "spike-a:nttp:$cxx:$style" -- \
      "$cxx" -std=c++23 -O0 -c "-DSPIKE_$style" -o "$out/$tag.o" \
      "$here/nttp.cpp"
    # gp instantiations by mangled length: the first is a vector times a
    # vector, the last the outermost product of use3, ((a b) c) d.
    nm "$out/$tag.o" | awk '$2 ~ /[TW]/ { print $3 }' | grep -E '2gp' |
      awk '{ print length, $0 }' | sort -n >"$out/$tag.all-gp"
    sym=$(head -1 "$out/$tag.all-gp" | cut -d' ' -f2)
    printf '%s\n' "$sym" >"$out/$tag.mangled"
    printf '%s\n' "$sym" | llvm-cxxfilt >"$out/$tag.demangled"

    "$root/tools/timed" build - "spike-a:nttp-misuse:$cxx:$style" -- \
      "$cxx" -std=c++23 -O0 -c "-DSPIKE_$style" -DSPIKE_MISUSE \
      -fno-diagnostics-color -o /dev/null "$here/nttp.cpp" \
      2>"$out/$tag.misuse.txt"
    printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' "$cxx" "$style" "${#sym}" \
      "$(wc -c <"$out/$tag.demangled")" \
      "$(cut -d' ' -f1 "$out/$tag.all-gp" | paste -sd,)" "$(wc -l <"$out/$tag.misuse.txt")" \
      "$(wc -c <"$out/$tag.misuse.txt")" \
      "$(awk '{ if (length > m) m = length } END { print m }' "$out/$tag.misuse.txt")" \
      >>"$summary"
  done
done
column -t -s $'\t' "$summary"
