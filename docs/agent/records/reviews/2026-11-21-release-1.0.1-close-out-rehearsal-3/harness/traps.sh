#!/usr/bin/env bash
set -euo pipefail
RECOVER="close-out: stopped in step 5; the comment at the top of SELF says how to recover"
STEP5=
fail() { echo "close-out: $*" >&2; [ -z "$STEP5" ] || echo "$RECOVER" >&2; exit 1; }
case $1 in
  guarded-sub-fail) x=$(sh -c 'echo "fatal: git-like error" >&2; exit 128') || fail "no FILE at main"; echo "not reached";;
  guarded-sub-pass) x=$(echo content) || fail "no FILE at main"; echo "passed: x=$x";;
  unguarded-sub-fail) x=$(sh -c 'exit 128'); echo "not reached";;
esac
STEP5=1
trap 'echo "$RECOVER" >&2' ERR
trap 'echo "$RECOVER" >&2; exit 130' INT TERM
case $1 in
  step5-unguarded) sh -c 'echo "fatal: a git command failed" >&2; exit 128';;
  step5-fail) [ a = b ] || fail "master's first parent is not 1.0.0";;
  step5-sub-in-test) [ "$(sh -c 'exit 128')" = x ] || fail "a comparison";;
  step5-sleep) sleep 3;;
esac
trap - ERR INT TERM
STEP5=
echo "done"
[ "$1" != after-success-fail ] || sh -c 'exit 3'
echo "end"
