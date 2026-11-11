// A source for the --alone mode that defines first in two paragraphs, two
// overloads, so that no one unit can hold first alone.
int helper = 0;

int first() { return helper; }

int first(int k) { return helper + k; }
