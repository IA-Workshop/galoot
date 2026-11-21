// canonical_name of a blade beyond e9 at run time, unchecked: no undefined
// behavior, but the name silently drops the generators it cannot spell.
#include <cstdio>
#include <galoot/core/blade_set.hpp>
int main(int argc, char**) {
  const auto mask = static_cast<galoot::core::mask_t>(0x201U << (argc - 1)); // e1 e10
  const auto name = galoot::core::canonical_name(mask);
  std::printf("canonical_name(e1 e10) = \"%.*s\"\n",
              static_cast<int>(name.text().size()), name.text().data());
  const auto only_e10 = galoot::core::canonical_name(mask & ~1U);
  std::printf("canonical_name(e10) = \"%.*s\"\n",
              static_cast<int>(only_e10.text().size()), only_e10.text().data());
  return 0;
}
