// metric::is_null with an index past the last generator, at run time,
// unchecked: an out-of-bounds read. Build with -D_GLIBCXX_ASSERTIONS or
// -fsanitize=address to see it.
#include <cstddef>
#include <cstdio>
#include <galoot/core/metric.hpp>
int main(int argc, char**) {
  const auto rigid3 = galoot::core::rigid<3>();
  const auto i = static_cast<std::size_t>(argc) + 3;  // 4, past e4
  std::printf("is_null(4) = %d\n", static_cast<int>(rigid3.is_null(i)));
  return 0;
}
