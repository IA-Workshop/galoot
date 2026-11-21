#!/usr/bin/env bash
# usage: compare.sh <dir A> <dir B>: compares two evidence trees file by file.
set -u
a=$1; b=$2; status=0
for f in $(cd "$a" && find . -type f | sort); do
  if cmp -s "$a/$f" "$b/$f"; then echo "same      $f"; else echo "DIFFERENT $f"; status=1; fi
done
for f in $(cd "$b" && find . -type f | sort); do
  [ -e "$a/$f" ] || { echo "ONLY IN B $f"; status=1; }
done
exit $status
