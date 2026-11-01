// is_null with an index past the last generator, at run time. Built with
// GALOOT_CHECKING=1, it must abort with the message (core.metric_is_null).
#include <cstddef>

#include <galoot/core/metric.hpp>

int main(int argc, char** /*argv*/) {
  // rigid(3) has 4 generators, and argc is at least 1, so the index is at
  // least 4; the compiler cannot know it, so the check happens at run time.
  const auto rigid3 = galoot::core::rigid<3>();
  return rigid3.is_null(static_cast<std::size_t>(argc) + 3) ? 1 : 0;
}
