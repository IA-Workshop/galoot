"""Stands in for a compiler in the tests of kernels_inline.py --alone.

Usage: python3 -I stand_in_compiler.py [<flag>...] <unit>

Reads the unit, the last argument, and writes to standard output a
definition for each of first and second that the unit defines: one that
calls nothing of the library for first, and one that calls the library for
second, or for first when the unit also defines second, so that a unit that
is not alone fails. Exits 1, naming it, if the unit lacks the source's
common text.
"""

import sys

text = open(sys.argv[-1], encoding="utf-8").read()
if "int helper = 0;" not in text:
    sys.exit("the unit lacks the source's common text")
both = "int first()" in text and "int second()" in text
print("\t.text")
if "int first()" in text:
    print("_Z5firstv:")
    if both:
        print("\tcall\t_ZN6galoot4core3fooEv")
    print("\tmovl\t$0, %eax")
    print("\tret")
if "int second()" in text:
    print("_Z6secondv:")
    print("\tcall\t_ZN6galoot4core3barEv")
    print("\tret")
