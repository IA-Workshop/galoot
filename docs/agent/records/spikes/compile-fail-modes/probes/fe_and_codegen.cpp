[[gnu::error("error attribute reached")]] void forbidden();
void caller() { forbidden(); }
static_assert(sizeof(int) == 0, "front-end failure");
