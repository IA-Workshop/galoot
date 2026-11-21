// Tag each operator; the parse tree tells which binds tighter.
#include <string>
#include <cstdio>
struct E { std::string s; };
E operator^(E a, E b) { return {"(" + a.s + "^" + b.s + ")"}; }
E operator&(E a, E b) { return {"(" + a.s + "&" + b.s + ")"}; }
E operator+(E a, E b) { return {"(" + a.s + "+" + b.s + ")"}; }
E operator*(E a, E b) { return {"(" + a.s + "*" + b.s + ")"}; }
E operator==(E a, E b) { return {"(" + a.s + "==" + b.s + ")"}; }
int main() {
  E a{"a"}, b{"b"}, c{"c"};
  std::puts((a ^ b == c).s.c_str());
  std::puts((a & b == c).s.c_str());
  std::puts((a ^ b + c).s.c_str());
  std::puts((a & b * c).s.c_str());
  std::puts((a ^ b & c).s.c_str());
}
