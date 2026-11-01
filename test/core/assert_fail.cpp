// A failing GALOOT_ASSERT at run time. Built with
// GALOOT_CHECKING=1 it must abort with the message (core.assert_checking);
// built with GALOOT_CHECKING=0 it must exit 0 (core.assert_unchecked).
#include <galoot/core/assert.hpp>

int main(int argc, char** /*argv*/) {
  // argc is at least 1 when the program is started by name, and the compiler
  // cannot know its value, so the check happens at run time.
  GALOOT_ASSERT(argc == 0, "assert_fail: argc is never zero");
  return 0;
}
