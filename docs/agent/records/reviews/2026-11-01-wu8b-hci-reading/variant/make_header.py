"""Writes variant/include/: the branch's headers, with mv.hpp's catch-alls
behind macros, so that run.py can compare the branch with catch-alls that
copy-initialization reaches. Usage, from this directory:
python3 -I make_header.py <branch include directory>"""
import shutil
import sys
from pathlib import Path

here = Path(__file__).resolve().parent
shutil.rmtree(here / "include", ignore_errors=True)
shutil.copytree(Path(sys.argv[1]), here / "include")
p = here / "include/galoot/core/mv.hpp"
s = p.read_text()
old_lossy = """  template <blade_set From>
    requires(!detail::holds_every_mask(Blades, From))
  constexpr explicit mv(const mv<Alg, From, T>& /*source*/) {"""
assert old_lossy in s
s = s.replace(old_lossy, old_lossy.replace("explicit", "VARIANT_LOSSY_EXPLICIT"))
anchor = "  /// Rejects any other number of values"
extra = """#if VARIANT_IMPLICIT_LOSSLESS
  template <class Source>
    requires(detail::is_multivector_v<Source> &&
             !std::same_as<Source, mv> &&
             std::same_as<typename Source::algebra_type, Alg> &&
             std::same_as<typename Source::scalar_type, T> &&
             detail::holds_every_mask(Blades, Source::layout))
  constexpr mv(const Source& /*source*/) {
    static_assert(false, "mv: a conversion between layouts is explicit, "
                         "though this one drops nothing; write Type{x}");
  }
#endif

"""
assert anchor in s
s = s.replace(anchor, extra + anchor, 1)
old_arr = "  constexpr explicit mv(const std::remove_cvref_t<T> (& /*values*/)[N]) {"
assert old_arr in s
s = s.replace(old_arr, old_arr.replace("explicit", "VARIANT_ARRAY_EXPLICIT"))
s = s.replace("namespace galoot::core {", """#ifndef VARIANT_LOSSY_EXPLICIT
#define VARIANT_LOSSY_EXPLICIT explicit
#endif
#ifndef VARIANT_ARRAY_EXPLICIT
#define VARIANT_ARRAY_EXPLICIT explicit
#endif
#ifndef VARIANT_IMPLICIT_LOSSLESS
#define VARIANT_IMPLICIT_LOSSLESS 0
#endif
namespace galoot::core {""", 1)
p.write_text(s)
