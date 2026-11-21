# The learner contracts of the user documentation

Written by: Claude (agent, Fable 5.2, math-expert and hci-expert roles), 2026-11-21, for unit U1 of plan 0049. Not yet reviewed.

A learner contract is what a page declares about its reader: the level, what the reader is assumed to know, and what the reader will be able to do afterward. An author writes to the contract, the cold reader of plan 0049 section 9.1 is given the contract and nothing more, and a stumble is settled by changing the page or by changing the contract in the open. This record fixes the readers, the contract of each kind of page, the contract of each tutorial chapter, and the form in which a page shows its contract.

Decision 0253 item 33 rules that pages carry no YAML front matter: the contract is a visible "Before you start" section. Section 5 gives its form.

## 1 The readers

Plan 0049 section 2 names two readers. Writing the first programs of each chapter showed a third, whom the plan folds into the researcher and whose needs differ: the reader who arrives with another convention. This reader is the one the `transform` trap of research note 0029 catches.

| Reader | Knows | Does not know | Wants | Starts at |
| --- | --- | --- | --- | --- |
| The newcomer | C++ as section 2 lists it; school vectors and trigonometry | geometric algebra, in any form | to learn enough to rotate and move things, and to trust what the compiler checked | the README's example, then the tutorial |
| The arriver | geometric algebra from another source: rotors with the geometric product, quaternions, or a rigid algebra with e0 and planes as vectors; C++ | Galoot's choices: the null generator last, points as vectors, the antiproduct, results on their own layouts | to carry formulas and habits over without a silent sign error | the first-things page, then the conventions card, then the porting recipe |
| The researcher | geometric algebra in some convention; numerical work | what Galoot has verified and how; what is exact | to decide whether a result can be trusted, and to find a limit before meeting it | the conventions chapter, then the verification and limits pages |
| The user with a task | Galoot's basics, from the tutorial or from use | the one thing the task needs | steps that work | a how-to recipe or a reference entry |

The map (`docs/guide/README.md`) opens with these four as "who you are, where to start", in the reader's own words: "I know C++ and have not met geometric algebra", "I know rotors or quaternions from elsewhere", "I need to know what is verified", "I have a task".

## 2 What the tutorial assumes

Level: `undergraduate-intro` for the mathematics, in the pedagogical skill's table: each algebra step shown with its rule named, sentences of at most about 35 words. The reading level is the same.

The assumed terms are rows P01 to P09 and Q01 to Q10 of `concept-inventory.md`, and nothing else. Stated for an author:

**Mathematics.** A vector as an arrow and as coordinates; adding and scaling; length by Pythagoras; unit vector; angle, perpendicular, parallel, clockwise and counterclockwise; sine, cosine, radian; the dot product by coordinates and as lengths times cosine; the cross product as a definition seen once (it is used twice, both times to say what something is not, or what its numbers happen to equal); the area of a parallelogram and the volume of a box; a line, a plane, the origin, and the axes in space; a matrix as a table of numbers.

**Not assumed**, though an author who knows the subject will reach for each: "linear" and "bilinear" (say "distributes over sums"); "basis" (say "generators" once chapter 1 has defined it, and "unit directions" before); determinant (chapter 2 computes the 2 × 2 and 3 × 3 cases as differences of products and names them only in passing); "orthogonal" (say "perpendicular"); "orientation" in the technical sense (chapter 2 defines it for an area); complex numbers and quaternions (no chapter relies on them; a remark for readers who know them goes in the closing "Why it works" section, never in the main line); "homogeneous coordinates" (chapter 7 teaches the fourth coordinate from nothing); matrices as transformations.

**C++.** Compiling and running a C++23 program with an include path; `#include`, namespaces, aliases, qualified names; a `struct` with a `static constexpr` member; using a template with angle brackets; `using` aliases and `auto`; `constexpr` and `static_assert`; brace initialization and narrowing; that operators have precedence and can be defined for a type; `int` and `double`, truncating division and rounding; `std::println`; finding the first error in a compiler's output.

**Not assumed:** writing templates; concepts and constraints; argument-dependent lookup (every call in the tutorial is qualified, so it never arises); a string as a template argument (chapter 1 says in one sentence that `"e1 e2"` is read at compile time); CMake (the tutorial compiles with one command; the README has the CMake routes); anything about how the library works inside.

**The toolchain.** GCC 16 or Clang 23 with a standard library that has `<print>`, the compilers Galoot is tested with. The opening page says so before anything else and gives the one command that shows whether the reader's compiler will do.

## 3 The contract of each kind of page

| Kind | Reader first | Level | May assume | Objectives are |
| --- | --- | --- | --- | --- |
| Tutorial chapter | newcomer | `undergraduate-intro` | section 2, and the chapters before it | things the reader can do, in the words the reader has before the chapter |
| Exercises and solutions | newcomer | as the chapter | as the chapter | the chapter's |
| How-to recipe | user with a task; arriver for the porting page | `undergraduate-intro` | the tutorial chapters the recipe names in its "You need" line, or the same knowledge from elsewhere | the task of its title |
| Concepts page | all four | `undergraduate-advanced` | the tutorial, or geometric algebra from another source and the conventions card; linear algebra (matrix, determinant, linear map) | what the reader can explain or decide |
| Reference entry | user with a task; researcher | `undergraduate-advanced` | the concepts page it links; no tutorial narrative | none: an entry states facts |
| Conventions chapter | arriver; researcher | `undergraduate-advanced` | geometric algebra in some convention: the geometric product, the wedge, grades, the reverse, a dual, a versor | to predict the value of each test line before running it |
| First-things page | arriver | `undergraduate-advanced` | as the conventions chapter | none: a list of facts |
| Diagnostics page | anyone holding a message | `undergraduate-intro` | section 2's C++ and the term the entry links, nothing more: an entry is read by someone who has not finished the tutorial | to find the entry from the message and fix the program |
| Verification page | researcher | `undergraduate-advanced`, with no background in formal methods or numerical analysis assumed | that floating-point arithmetic rounds | to say, for a result, what was checked, how, and what was not |
| Map and glossary | all four | `undergraduate-intro` | section 2 | none |

Two consequences. A reference entry, a recipe, and a concepts page drop the tutorial's scaffolding (worked steps, prompts, faded examples): for a reader who knows the steps, they slow the reading. And the diagnostics page is held to the lowest level, because the reader with the least background meets it first.

## 4 The contract of each tutorial chapter

Each chapter may assume section 2 and the chapters before it. Its objectives are drafts for the chapter's author, written in the vocabulary the reader has on arriving, as the pedagogical skill requires: a term the chapter defines does not appear in its objectives. Each objective needs a worked example that demonstrates it and an exercise that tests it (plan criterion A7).

**1. A first program: vectors and dot products.** After it the reader can:
- compile a program that includes Galoot and names the plane, or the space, its vectors live in;
- make vectors with whole-number coordinates, add and scale them, and read a coordinate back;
- compute a dot product and decide from it, without a square root, whether two vectors are perpendicular and which of two is longer;
- state a fact about a result so that the compiler checks it, and print a result;
- read the message for a vector given the wrong number of coordinates, and fix it.

**2. The wedge.** After it the reader can:
- compute the area of the parallelogram on two vectors, with a sign that says which way round it runs;
- predict what exchanging the two vectors does to the result, and what happens when they are parallel;
- compute the volume of the box on three vectors;
- say which number of a result belongs to which pair of unit directions, and read it by name;
- store a result in a type of the reader's own, and explain why the compiler asks for braces and parentheses where it does;
- say what the three numbers share with the cross product and what is different.

**3. The geometric product.** After it the reader can:
- multiply two vectors by three rules on paper and predict which parts the result has;
- compute the same product in a program and check each part;
- show with numbers that the product depends on the order of its factors;
- take one part of a result, choosing between the operation that checks what it drops and the one that does not;
- undo a multiplication by a vector.

**4. Reflections and rotors.** After it the reader can:
- reflect a vector in a mirror through the origin, in whole numbers, and say what factor the answer carries;
- predict the turn that two mirrors make from the angle between them, and build it;
- turn any vector by it, chain two turns, and predict which acts first;
- write the turn for a given angle in `double`, and say why half the angle appears;
- say why this chapter does not use the function named `transform`.

**5. Complements and the meet.** After it the reader can:
- find, for a vector or a bivector of space, the blade made of the unit directions it lacks, with its sign;
- go from a plane through the origin to its normal and back;
- find the line in which two planes through the origin meet;
- predict what taking that blade twice gives, in the plane and in space;
- write all of this in C++ and avoid the two traps of `!`.

**6. Mirrors as planes: the antiproduct.** After it the reader can:
- reflect a vector in a mirror given by its plane, and turn it by two such mirrors, and get chapter 4's numbers;
- multiply two planes with a second product, and say how that product is made from the first and the complement;
- use `transform` and `compose`, and predict the order in which two motions act;
- explain what went wrong in chapter 4's mistake;
- say what factor a motion leaves and where it shows.

**7. Points, lines, and planes.** After it the reader can:
- write a position in space with four numbers, and tell when two such lists are one position;
- build the line through two points and the plane through three, and read off a direction, a normal, and an offset;
- find where a line meets a plane and where two planes meet, in whole numbers;
- test whether a point lies on a plane;
- store each result in the type made for it and read it by the right name;
- say in what sense two parallel planes meet.

**8. Motions.** After it the reader can:
- reflect a point, a line, or a plane in any plane;
- predict the motion that two parallel mirrors make, and that two crossing mirrors make, and build each;
- build a turn about any line and a move, from an angle and a distance, chain them, and predict the order;
- read the eight numbers of a motion and say why they hold halves;
- say what a motion written in whole numbers does to a point's fourth number;
- recognize the other common description of these motions and say how the two are related.

**9. Exact and rounded.** After it the reader can:
- say which parts of a computation are exact and which round;
- check a motion exactly at compile time and run the same motion in `double`;
- compare two rounded results correctly;
- turn checking on and say what it verifies and what it cannot;
- say when the compile time of a program will be felt.

**10. A robot arm.** After it the reader can:
- model an arm of three joints as three turns about three lines, and compute where its tip is;
- check the model exactly with quarter turns;
- run it with real angles and judge the result.

## 5 The form of "Before you start"

Every tutorial chapter, recipe page, and concepts page opens, after one paragraph that says what the page is for in words the reader has, with this section. The labels are fixed, so that a checker can read them and so that front matter could be generated from them if the owner later asks for it.

```text
## Before you start

**You need:** [chapters or pages, each a link], and [topics, in a clause each].
**Check yourself:** [two or three questions with one-line answers, each answer
saying where to review].
**After this page you can:**
- [objective]
- [objective]
```

The tutorial's opening page carries the check for section 2 as a whole: one command for the toolchain and six questions, three on vectors and three on C++. Drafts, with the answers the page gives:

1. What is the dot product of (3, 4, 0) and (4, −3, 0), and what does that say about the two vectors? (0; they are perpendicular.)
2. A parallelogram has sides (2, 0) and (1, 3). What is its area? (6: the base 2 times the height 3.)
3. What are the cosine and the sine of a quarter turn, and of a half turn? (0 and 1; −1 and 0.)
4. What does `static_assert(2 + 2 == 5);` do, and when? (It stops the compilation, with a message.)
5. Why does `int n{2.5};` not compile where `int n(2.5);` does? (Braces reject a conversion that loses information.)
6. In `a ^ b == c`, which operator does C++ apply first? (`==`; `^` binds more loosely.)

A reader who misses a question is told which school topic or which C++ feature to review, by name; the page links nothing outside the repository except the two sources the README already names.

## 6 What a contract may not do

- A contract is not silently widened. When a cold reader stumbles on a term, the page changes; the term joins the assumed terms only when it is school mathematics or section 2's C++, and then this record changes in the same unit.
- The README is not a prerequisite of the tutorial. The opening page says that the README's example is the destination and the setup check, and assumes nothing from it.
- No tutorial chapter depends on a page of milestone D3, which is not authorized (decision 0253 item 23): a chapter may link the conventions chapter, the glossary, the first-things page, the limits page, and earlier chapters.
