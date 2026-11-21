"""Show that inventory_check.py fails on three broken copies of the inventory.

Each mutant changes one row of ../concept-inventory.md (or the path given):
a row made to rest on a row below it, a row made to rest on an ID that no row
has, and K204 put back as it read before the word "blade" was defined there,
so that K204 and K207 use the word above K210. The script runs the check on
the inventory itself and on each mutant, and fails unless the inventory
passes and every mutant fails with the expected error.

Run: python3 -I inventory_mutants.py [path]
"""

import contextlib
import io
import pathlib
import sys
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import inventory_check  # noqa: E402

K204_BEFORE = ("| K204 | The basis bivectors e12, e13, e23, each the wedge of two "
               "generators; a blade's name lists its generators | term | K202, K101 | |")


def replace_row(text, ident, new_row):
    lines = text.splitlines()
    hits = [i for i, line in enumerate(lines) if line.startswith(f"| {ident} |")]
    assert len(hits) == 1, ident
    lines[hits[0]] = new_row
    return "\n".join(lines) + "\n"


def change_rests(text, ident, rests):
    row = next(line for line in text.splitlines() if line.startswith(f"| {ident} |"))
    cells = row.strip().strip("|").split("|")
    cells[3] = f" {rests} "
    return replace_row(text, ident, "|" + "|".join(cells) + "|")


def run(path):
    out = io.StringIO()
    with contextlib.redirect_stdout(out):
        status = inventory_check.main(["inventory_check.py", str(path)])
    return status, [l for l in out.getvalue().splitlines() if l.startswith("ERROR")]


def main(argv):
    here = pathlib.Path(__file__).resolve().parent
    path = pathlib.Path(argv[1]) if len(argv) > 1 else here.parent / "concept-inventory.md"
    text = path.read_text(encoding="utf-8")
    mutants = [
        ("K204 rests on K210, a later row", change_rests(text, "K204", "K202, K101, K210"),
         "K204 rests on K210, which comes later"),
        ("K305 rests on K299, which has no row", change_rests(text, "K305", "K304, K299, K105"),
         "K305 rests on K299, which has no row"),
        ("K204 as it read before finding 1 was corrected", replace_row(text, "K204", K204_BEFORE),
         "K204 uses 'blade' before K210"),
    ]
    failures = 0
    status, errors = run(path)
    print(f"inventory itself: {'passes' if status == 0 else 'FAILS'}")
    failures += status != 0
    with tempfile.TemporaryDirectory() as tmp:
        for name, mutated, expected in mutants:
            copy = pathlib.Path(tmp) / "concept-inventory.md"
            copy.write_text(mutated, encoding="utf-8")
            status, errors = run(copy)
            caught = status != 0 and any(expected in e for e in errors)
            failures += not caught
            print(f"mutant, {name}: {'fails as expected' if caught else 'NOT CAUGHT'}")
            for e in errors:
                print("  " + e)
    print("all mutants caught" if not failures else f"{failures} problems")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
