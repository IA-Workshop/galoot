#!/usr/bin/env bash
# Runs each old test case alone in the old and new instrumented programs and
# compares the drawn coefficients.
S=$1
for seed in 0 12345; do
  for prog in core_identities_test core_dual_identities_test; do
    while IFS= read -r tc; do
      old=$S/draws/old-$prog-$seed-$(echo "$tc" | md5sum | cut -c1-8).txt
      new=${old/old-/new-}
      mkdir -p $S/draws
      $S/bp/build/gcc-debug/test/$prog --rand-seed=$seed "--test-case=$tc" 2>$old >/dev/null; eo=$?
      $S/tipd/build/gcc-debug/test/$prog --rand-seed=$seed "--test-case=$tc" 2>$new >/dev/null; en=$?
      o=$(wc -l <$old); n=$(wc -l <$new)
      if cmp -s $old <(head -n $o $new); then pre=prefix; else pre=DIFFERENT; fi
      echo -e "$seed\t$prog\t$tc\told=$o\tnew=$n\textra=$((n-o))\t$pre\texit=$eo/$en"
    done < $S/cases-$prog.txt
  done
done
