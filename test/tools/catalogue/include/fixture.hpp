// Messages for tools.catalogue; never compiled. Line numbers matter: the
// test names each message by its line, as the tool reports it.
#if 0
#error "the directive"
#endif
#define GALOOT_ASSERT(condition, message) "a macro definition is skipped"
static_assert(a, "first: required in the second place");
static_assert(b, "second: holds; a semicolon");
static_assert(c, "third: only forbidden");
static_assert(d, "fourth: only in a test that is meant to fail");
void f() { GALOOT_ASSERT(x > 0, "fifth: run time"); }
static_assert(e(")"), "sixth: joined "
                      "from two literals");
static_assert(g, "seventh: shares a prefix");
static_assert(h, "seventh: shares a prefix too");
static_assert(sizeof(int) == 4);
// static_assert(false, "a comment is skipped");
void g() { GALOOT_ASSERT(y, "eighth: cut short at run time"); }
static_assert(i, "dot: nested");
static_assert(j, "antidot: nested");
static_assert(n, "the directive");
