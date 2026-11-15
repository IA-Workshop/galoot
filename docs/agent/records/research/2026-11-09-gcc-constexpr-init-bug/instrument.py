"""Instruments a scratch checkout for init_probe.hpp.

Appends `::galoot_init_probe::check(name, __FILE__, __LINE__);` to each line
under include/ and test/ that declares a local `T name{};` or `T name = {};`
(not constexpr), on the same line, so that line numbers do not change.
Prints each site as path:line. Lines named in the exclusion file (path:line,
one per line) are left alone; they are declarations at class scope, where a
statement cannot go.

With --kernels, the check in the library's run-time kernels run_map and
run_product (mv.hpp) and run_sum (linear.hpp) becomes check_kernel or
check_kernels, which also report the damaged places that no term writes.

Usage: python3 -I instrument.py [--kernels] <checkout> [<exclusions>]
"""

import pathlib
import re
import sys

DECLARATION = re.compile(
    r"^(?P<indent>\s{2,})(?!return\b|case\b|using\b|throw\b|co_return\b)"
    r"(?P<decl>(?:(?:const|static|alignas\([^)]*\))\s+)*"
    r"[A-Za-z_][\w:<>,\s*&().+\-]*?[\w>\]]\s+"
    r"(?P<name>[A-Za-z_]\w*)\s*(?:\{\}|=\s*\{\})\s*;)\s*(?P<comment>//.*)?$"
)


KERNEL_CHECK = (
    "std::array<T, R> out{}; ::galoot_init_probe::check(out, __FILE__, "
    "__LINE__);"
)
KERNELS = {
    "include/galoot/core/mv.hpp": (
        "std::array<T, R> out{}; ::galoot_init_probe::check_kernel<Kernel, R>("
        "out, __FILE__, __LINE__, std::index_sequence<K...>{});"
    ),
    "include/galoot/core/linear.hpp": (
        "std::array<T, R> out{}; ::galoot_init_probe::check_kernels<KX, KY, R>("
        "out, __FILE__, __LINE__, std::index_sequence<I...>{}, "
        "std::index_sequence<J...>{});"
    ),
}


def patch_kernels(root: pathlib.Path) -> None:
    for relative, replacement in KERNELS.items():
        path = root / relative
        text = path.read_text()
        count = text.count(KERNEL_CHECK)
        path.write_text(text.replace(KERNEL_CHECK, replacement))
        print(f"kernels: {relative}: {count}", file=sys.stderr)


def main() -> int:
    arguments = sys.argv[1:]
    kernels = arguments[:1] == ["--kernels"]
    if kernels:
        arguments = arguments[1:]
    root = pathlib.Path(arguments[0]).resolve()
    excluded = set()
    if len(arguments) > 1:
        excluded = {
            line.strip()
            for line in pathlib.Path(arguments[1]).read_text().splitlines()
            if line.strip()
        }
    for top in ("include", "test"):
        for path in sorted((root / top).rglob("*")):
            if path.suffix not in (".hpp", ".cpp") or not path.is_file():
                continue
            relative = path.relative_to(root).as_posix()
            lines = path.read_text().split("\n")
            changed = False
            for i, line in enumerate(lines):
                match = DECLARATION.match(line)
                if match is None or "constexpr" in match["decl"]:
                    continue
                site = f"{relative}:{i + 1}"
                if site in excluded:
                    continue
                lines[i] = (
                    f"{match['indent']}{match['decl']} "
                    f"::galoot_init_probe::check({match['name']}, __FILE__, "
                    f"__LINE__);"
                )
                changed = True
                print(site)
            if changed:
                path.write_text("\n".join(lines))
    if kernels:
        patch_kernels(root)
    return 0


if __name__ == "__main__":
    sys.exit(main())
