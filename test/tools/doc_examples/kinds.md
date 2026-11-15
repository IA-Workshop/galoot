# A page with every kind of block

A program, compiled and run:

```cpp
int main() {}
```

A program that must not compile, with the message it must fail with:

```cpp rejected
// rejected: a fixture message
static_assert(false, "a fixture message");
```

A consumer project:

```cmake
cmake_minimum_required(VERSION 4.4)
```

A command, which is not run:

```text
cmake --build build
```
