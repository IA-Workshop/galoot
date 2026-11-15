"""Stands in for a compiler in the tests of kernels_inline.py --alone.

Usage: python3 -I stand_in_compiler.py [<flag>...] <unit>

Reads the unit, the last argument, and writes to standard output a
definition for each of first, second and third that the unit defines.
second always calls the library. first and third call it only when the
unit defines another of the three, so that a unit that is not alone fails.
Exits 1, naming it, if the unit lacks the sources' common text.
"""

import sys

text = open(sys.argv[-1], encoding="utf-8").read()
if "int helper = 0;" not in text:
    sys.exit("the unit lacks the source's common text")
names = [name for name in ("first", "second", "third")
         if f"int {name}()" in text]
print("\t.text")
for name in names:
    print(f"_Z{len(name)}{name}v:")
    if name == "second" or len(names) > 1:
        print(f"\tcall\t_ZN6galoot4core3fooEv")
    print("\tmovq\t%rdi, %rax")
    print("\tret")
