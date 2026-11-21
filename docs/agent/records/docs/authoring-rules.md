# Page templates and authoring rules of the user documentation

Written by: Claude (agent, Fable 5.2, math-expert and hci-expert roles), 2026-11-21, for unit U1 of plan 0049. Brought in step with decisions 0255 part A and 0258 on 2026-11-21 by Claude (agent, Opus 5.5, math-expert and hci-expert roles): section 1, templates T4 and T5, rules W9, W12, A8, A11, R5, and G1 to G8, and sections 4 and 5. Not yet reviewed.

An author of a page under `docs/guide/` reads this record, `learner-contracts.md`, the chapter's rows of `concept-inventory.md`, and `misconceptions.md` before writing a line. Section 1 says what is already decided and not the author's to change: what the owner ruled, and what the coordinator settled. Section 2 has a template for each kind of page. Section 3 has the rules, each with an identifier a reviewer can cite and its reason in a clause. Section 4 says which rules a machine checks.

## 1 What is already decided

The owner ruled these in decision 0253 (items 24 to 35) and decision 0258; an author designs within them and reports, not works around, anything that argues against one.

| Ruled | Consequence for an author |
| --- | --- |
| Pages live in subdirectories of `docs/guide/` by kind: `tutorial/`, `exercises/`, `howto/`, `concepts/`, `reference/`, `figures/`, and single pages for the conventions, the diagnostics, the verification, the glossary, and the limits | A page has one kind and follows that kind's template. A paragraph that belongs to another kind moves there and leaves a link |
| The conventions chapter restates the specification in plain words; user pages do not link it | State the rule and its reason on the page; the agent-side trace keeps the two in step |
| Unicode symbols, and words for accented operations (decision 0253 item 27); of them, ⟑ and ⟇ in running text, and `*` and `%` inside code and text blocks (decision 0258 section 4 item 9) | Section 3, rules A5 to A7 and A11, and `notation-key.md` rules 6 and 9 |
| About fourteen SVG figures drawn by one standard-library script, with a staleness test | A figure is asked for by its scene description; no hand-drawn file |
| The reference is written by hand, with coverage checks | Template T4; every fact that can be a `static_assert` is one |
| Every example runs in every configuration until the cost is measured | Rule E8 keeps each example cheap |
| In-repository checks only; no YAML front matter | The contract is the visible "Before you start" section (`learner-contracts.md`, section 5) |
| The tutorial teaches around what 1.0.0 lacks | Research note 0029 section 4 lists each lack and what the page does instead; no page promises an operation the release does not have |
| STYLE S11's new wording is ratified (decision 0258 section 2): on each declaration of a public header outside a `detail` namespace, one sentence that says what it is; on each declaration with a reference entry, a pointer to the guide page and heading that hold its contract; a precondition whose violation is undefined behavior in a build without checking kept on the declaration in full. Under the owner's condition that the headers and the guide not drift apart (section 3 of that decision), the wording is applied, and a contract leaves a header, only when seven checks run as tests at each merge into `main`. Until then S11 as ratified on 2026-11-01 stands and no unit removes a contract from a header. The reference pages are in milestone D3, which is not yet authorized | A reference entry is written to be the full statement of its contract, and so that each of the seven checks can be built: rules G1 to G8 |
| `transform` and the classical sandwich: taught in the documentation now, in four pieces (decision 0258 section 4 item 1); the first-things page gains a tenth fact, the same trap (item 11) | `misconceptions.md` section 5 says where each piece goes, what it says, and with which values; rule A8 |
| A human cold reader: still optional, and nobody is named (decision 0258 section 4 item 10) | Language-model readers only, each record saying so (rules R4 and R5) |

The coordinator settled these in decision 0255 part A, as readings of plan 0049 that no ruling of the owner's changes; the owner can reverse each.

| Settled | Consequence for an author |
| --- | --- |
| The chapter cut of research note 0029 section 3.3 (item 4): the plan's chapters 1 and 2 are one, and a new chapter 6 teaches the antiproduct in R(3, 0, 0) before the rigid algebra; the page count of plan 0049 section 4.1 does not change | The chapters are those of `concept-inventory.md` part 2 and `learner-contracts.md` section 4. U5 writes chapters 1 to 5 and U6 chapters 6 to 10 of this cut |
| Tutorial examples are whole programs with one fixed preamble per chapter; the mechanism of decision 0242 does not change (item 5) | Rule E2. No page preamble that later blocks are compiled with |
| The tutorial links only pages that exist at milestone D2; chapter 8 points to the conventions chapter, not to the porting recipe (item 6) | Rule W12 and `learner-contracts.md` section 6 |
| U3 names the conventions page so that the citation check passes unchanged, and no citation pattern is weakened for it (item 7) | Section 5, finding 1. `docs/guide/conventions.md`, the name in plan 0049 section 4.1, is not that name |
| A second language-model cold reader, told that it knows rotors and quaternions, reads the conventions chapter and the first-things page at U4; it is the only test of the documentation of the `transform` trap (item 8) | Rule R5 |

The governance adds: user documentation is plain American English and changes in the same unit as the code it describes; every code example is compiled and run as a test; no user page names a governance document, a numbered rule, a record, or an agent.

## 2 Templates

### T0 The map (`docs/guide/README.md`) and the tutorial's opening page

The map: one paragraph on what Galoot is for; "Who you are, where to start", the four readers of `learner-contracts.md` in their own words, each with one link; the kinds of page, a line each; the notation key (`notation-key.md`, section 2) with the note on fonts; the release the pages describe.

The opening page of the tutorial: the toolchain and the one command that tests it; the self-check of `learner-contracts.md` section 5; the destination, in the reader's words and one figure ("by chapter 8 you will turn a point about any line and move it, and the compiler will have checked the arithmetic"), with a link to the README's example as the program to run now and understand later; the ten chapters, each with a sentence that uses only school words; how a chapter is laid out and how to use the exercises. It defines no term.

### T1 A tutorial chapter

A chapter takes one sitting. Its parts, in order:

1. **Title**: what the reader will do; it may name the one concept the chapter defines.
2. **Opening paragraph**: the question the chapter answers, in words the reader has on arriving.
3. **Before you start**: the fixed form.
4. **Sections**, one per concept of the chapter's inventory rows, in the inventory's order. Each section follows the pedagogical skill's sequence:
   - the question the concept answers;
   - a concrete case with small whole numbers, worked by hand;
   - the definition, with the term in italics once, stated in symbols and again in words, with the C++ spelling beside it;
   - a worked example in the fixed form: **Problem**, **Plan**, **Solution** with numbered steps under bold subgoal labels, **Check**, a **self-explanation prompt** with its answer, **Takeaway**. The Solution ends in one compiled program whose `static_assert`s are the hand-derived values and whose `// prints:` lines show them;
   - a contrasting example that differs from the first in one feature;
   - the refutation of each misconception placed here: the wrong idea, "this is wrong", the correct idea with the value that shows it, and why the wrong idea is tempting;
   - one or two check questions, answered at the end of the chapter.
5. **A mistake you will make**: the chapter's likely mistake from the inventory, as a program that fails, what the reader sees (the message, trimmed to the lines that matter, or the `static_assert` that fails), what it means, and the fix. A mistake that fails to compile is a `cpp rejected` block; one that compiles to a wrong value is a `cpp` block whose assertion states the wrong value as what the program gives.
6. **The faded sequence**: after the fully worked examples, one example with its last step left to the reader (a `cpp exercise` block with `// your turn`), then one that states only the goal.
7. **Why it works** (optional reading): any general argument the exercises do not need, as a calculational chain. The main line never depends on it.
8. **Exercises**, numbered within the chapter: same structure with new numbers first, then a new situation, then one that needs an earlier chapter, and one true-or-false question with its reason that probes a misconception. Each has a "predict, then run" form where it can: the reader writes the expected value before compiling.
9. **What you can do now**: the objectives again, one line each, and nothing else.

Solutions are on the chapter's page under `exercises/`, each a compiled program with its derivation, in the worked-example form.

### T2 A how-to recipe

Title: the task, as the reader would say it ("Turn a point about any line"). Then: **You need** (chapters or equivalent knowledge, and what must be at hand); **Steps**, numbered, each an action; **The whole program**, one compiled block; **If it goes wrong**, the two or three failures a reader meets, each with its message and fix, linked to the diagnostics page; **See also**. No derivation, no alternatives, no history: a recipe shows one way.

### T3 A concepts page

Opening paragraph: the question the page answers and who needs the answer. "Before you start". Then the explanation, which may weigh alternatives and give reasons, as a tutorial may not. A derivation follows the knowledge base's proof layout: the claim in words; a small case with numbers and a check; a table of the notation used; numbered steps, each with its reason; "what this does not show"; one or two exercises. Each page ends with "Where this is used", the reference entries and recipes that depend on it.

### T4 A reference entry

Entries on a page are in the order a reader would learn them, the thing others depend on first; the index page lists every name alphabetically. One entry:

| Field | Content |
| --- | --- |
| Name | The declaration as a user writes the call, its qualified name as the interface manifest spells it, and the header to include (rules G1 and G2) |
| Signature | The declaration as the manifest tool writes it (rule G4) |
| One sentence | The declaration's `///` sentence, word for word: what it is, in terms a user can read without the tutorial's story (rule G3) |
| Covers | The members and rejecting overloads the entry covers, each by its manifest name and the rule that covers it (rule G2) |
| Requires | Each precondition, with "must", numbered; a precondition kept in full on the declaration is quoted in its words (rule G5) |
| Guarantees | What holds of the result |
| Result layout | The blades, their order, and their orientations |
| Exactness | Exact for an exact scalar; for a rounding scalar, what is rounded and where the bound is stated; or "does no arithmetic" |
| On violation | For each requirement, by its number: rejected at compile time (with the message, quoted as rule G6 says, and linked), checked in checking mode and in every constant evaluation, or not checked, and then what happens; for each checkable violation, the example or compile-fail test that exercises it (rule G7) |
| Degenerate inputs | Zero, parallel, unnormalized, or non-versor inputs, and the value returned for each |
| Asking whether a call is valid | Where a rejecting overload exists: that a `requires`-expression is told yes, and what to ask instead |
| Cost | What is worked out at compile time and how it grows, with a link to the measured sizes |
| Example | One compiled program; every fact about types and layouts is a `static_assert` |
| Why | A link to the concepts page that gives the reason for the design, where one exists |
| See also | Related entries |

An entry is the full statement of its contract, so it is complete even where that repeats a neighbor. Until the seven checks that rules G1 to G7 serve run as tests, the header keeps its contract too (section 1); afterward the header keeps its sentence, its pointer, and any precondition kept in full, each of which reads the same in the entry. It has no worked steps and no prompts.

### T5 The conventions chapter

Its file name is settled in section 5, finding 1.

1. **The card**, on one screen: a table with one row per choice. Columns: the choice; Galoot's rule in a clause; a one-line expression; its value; a link to the section. A reader from another source runs the lines and finds each difference without reading further. The rows, in order: generator names and numbering; where a null generator sits; blade names and orientation; the order of components; the reverse, the antireverse, the grade involution; the two complements; bulk, weight, and the duals; which dot; the four products and their operators; `transform`, even and odd; `transform` against the classical sandwich, with the row and section that `misconceptions.md` section 5.2 sets out; `compose`'s order; a point, a line, a plane, a motor, a flector by their coefficients; full angles in, halves stored; operator precedence; the printed form.
2. **One section per row**: the rule; a compiled example that would change if the rule did, its values derived by hand beside it; the reason for the choice where there was one; how the evidence stands, in words a user can read ("recomputed by a script that shares no code with the library").
3. **How this differs from other sources**: only rows for sources someone has read, each naming the source and the lines of the card on which it differs.
4. **Where published sources disagree**: each recorded disagreement, the choice made, and an example that tells the alternatives apart.

### T6 A diagnostics entry

Grouped by what the reader sees first. One entry: the message, exactly as it appears; a program that produces it (`cpp rejected`); what each compiler prints, trimmed, with the line that carries the library's text marked; what the reader probably meant; the fix, as the changed line; a link to the term or the entry it concerns. Where a message was reworded after 1.0.0, the entry keeps the old text under "formerly", because a reader searches for the text in hand. The page ends with a table of every message.

### T7 The verification page

Three questions in plain words (is the library computing what its definitions say; are the definitions the mathematics people mean; how far can a rounded result be from the exact one). Then the kinds of evidence, strongest first, each explained with no background assumed; a table by area saying what was checked, by which kind, over which algebras, scalar types, and compilers; what a machine-checked proof shows and does not show; what is not verified; the commands that rerun the checks. The page names the release it describes, and no claim is stronger than its record.

### T8 The glossary

Two tables. Terms: term, definition in one sentence, the page of its defining instance. Symbols: symbol, how to read it aloud, meaning, C++ spelling, first page. Built from `concept-inventory.md`, one row per row of kind "term" or "symbol". One term for one thing: a synonym used by other sources is listed with "see".

## 3 Rules

### Wording and structure

- **W1** Write for the page's contract and for nobody else. A sentence that helps another reader goes on that reader's page.
- **W2** Every word, symbol, and C++ name is assumed by the contract or defined earlier on the reader's path. That holds in titles, headings, captions, figure labels, comments in code, and exercises. A forward reference is a signpost that makes sense to a reader who never follows it.
- **W3** A heading of a tutorial section says what the reader will have at its end ("Join two points into a line"), not the tool ("Using `wedge`").
- **W4** Italics mark the defining instance of a term and nothing else. Each italic term has a glossary row.
- **W5** One way first. An operation is introduced by its named function, which is its name; its operator follows in the same section with its trap; after that the page uses the operator. A recipe and a chapter show one way to do each thing.
- **W6** Every call is qualified with its namespace alias (`core::`, `rga3::`), as the README's are, so that no page depends on name lookup the reader has not been taught.
- **W7** No step is called easy. The words "simply", "just", "obviously", "clearly", "trivially", "straightforward", "it can be shown", and "left to the reader" do not appear on a teaching page.
- **W8** Cut what serves no objective: history, asides, and remarks for readers who know more. One such remark may go in "Why it works".
- **W9** A page names nothing behind the user documentation: no governance document, numbered rule, record, or agent. The citation check also rejects, in ordinary teaching text: "Lesson", "Plan", or "Review" followed by a number; "section" or "item" followed by a dotted number; a capital P or W followed by digits, so points are p, q, r and never P1; a parenthesized capital C, S, or V with digits, such as (C3); the word "Opus"; and the file names of the governance documents and of the conventions specification in any case, `conventions.md` among them, so that no page is named so or links a page so named. Sections are referred to by name, chapters as "chapter 4", equations as "Eq. (3)", exercises as "exercise 4.2".
- **W10** One term for one thing, as the glossary has it. "Complement" always means the right complement unless "left" is written; "motion" is the plain word and "motor" the object; "unitized" is used for weight 1 and "unit" for norm 1.
- **W11** Plain American English; no em dashes; sentence-case headings; bold only for the fixed labels of a worked example and of "Before you start".
- **W12** A tutorial page links only pages that exist at milestone D2: earlier chapters, the opening page, the conventions chapter, the glossary, the first-things page, the limits page, and a page that U3 created with content moved from today's guide, at a heading it has at D2. Chapter 8 points to the conventions chapter for the other convention, not to the porting recipe, which is in milestone D3. A forward link to a later chapter follows rule W2.

### Mathematics

- **A1** Every expected value is derived on the page, in the prose or in a comment beside it, before the program states it. A value is never pasted from a run.
- **A2** A statement that two computations agree is tested with inputs that could tell them apart. For a rotation: an angle that is not a multiple of a quarter turn (the mirrors (1, 0, 0) and (2, 1, 0) give 2 − e12, with cosine 3/5 and sine 4/5) and a vector off the rotor's plane. For a reflection: a vector neither along the normal nor perpendicular to it. For a sign convention: an algebra in which the alternatives differ; three generators cannot tell the left complement from the right, nor show that two complements differ from none.
- **A3** A statement true only for some metrics says for which. "In a Euclidean algebra", "in rigid(3)", "when no generator squares to zero" are part of the sentence.
- **A4** Numbers before symbols: each rule is first worked with small whole numbers, which the compiler checks exactly, and the general statement follows.
- **A5** Symbols are those of `notation-key.md`, each with its C++ spelling at its first use on a page.
- **A6** Accented operations are written as words: `reverse(a)`, `antireverse(a)`, `right(a)`, `left(a)`. The literature's mark is shown once, where the operation is defined.
- **A7** A derivation is a calculational chain in a `text` block: one expression per line, and between two lines the relation and its reason in braces. Each line follows from the one before by the reason given and by nothing else.
- **A8** A sandwich of a rotor is written `r * v * ~r`. `transform` is never used with a versor built by the geometric product, except in the four pieces that show it gives another vector (`misconceptions.md` section 5); with `transform`, a versor is a plane, an antiproduct of planes, or a complemented rotor.
- **A9** A point, a line, a plane, or a motor may differ from another by a factor and be the same object. A page that compares two says which it means, and compares coefficients only after making the weights equal.
- **A10** A sentence about another author's convention names a source that someone has read, and says no more than was read.
- **A11** The geometric product and the antiproduct are written ⟑ and ⟇ in running text, and `*` and `%` inside a `text` block, a `cpp` block of any kind, and the comments of a `cpp` block. Inside a block, `*` stands wherever running text would write ⟑ (`notation-key.md` rule 6) and `%` wherever it would write ⟇; juxtaposition stays where rule 6 allows it, and every other symbol is the same in both places. Blocks are monospaced, and no monospaced font on the machine probed has either glyph. One line of each kind, from chapter 6:
  - in running text: "the two mirror planes multiply to the complemented rotor, (e23 + e31) ⟇ e23 = e123 − e3";
  - in a `text` block, the same rotor turning e3:

    ```text
    (e123 − e3) % e3 % (e123 + e3) = 2 e3
    ```

### Examples

- **E1** Every `cpp` block is a whole program that compiles alone, runs, and prints what its `// prints:` lines say. A fragment that cannot stand alone is a `text` block and is not presented as code to copy.
- **E2** A chapter has one preamble (the includes, the namespace aliases, the algebra's tag, the type aliases), identical in every example of the chapter and explained once. The reader learns to skip it.
- **E3** An example shows the typical use. A boundary case belongs in the test suite, unless the boundary is the lesson.
- **E4** One new thing per example. Names in an example say what the value is (`mirror`, `quarter_turn`, `origin`), and the same thing has the same name through a chapter.
- **E5** The output is beside the code that printed it, and a figure beside the sentence that describes it.
- **E6** Printed values are exact: whole numbers, or a `double` printed with a stated number of digits. A program that compares rounded values says how.
- **E7** A `cpp rejected` block states in its first comment the text the error must contain, and that text is a literal of the library.
- **E8** Examples use typed layouts of a few blades. An example on every blade of an algebra costs seconds to compile and can meet a compiler's limit on constant evaluation; it appears only on the page about that limit.
- **E9** An exercise's scaffold and its solution agree outside the lines marked `// your turn`.

### Figures

- **F1** A figure appears where the idea is spatial and nowhere else. Its coordinates are those of the example beside it.
- **F2** Labels sit at the part they describe; there is no legend. A label uses only terms defined by that point of the page.
- **F3** The alternative text and the caption carry the content for a reader who cannot see the figure. Nothing depends on color alone, and the figure reads on a light and on a dark background.

### Claims and cost

- **C1** A factual claim that no example shows has a row in the claim trace: page, heading, claim, source.
- **C2** A page describes one release and says which. A statement about a later release is not made.
- **C3** What is not checked is said as plainly as what is: "not checked", "not tested above 16 generators", "over `double` this is not verified".
- **C4** No adjective stands for a measurement. "Fast", "cheap", "small", "accurate", and "robust" appear only with a number and its conditions: compiler and version, flags, date, the machine's kind, and the command that reproduces it.
- **C5** An accuracy statement gives the measure, the inputs it covers, where the bound comes from, the conditions, and what is not promised.

### Reading tests

- **R1** The author does not argue with a cold reader. A stumble is fixed on the page, or the missing item joins the contract in the open.
- **R2** A cold reader gets the contract, the pages up to the one under test, the glossary, and a command that compiles one file. It gets no path to the headers, the tests, or the records, and lists every file it opened.
- **R3** A cold reader reports, beside the pedagogical skill's table, every term, fact, or convention it supplied from its own knowledge. A language model has read about geometric algebra; what it supplied is what the page failed to say.
- **R4** A reader's pass is weaker evidence than a reader's stumble, and a language model's pass is weaker than a person's. Each record says which it was.
- **R5** The second cold reader, at U4, is a language model told that it knows geometric algebra from another source: Euclidean rotors built with the geometric product, and quaternions. It gets what R2 gives, with the conventions chapter and the first-things page as the pages under test and their contracts from `learner-contracts.md` section 3. It predicts the value of each line of the card before running it. Then, from those two pages alone, it writes a program that turns (4, −1, 2) by the rotor (1, 0, 3)(0, 0, 1) = 3 + e13 and states the result. The pages pass only if it reaches (44, −10, −8) by a call it took from them; (−20, 10, −40), from `transform(v, t)` with t the rotor, is the trap. This rotor turns in the plane of e1 and e3, with cosine 4/5, sine 3/5, and factor 10, where the pages' examples use 2 − e12 in the plane of e1 and e2, so that the reader must carry the rule to a case the pages do not print; no page under test uses this rotor or this vector. The values are derived here: with t̃ = 3 − e13, t e1 t̃ = (8, 0, −6), t e2 t̃ = (0, 10, 0), and t e3 t̃ = (6, 0, 8), so t v t̃ = 4 (8, 0, −6) − (0, 10, 0) + 2 (6, 0, 8) = (44, −10, −8); `transform(v, t)` is −t̃ v t (research note 0029, Theorem A), and t̃ e1 t = (8, 0, 6), t̃ e3 t = (−6, 0, 8), so it is −(4 (8, 0, 6) − (0, 10, 0) + 2 (−6, 0, 8)) = (−20, 10, −40); and !t = 3 e123 − e2 gives (44, −10, −8) (checked by `checks/rulings_values_check.py` and `checks/rulings_values_probe.cpp`). It reports as R3 asks, with each wrong prediction beside the line. It is the only test of the documentation of the `transform` trap, so its record says that a language model read the pages and no person did. A human cold reader is still optional and unnamed.

### Reference entries and the headers

The owner's condition on the new wording of S11 is that the headers and the guide not drift apart (decision 0258 section 3). Seven checks are to hold it, designed and built by plan 0049's unit U9 and plan 0048's phase 4, each an acceptance criterion of their plans; none exists yet. These rules are what the author of a reference entry does so that each check can be built and pass. Each names its check by the decision's number. They hold from the first entry, before the checks exist, so that no entry is rewritten for them.

- **G1** (check 1, pointers resolve) An entry's heading is the text that the header's pointer quotes, a heading of the list U3 publishes for the reference, and no other heading on that page has the same text. A heading is renamed only in a commit that changes every pointer to it too. A declaration in a `detail` namespace has no entry and no heading.
- **G2** (check 2, coverage both ways) An entry documents one public declaration, or one overload set, and its Name field gives the qualified name as the interface manifest of plan 0047 spells it. A member or a rejecting overload has no entry of its own: its owner's entry lists it under Covers, by its manifest name and with the rule that covers it ("a member of `mv`", "the overload of `get` that rejects a blade the layout lacks"). An author writes no entry for a name the manifest lacks, and reports a public declaration that no entry documents or covers instead of leaving it out.
- **G3** (check 3, one sentence, one text) The One sentence field is the declaration's `///` sentence, copied from the header and not rewritten: the same words, signs, and code spans, with only line breaks and the comment markers gone. Where the declaration does not yet carry such a sentence, the entry's author drafts one under S11's new wording and reports it to plan 0048, which owns the header and puts it there (plan 0049 section 7 item 6). The field is a paragraph of its own, directly after the Signature field, and holds that sentence and nothing else: no link, no emphasis, nothing the header does not write. Where the header's sentence has ⟑ or ⟇, as `geometric_antiproduct`'s has, the entry keeps the symbol, since the field is running text (rule A11) and the two must read the same. To change the sentence, change the header and the entry in one commit; a sentence that is not one sentence, or that a user cannot read without the project's records, is reported to the header's owner, plan 0048, and not mended in the entry alone.
- **G4** (check 4, signatures agree) The Signature field prints the declaration as the manifest tool writes it: the template head, its constraints, the parameters, and the return type, with nothing elided ("..."), shortened, or dropped. It is copied from the tool's output, not typed. It sits in a block of its own, one per overload, that holds the declaration and nothing else. Because rule E1 compiles every `cpp` block as a whole program, that block is of a kind the documentation tests do not compile; its info string is fixed with the check.
- **G5** (check 5, preconditions kept in full read the same) A precondition that stays on its declaration in full is quoted in the Requires field in the header's words: the header's sentences whole, as one numbered item, with no word changed, added, or dropped and no link or emphasis inside them. What the entry adds, such as the detail of On violation, goes in a sentence of its own after the quoted ones. The preconditions kept in full are those on the list that the comment table's checker holds (`records/docs/check_comment_moves.py`), as decision 0258 section 4 item 4 rules it: `rotation`'s zero direction (`motions.hpp:48-61`) stays in full, though the checker's list does not hold it yet, and the unchecked floating division of `linear.hpp:397-406` moves. Any other Requires item is written in the entry's own words.
- **G6** (check 6, messages agree) A message that an entry quotes is the library's string literal, whole and exactly as the header spells it, in a code span or block, never reflowed inside the literal, cut with an ellipsis, or paraphrased in quotation marks or code font; a paraphrase is plain prose. Where a compiler prints the literal inside its own text, the entry marks which part is the library's, as template T6 does. A text the library no longer has appears only under a field labeled "formerly" (template T6), so that the check can tell a former text from a current one. The same holds for every page that quotes a message (rule E7).
- **G7** (check 7, stated contracts are exercised) For each numbered requirement whose violation is checkable, rejected at compile time or checked in checking mode and in every constant evaluation, On violation names what exercises it: the entry's own compiled example, a `cpp rejected` example by its page and heading, or a compile-fail test of the suite by its test name. A requirement that is not checked says "not checked" and names nothing (rule C3). An author who finds nothing that exercises a checkable requirement writes the `cpp rejected` example or reports the gap; a requirement is never called checked without one.
- **G8** (the process rules of the same condition) A unit that changes what a declaration requires, guarantees, or preserves changes its entry in the same unit, and its review request names the entry; the blind reviewer's checklist asks for it. A contract leaves a header only after the entry that receives it has passed review (decision 0253), and not at all until the seven checks run at each merge into `main`.

## 4 Which rules a machine checks

| Rule | Check (plan 0049 section 5.3) | By hand |
| --- | --- | --- |
| E1, E7 | the documentation tests of each page | |
| E9 | `tests.docs_exercises` | |
| W2, W4 | `tests.docs_terms`, for glossary terms along the tutorial's order | symbols, C++ names, and words that are not glossary terms: the cold reader |
| W9 | `tests.citations_guide` | |
| F1, F3 | `tests.docs_figures`, for staleness and alternative text | that the figure matches its example: review |
| T4's coverage | `tests.docs_reference` | the prose of Requires and Guarantees: the blind reviewer's sample |
| T6's messages | `tests.docs_diagnostics` | |
| links | `tests.docs_links`, `tests.docs_header_pointers` | |
| A1, A2, A3, A10 | | the independent math review of plan section 9.2 |
| W7, W11 | `amspell.py --check` and the writing-style scans | the reviewer reads the hits |
| C1 to C5 | | the trust review of plan section 9.3 |
| A11 | half of it by the script that `notation-key.md` section 6 suggests: a ⟑ or ⟇ inside a block | that each `*` and `%` in a block stands where the symbol would: review |
| W12 | `tests.docs_links`, while the pages of D3 do not exist | once they do, that the tutorial links none of them: review |
| G1 | check 1, `tests.docs_header_pointers` | |
| G2 | check 2, `tests.docs_reference` with the manifest | that the rule under Covers is the right one: the blind reviewer's sample |
| G3, G5 | checks 3 and 5, to be built | |
| G4 | check 4, by the manifest tool | |
| G6 | check 6, `tests.docs_diagnostics` | |
| G7 | check 7, to be built | that a named example or test exercises the requirement it is named for: review |
| G8 | | the blind reviewer's checklist |
| R5 | | the coordinator, at U4 |

Two things the checks cannot hold that matter most here: that a value was derived and not pasted (A1), and that a test could have failed (A2). Both are what the math reviewer is asked first.

## 5 Findings for the units that follow

1. **A link to `conventions.md` fails the citation check. Settled:** U3 names the conventions page so that the check passes unchanged, and no citation pattern is weakened for it (decision 0255 part A item 7). `tools/citation_patterns.txt` rejects the file name in any case, so that no page cites the specification, and plan 0049 section 4.1 names the new page `docs/guide/conventions.md`. A page that links it, and a header comment that points to it, fail `tests.citations_guide` and `tests.citations`. (Checked: a page with the link `(conventions.md)` is reported by `python3 -I tools/test_references.py citations`.) Of the two options U1 gave, dropping the lower-case file name from that pattern is closed by the settlement; the other, `docs/guide/conventions/README.md`, needs no tool change. The name is U3's to choose and publish with its heading list, before any page or pointer links the chapter.
2. **The tutorial's examples are whole programs. Settled:** one fixed preamble per chapter (rule E2), and decision 0242's mechanism does not change (decision 0255 part A item 5; research note 0029, open item 7).
3. **The pedagogical skill asks for front matter and its two checkers**; decision 0253 item 33 rules them out until the owner says where they are. Section 2's fixed labels are chosen so that front matter can be generated from a page if that changes.
