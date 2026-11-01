constexpr int f(int x) { return x == 0 ? 1 : throw 0; }
inline int g() { return f(1); }
