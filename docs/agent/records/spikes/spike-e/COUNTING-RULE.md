# The counting rule

This rule is prose on purpose. The reference counts of part C and the counting scalar of part C each follow it separately, and neither reads the other's code or this directory's scripts. `derive.py` follows it, in two programs that share nothing but the nodes of a form (counter 1 tallies nodes, counter 2 runs generated text on a number that counts the operators it meets), and in a third, `plans.py`, for the library's kernels.

## 1. What is counted

An operation is counted when it is applied to values of the scalar type while one object, or one motor, is processed. Reading a coefficient, copying, converting, indexing, branching, and constructing a zero or an integer constant are not counted. In checking mode the comparisons of the exact check are counted apart, as `compare`.

Five classes:

| Class | An operation of this class is |
| --- | --- |
| `mul` | the product of two values, neither a literal constant |
| `add` | the sum or the difference of two values (a subtraction is an addition; rule K also lists the subtractions apart, as `sub`) |
| `neg` | a unary minus that is evaluated |
| `scl` | a doubling, the product with the literal 2, which is one exact addition x + x; it is listed apart and is not in `add` |
| `compare` | an equality or order comparison (checking mode only) |

A product with a literal +1 or -1 is not a `mul`: it is a sign, carried with the factor. The kernels of form A have no other coefficient. Forms B and C have +1, -1, and 2.

A value computed once and read by several results is counted once, as its operation. Two products of the same two values are one product, in either order.

## 2. Rule K: the library's kernels (form A)

Form A is what the transform kernels evaluate. A kernel is a list of terms in order, each term a product of one coefficient of each operand with a sign +1 or -1 and an output place. Per output place, in the order of the list:

- the first term assigns its product, or assigns the negated product when its sign is -1 (one `neg`);
- each later term adds its product when its sign is +1 and subtracts it when -1 (one `add` or one `sub`);
- every term is one `mul`.

So a kernel's counts are `mul` = number of terms; `add` + `sub` = number of terms minus the number of outputs it writes; `neg` = number of outputs whose first term has sign -1. An output no term reaches is not written and costs nothing.

Terms come in the order of the first operand's position, then the second operand's position, as the plan lists them. The sign of a term folds in every orientation of the layouts (the oriented names: e41 = -e14 and so on), the antireverse's sign of the versor's blade, and the plan's own sign, so no further operation is counted for them.

A transform has two kernels. Stage 1 is V ⟇ x restricted to the terms whose output stage 2 reads. Stage 2 is the middle times V (with the antireverse folded in) restricted to the terms that land on blades of x, written at x's positions with x's orientation folded in. With checking on and an exact scalar, stage 1 is every term of V ⟇ x, and the terms that land on the dropped blades are computed as a third kernel, then each dropped coefficient is compared once with zero. The counts of the first and the checking-on case are both tabulated.

## 3. Rule S: forms B and C (and A, for comparison)

Rule S is the count of a straight-line program that a careful person would write, in which a sign costs nothing while it can be carried.

- A sum of k terms costs k - 1 `add`. Each term has a sign. The sum is ordered so that it costs no `neg`: a subtraction takes the negative operand, and a sum whose terms are all negative is carried as the negative of the sum of their magnitudes.
- A carried sign is folded into the next product or sum that reads the value. It costs an operation only where a result is the final output with a negative carried sign, as in -(a + b): one `neg`. A negated difference is not a `neg`: -(a - b) is b - a.
- A product is one `mul` and has the sign of the product of its factors.
- A doubling is one `scl`. A power-of-two scale is exact, and it may be applied to a sum's result, to a factor before the products, or to a motor coefficient once for many objects; each of these is counted where it is made.

Form A under rule S is the same terms as form A under rule K with every sign carried: it has `neg` = 0 for the transforms, since each output has a positive term. Rule K is what the library does; rule S is the common ground on which A, B, and C are compared.

## 4. Per call, per motor, per object

A form's count per call is the count of all its operations for one object and one motor, shared values counted once. For a batch of objects under one motor, the operations that read no coefficient of the object are the motor-only part, counted once per motor; the operations that read the object are counted for each object. A form with no motor-only part has per-object = per-call. The sum of the two parts can exceed the per-call count only by the multiply-adds that cross the boundary (a motor-only product read by an object sum); these are counted unfused in both parts, which is what a batch does.

Form B's motor-only part builds the matrix of quadratic entries: each distinct product of two motor coefficients once, each entry as a sum of those products. Its per-object part is the product of the matrix with the object's coefficients, one `mul` for each nonzero entry.

## 5. Multiply-adds, for the comparison with published counts

A published count of "multiply-adds" counts a product and the sum that takes it as one unit. The rule here: a sum is the tree of additions that only each other read, whose leaves are the values the tree reads; if p of the leaves are products that nothing else reads, the sum can be run as a chain in which min(additions, p) additions each absorb one such product. Then

MA = mul + add - fused, where fused is the sum of those minima over all sums.

A dot product of two 3-vectors is 3 MA (3 `mul`, 2 `add`, 2 fused). A cross product is 6 MA (6 `mul`, 3 `add`, 3 fused). Both are checked in `derive.py` by hand-written assertions. MA is reported without `scl` and with it (`MA+scl`), since a published count may or may not treat the doubling as a unit. Galoot builds with `-ffp-contract=off`, so no fused instruction is ever used; MA exists only to compare with another unit and is not a prediction of an instruction count.

## 6. What the rule does not settle

- The order in which the terms of a sum are added changes rounding, not counts.
- Hardware counts (the compilers' vectorization and constant folding) differ from these counts: they are measured in other parts of the plan.
- A minimum over all evaluation orders is not searched. Each form is one program; the counts are upper bounds on the best program of that form, not lower bounds on the operation.
