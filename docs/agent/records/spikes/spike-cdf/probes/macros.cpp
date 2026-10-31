// Spike C probe: the feature-test macros for user-generated static_assert
// messages (__cpp_static_assert) and deleted-function reasons
// (__cpp_deleted_function). run_probes.sh preprocesses it with -E -P.
#include <version>
long sa = __cpp_static_assert;
#ifdef __cpp_deleted_function
long df = __cpp_deleted_function;
#else
long df = 0;
#endif
