// A source for the --alone mode whose two functions each inline when they
// are alone: text that every unit needs, then first and third, each in a
// paragraph of its own.
int helper = 0;

int first() { return helper; }

int third() {
  const int base = helper;
  return base + 3;
}
