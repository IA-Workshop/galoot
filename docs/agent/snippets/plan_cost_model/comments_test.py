"""Tests of `cost.py comments`: which counts in a header's /// comments it
accepts. Standard library only; run as `python3 -I comments_test.py`.
"""

import contextlib
import importlib.util
import io
import pathlib
import sys
import tempfile
import unittest

HERE = pathlib.Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("cost", HERE / "cost.py")
cost = importlib.util.module_from_spec(spec)
spec.loader.exec_module(cost)

FIGURES = (
    "# Predicted thresholds from weights.tsv\n"
    "label\tT\tR\tlargest phase\tthreshold\tnext phase\tnext threshold\t"
    "covered\n"
    "gp-r-5\t1024\t32\tfill\t3611426\tcount\t3528441\tyes\n"
)
SOURCE = "records/research/2026-11-09-scalar-full10-gcc.md"


def run(comment, non_cost=None):
    """Runs `comments` on a header whose only comment line is `comment`;
    returns (result, printed output)."""
    with tempfile.TemporaryDirectory() as tmp:
        header = pathlib.Path(tmp) / "h.hpp"
        header.write_text(f"/// {comment}\nint x;\n", encoding="utf-8")
        figures = pathlib.Path(tmp) / "figures.txt"
        figures.write_text(FIGURES, encoding="utf-8")
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            if non_cost is None:
                result = cost.comments(str(header), [str(figures)])
            else:
                result = cost.comments(str(header), [str(figures)],
                                       non_cost=non_cost)
        return result, out.getvalue()


class CommentsTest(unittest.TestCase):
    def test_figures_row_count_passes(self):
        result, out = run("The product needs 3,611,426 operations.", [])
        self.assertTrue(result, out)

    def test_listed_count_with_source_passes(self):
        result, out = run("1,024 terms on 4096 blades.",
                          [(1024, SOURCE), (4096, SOURCE)])
        self.assertTrue(result, out)

    def test_unlisted_count_fails_and_is_named(self):
        result, out = run("1,024 terms on 4096 blades.", [(1024, SOURCE)])
        self.assertFalse(result)
        self.assertIn("4096", out)
        self.assertNotIn(": 1,024", out)

    def test_count_in_words_fails_unless_listed(self):
        result, out = run("It needs about 89 million operations.", [])
        self.assertFalse(result)
        self.assertIn("89 million", out)
        result, out = run("It needs about 89 million operations.",
                          [(89_000_000, SOURCE)])
        self.assertTrue(result, out)

    def test_entry_without_source_is_refused(self):
        for source in ("", None, "   "):
            with self.assertRaises(ValueError):
                run("1,024 terms.", [(1024, source)])


if __name__ == "__main__":
    unittest.main()
