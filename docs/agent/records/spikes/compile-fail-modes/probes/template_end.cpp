template <class T> int h() {
  static_assert(sizeof(T) == 0, "instantiated at the end of the unit");
  return 0;
}
int k();
int k() { return h<int>(); }
