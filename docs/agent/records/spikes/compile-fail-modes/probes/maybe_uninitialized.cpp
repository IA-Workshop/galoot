int g(int);
int f(int c) {
  int x;
  if (c > 3) {
    x = g(c);
  }
  return g(c) + (c > 5 ? x : 0);
}
