import sys, pathlib
for f in sys.argv[1:]:
    p = pathlib.Path(f); s = p.read_text()
    s = s.replace("#include <cstdint>\n", "#include <cstdint>\n#include <cstdio>\n", 1)
    old = """  for (auto& c : x.coefficients) {
    c = digit(source);
  }
  return x;"""
    new = """  for (auto& c : x.coefficients) {
    c = digit(source);
  }
  for (const auto& c : x.coefficients) {
    std::fprintf(stderr, "%lld ", static_cast<long long>(c));
  }
  std::fprintf(stderr, "| %zu\\n", static_cast<std::size_t>(X::component_count));
  return x;"""
    assert s.count(old) == 1, f
    p.write_text(s.replace(old, new))
    print("patched", f)
