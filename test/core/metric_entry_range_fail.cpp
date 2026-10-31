// A Gram matrix with an entry beyond max_form_entry is rejected with a
// message that says so (core.metric_entry_range). This file must not compile.
#include <galoot/core/metric.hpp>

namespace {

struct huge_entry {
  static constexpr auto value = galoot::core::from_gram({{32768}});
};

} // namespace
