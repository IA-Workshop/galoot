# 0066 GCC 16 drops `explicit(bool)` from an inherited constructor template that reads its own parameters

- Date: 2026-11-02
- Unit: WU8 (plan 0016), found by the HCI reading, finding F15

## What happened

A class derived from `mv` with `using mv::mv;` inherits a constructor whose `explicit(sizeof...(V) == 1)` reads the constructor's own template parameters. On GCC 16.2.1 the inherited constructor is treated as not explicit: `std::is_convertible_v<double, Derived>` is true and `Derived d = 2.0;` compiles. Clang 23.1.4 keeps `explicit`.

## Scope and handling

Plain `explicit`, a non-template constructor, and a condition on the class's own parameters are kept, so `mv` itself is unaffected; only classes derived from `mv` are. This is one reason named model classes (F6) are deferred to WU16. No upstream report is made (agents do not contact maintainers). The knowledge base's `upstream-awareness.md` records it. Any later work on derived model types needs an `is_convertible` probe on both compilers.
