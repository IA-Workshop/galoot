// translation over bool, which is not a ring scalar: the rejection that
// fires is the integer one, whose message says halving would truncate.
#include <galoot/presets/rga3.hpp>
[[maybe_unused]] const auto m =
    galoot::presets::rga3::translation(true, false, true);
