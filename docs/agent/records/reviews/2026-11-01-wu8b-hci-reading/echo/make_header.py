"""Writes echo/include/: the branch's headers with a hint on the line of each
converting constructor, the line GCC and Clang echo when copy-initialization
skips an explicit constructor. Usage, from this directory:
python3 -I make_header.py <branch include directory>
then compile ../user.cpp with -I include and -DCASE=1, 3, or 5."""
import shutil
import sys
from pathlib import Path

here = Path(__file__).resolve().parent
shutil.rmtree(here / "include", ignore_errors=True)
shutil.copytree(Path(sys.argv[1]), here / "include")
p = here / "include/galoot/core/mv.hpp"
s = p.read_text()
a = "  constexpr explicit mv(const mv<Alg, From, T>& source) {"
c = "  constexpr explicit mv(const mv<Alg, From, T>& /*source*/) {"
assert a in s and c in s
s = s.replace(a, a + " // explicit: Type{x}")
s = s.replace(c, c + " // lossy: truncate<Type>(x)")
p.write_text(s)
