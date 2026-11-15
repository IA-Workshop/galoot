[[gnu::error("error attribute reached")]] void forbidden();
void caller() { forbidden(); }
