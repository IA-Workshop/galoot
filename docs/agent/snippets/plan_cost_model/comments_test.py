"""Tests of `cost.py comments`: which counts in a header's /// comments, or
in one named section of a Markdown page, it accepts, and which pages it
refuses. Standard library only; run as `python3 -I comments_test.py`.
"""

import contextlib
import importlib.util
import io
import pathlib
import subprocess
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


SECTION = "Steps and operations of each plan"


def page(section_body, before="", after="", heading=SECTION):
    """A limits page: an unrelated section, the section `heading` at level 3
    holding `section_body`, then a sibling section holding `after`."""
    return ("# Limits\n\n"
            "## Compile-time limits: products and plans\n\n"
            f"{before}\n\n"
            f"### {heading}\n\n"
            f"{section_body}\n\n"
            "### Another subsection\n\n"
            f"{after}\n\n"
            "## Raising the limits\n\n"
            "Pass -fconstexpr-steps=1200000 to Clang.\n")


def run_page(text, section=SECTION, non_cost=()):
    """Runs `comments` on a Markdown page with the text `text`, reading the
    section `section`; returns (result, printed output)."""
    with tempfile.TemporaryDirectory() as tmp:
        md = pathlib.Path(tmp) / "limits.md"
        md.write_text(text, encoding="utf-8")
        figures = pathlib.Path(tmp) / "figures.txt"
        figures.write_text(FIGURES, encoding="utf-8")
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            result = cost.comments(str(md), [str(figures)], non_cost=non_cost,
                                   section=section)
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


class SectionTest(unittest.TestCase):
    def test_figures_count_in_the_section_passes(self):
        result, out = run_page(page("- n = 5: 3,611,426 operations on GCC."))
        self.assertTrue(result, out)
        self.assertIn("0 counts", out)

    def test_unlisted_count_in_the_section_fails_and_is_named(self):
        result, out = run_page(page("- n = 5: 3,611,426 and 3,611,427."))
        self.assertFalse(result)
        self.assertIn("limits.md:9: 3,611,427", out)
        self.assertNotIn(": 3,611,426", out)

    def test_count_in_a_deeper_heading_of_the_section_is_read(self):
        body = "3,611,426 operations.\n\n#### On GCC\n\n4,096 blades."
        result, out = run_page(page(body))
        self.assertFalse(result)
        self.assertIn(": 4,096", out)

    def test_unlisted_count_outside_the_section_passes(self):
        result, out = run_page(page("3,611,426 operations.",
                                    before="It takes 4,096 blades.",
                                    after="And 8,192 more."))
        self.assertTrue(result, out)

    def test_heading_in_a_code_block_does_not_end_the_section(self):
        body = ("```cpp\n"
                "# define NOT_A_HEADING 1\n"
                "```\n\n"
                "4,096 blades.")
        result, out = run_page(page("3,611,426 operations.\n\n" + body))
        self.assertFalse(result)
        self.assertIn(": 4,096", out)

    def test_listed_non_cost_count_in_the_section_passes(self):
        result, out = run_page(page("4,096 blades."),
                               non_cost=[(4096, SOURCE)])
        self.assertTrue(result, out)

    def test_page_without_the_section_is_refused(self):
        text = page("3,611,426 operations.", heading="Steps of each plan")
        with self.assertRaises(ValueError) as caught:
            run_page(text)
        self.assertIn(f"has no section {SECTION!r}", str(caught.exception))

    def test_section_named_twice_is_refused(self):
        text = page("3,611,426 operations.") + f"\n### {SECTION}\n\n1.\n"
        with self.assertRaises(ValueError):
            run_page(text)

    def test_section_without_a_count_is_refused(self):
        with self.assertRaises(ValueError):
            run_page(page("The counts moved to another page."))

    def test_markdown_page_without_a_section_is_refused(self):
        with self.assertRaises(ValueError):
            run_page(page("3,611,426 operations."), section=None)

    def test_command_line_reads_the_section_and_refuses_a_missing_one(self):
        with tempfile.TemporaryDirectory() as tmp:
            md = pathlib.Path(tmp) / "limits.md"
            md.write_text(page("3,611,426 operations.", after="8,192."),
                          encoding="utf-8")
            figures = pathlib.Path(tmp) / "figures.txt"
            figures.write_text(FIGURES, encoding="utf-8")

            def command(section):
                return subprocess.run(
                    [sys.executable, "-I", str(HERE / "cost.py"), "comments",
                     str(md), "--section", section, str(figures)],
                    capture_output=True, text=True, check=False)

            found = command(SECTION)
            self.assertEqual(found.returncode, 0, found.stdout + found.stderr)
            missing = command("No such heading")
            self.assertEqual(missing.returncode, 1)
            self.assertIn("has no section 'No such heading'", missing.stderr)


if __name__ == "__main__":
    unittest.main()
