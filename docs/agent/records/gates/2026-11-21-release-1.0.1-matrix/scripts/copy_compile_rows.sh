copy_compile_rows() {
  local tree=$1 record=$2 rows revisions repeated
  refuse() { echo "copy refused: $*" >&2; return 1; }
  [ -f "$tree/compile.tsv" ] || { refuse "$tree/compile.tsv does not exist"; return 1; }
  [ -f "$record" ] || { refuse "$record does not exist"; return 1; }
  tools/measure read "$tree/compile.tsv" > /dev/null || { refuse "tools/measure read rejects $tree/compile.tsv"; return 1; }
  cmp -s <(head -n 1 "$tree/compile.tsv") <(head -n 1 "$record") || { refuse "its header is not the committed record's"; return 1; }
  rows=$(tail -n +2 "$tree/compile.tsv" | wc -l)
  [ "$rows" -gt 0 ] || { refuse "$tree/compile.tsv has no rows"; return 1; }
  revisions=$(tools/measure read "$tree/compile.tsv" revision | tail -n +2 | sort -u)
  [ "$(printf '%s\n' "$revisions" | wc -l)" = 1 ] || { refuse "rows of more than one revision: $(echo $revisions)"; return 1; }
  case $revisions in
    unavailable) refuse "the revision is unavailable"; return 1 ;;
    *-dirty) refuse "the revision $revisions has uncommitted changes"; return 1 ;;
  esac
  repeated=$(tools/measure read "$tree/compile.tsv" preset source object | tail -n +2 | sort | uniq -d)
  [ -z "$repeated" ] || { refuse "a compile appears more than once (the tree was rebuilt): $(echo $repeated)"; return 1; }
  tail -n +2 "$tree/compile.tsv" >> "$record"
  tools/measure read "$record" > /dev/null || { refuse "after appending, tools/measure read rejects $record; restore it with git checkout"; return 1; }
  echo "copied $rows rows of revision $revisions" >&2
  rm -- "$tree/compile.tsv"
}
