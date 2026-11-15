# 0019 API design: usability evidence, and recommendations for WU8 and after

Date: 2026-11-01. Checked by: the hci-expert agent in the researcher role (Claude Opus 5.5), with three reading assistants (Claude Sonnet) that fetched and read sources and reported back; which claims rest on an assistant's reading, and which of those I re-read myself, is marked on each claim. Scope: the owner's request to research API design in general and for a library like Galoot, and to turn it into recommendations for WU8 (the `mv` type: concepts, the aggregate, `apply`, `truncate`, `get<"e41">`, exact equality) and the units after it (products and operators, `transform`, models, `format`). It reads plan 0001 sections 3 to 9, the design study's API sections, notes 0010 and 0012, decisions 0033 and 0034, CONSTITUTION C5, C8, C9, and STYLE S7 to S16, and, after it was committed to `develop` during this work, plan 0016 (WU8), which it cites but does not change. Nothing under `include/` was touched.

Revision and provenance. Branch `research/api-design`, from `develop` at `7da09f1`. The experiment sources and every diagnostic text quoted below are committed in `docs/agent/records/spikes/api-design/` (first at `330205c`); the run at `330205c` reproduced each committed text byte for byte (`adcdce9`). Probes added at `23dea03`.

How each claim is marked:

- **Read**: I read the source myself, at the URL given, on 2026-11-01.
- **Read (assistant)**: a reading assistant read the source and reported it; I did not re-read it. **Read (assistant, spot-checked)**: I then re-read the quoted passage myself and found it as reported.
- **Local**: compiled on this machine (GCC 16.2.1, Clang 23.1.4, libstdc++ 16) by the scripts named, with `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off`.
- **Inferred**: my reasoning, not checked. **Judgment**: a design opinion, not evidence.

Quotations are marked with quotation marks and come from the source named beside them.

## 1. Departures, skipped checks, and ambiguities (C20)

1. Sources not read (V9). Full texts not retrieved, so their claims are not used beyond what a named substitute says: Green and Petre 1996 (the 1998 Green and Blackwell tutorial is read instead); Ellis, Stylos, and Myers 2007 on factories (only a later paper's summary, "up to 10 extra minutes", is used, marked secondary); Myers and Stylos 2016, CACM; Becker et al. 2019 (its guideline list is used from Sy Brand's restatement in WG21 P2429R0, a secondary account that may differ in wording and numbering); Denny et al. 2021 (P2429R0's summary only); Traver 2010; Lawrie et al. 2006; Butler et al. 2010; Arnaoudova et al. 2016 (linguistic antipatterns); Piccioni, Furia, and Meyer 2013; the Blaze wiki (404; Blaze headers read instead); the Gaigen 2 GPCE paper and manual (Fontijne's thesis read instead); Hildenbrand's book; the Breuils et al. survey; the ga-benchmark compile-time results; Mariani's original "pit of success" post (Atwood's post read through a fetch summary); a primary source for the principle of least astonishment. Bloch's talk was read as a 2005-dated slide deck, not the 2006 OOPSLA companion.
2. Fetch summaries. A few pages were read through a summarizing fetch rather than as raw text (Dr. Dobb's 2004, Atwood, Nielsen, Rust RFC 1644, parts of the Versor, Klein, and ganja.js material). Claims from them are marked Read (assistant) and no quotation from them is relied on unless spot-checked.
3. Core Guidelines version. STYLE S1 pins the guidelines at commit `d1dda82e`; the I, C, and ES rules quoted here were read from `master` dated "Jun 14, 2026". The rule titles quoted are stable across those versions as far as I know (Inferred, not compared).
4. No user study. Every statement about what Galoot's users would find easier is inferred from the literature and from diagnostic texts; no person was observed using Galoot. The ranking of "most likely mistakes" in section 6 is Judgment.
5. Plan 0016 overlaps this note. The WU8 planner committed plan 0016 to `develop` while this note was in progress, with owner questions that this note's evidence bears on (its items 1 to 11). This note does not edit it; section 6 says where the evidence agrees or differs, item by item.
6. Process. One reading assistant created three empty files in the main checkout by mistake and deleted them; `git status` there afterward shows only `runs.tsv`, modified by other agents' runs. One of its fetches, with a browser User-Agent, was refused by the permission system and was not retried. No email address or personal identifier went into any request, and no maintainer or author was contacted.
7. Exercise code location. The knowledge base asks that exercise code live in `math-tools/exercises/`; the brief makes math-tools read only. The exercise code lives in this spike directory, and the knowledge-base exercise entry describes it without a path.

## 2. Summary

1. The general literature agrees on a small set of rules that apply directly to a header-only template library: fail at compile time and at the first misuse (Bloch); make the common task short and the method findable from the object the user starts with (Stylos and Myers measured 2 to 11 times faster task completion); full-word names beat abbreviations (19 percent faster defect finding, Hofmeister et al.); every observable behavior becomes a dependency (Hyrum's law), so an interface change is cheapest before release.
2. Compiler error messages are read, with effort comparable to reading code (Barik et al., eye tracking, 56 participants), and their readability predicts task success. The guidance from Elm, rustc, and the Becker et al. survey converges: say what is wrong in the user's terms first, in one error, with the user's code shown; put the remedy in a separate clause; no cascades.
3. Every surveyed math library that overloads `^` in C++ or Python warns about its precedence (Terathon, GATL, Garamon, clifford); ganja.js changes the precedence with a transpiler. Galoot's rule of full parentheses (S13) matches the field. Named functions as the interface with operators as aliases (S13, CONVENTIONS 7.1) matches std::linalg's stated reasons and Terathon's.
4. Silent lossy conversion is the most common GA-library complaint found (Versor issue 20; Gaigen 2's thesis admits it drops coordinates "silently"; GLM's silent no-op swizzle assignment, issue 786). Units libraries (mp-units, Boost.Units) treat it as the first safety rule and spell the lossy path with its own name. C8 and `truncate` are on the side of the evidence.
5. Local: an aggregate whose storage is a `std::array` accepts too few values and zero-fills the rest with no warning on either compiler, even with `-Wall -Wextra -Werror`; only a struct with named members draws `-Wmissing-field-initializers`. A non-aggregate with a constructor that requires one value per component stays trivially copyable and structural, and reports "3 == 4" on both compilers. This is the strongest local finding against STYLE S8's word "aggregates".
6. Local: a type that carries the blade names as written prints those names in every diagnostic on both compilers (`"e41 e42 e43 e23 e31 e12"`), where the mask spelling prints `{9, -1}, {10, -1}, …` and, on GCC, libstdc++ internals; the longest GCC line falls from 534 to 265 characters for `get`, and from 892 to 363 for a lossy conversion.
7. Local: a catch-all whose `static_assert` tests a simple numeric condition, rather than `false`, adds the values to the message on both compilers ("the comparison reduces to ‘(3 == 4)’", "expression evaluates to '3 == 4'") with one error. Testing a concept instead roughly doubles GCC's output. Decision 0033's form P stays the best default; a numeric condition is a cheap upgrade where the numbers are the help.
8. Local: one `get` overload on a forwarding reference makes GCC's raw constraint failure 25 lines where note 0012's const and non-const pair gave 34 (different fixtures, so consistent with, not proof of, note 0012's guess).
9. Recommendations for WU8 (section 6), strongest evidence first: keep conversions explicit and the lossy one named; keep named functions primary and operators as aliases; take one value per component and reject any other count; reject an opposite-orientation name with its own message; compare with `==` exactly and never add a tolerance overload of `==`; make messages one error in the algebra's terms with the remedy after a semicolon.
10. Owner questions (section 8): S8's "aggregates" against the zero-fill finding; whether a type should carry its names as written; a numeric-condition variant of form P; `gp` and `antigp` as the only abbreviated operation names; `truncate` taking a type.

## 3. General foundations

### 3.1 Cognitive dimensions and API-specific dimensions

- **Claim.** The cognitive dimensions framework gives a shared vocabulary for trade-offs in a notation: viscosity ("resistance to change"), hidden dependencies ("important links between entities are not visible"), premature commitment ("constraints on the order of doing things"), role-expressiveness ("the purpose of a component … is readily inferred"), error-proneness ("notation invites mistakes"), consistency ("similar semantics are expressed in similar syntactic forms"), closeness of mapping ("closeness of representation to domain"), diffuseness ("verbosity of language"), hard mental operations, progressive evaluation, provisionality, secondary notation, visibility, and abstraction. The tutorial warns that terseness is also a hazard because it invites mistakes, "an awkward trade-off". Source: T. R. G. Green and A. F. Blackwell, *Cognitive Dimensions of Information Artefacts: a tutorial*, v1.2, 1998, pp. 11 and 39 to 42, https://www.cl.cam.ac.uk/~afb21/CognitiveDimensions/CDtutorial.pdf. Read (assistant). The 1996 Green and Petre paper was not read.
- **Claim.** Clarke adapted the framework to APIs with twelve dimensions: abstraction level, learning style, working framework, work-step unit, progressive evaluation, premature commitment, penetrability, API elaboration, API viscosity, consistency, role expressiveness, domain correspondence; and three personas by work style (opportunistic, pragmatic, systematic), "not experience or proficiency". Sources: S. Clarke, "Describing and Measuring API Usability with the Cognitive Dimensions", 2005 position paper, https://www.cl.cam.ac.uk/~afb21/CognitiveDimensions/workshop2005/Clarke_position_paper.pdf, Read (assistant); persona wording from Stylos and Clarke 2007 (below), Read (assistant). The paper gives one anecdote (a task unfinished in two hours before a redesign, finished in twenty minutes after) with no sample size.
- **Inferred, for Galoot.** The dimensions that matter most for a compile-time algebra library are closeness of mapping (names and operators of the field, the oriented names of CONVENTIONS 1.6), error-proneness (silent zero fill, `^` precedence, dual signs), hidden dependencies (orientation signs of stored blades; a default scalar), and premature commitment (having to name a blade set before a value can be made).

### 3.2 Measured API studies

- **Claim.** Programmers were "dramatically faster — between 2 to 11 times — combining multiple objects when a method on the starting class referred to the other class": 11.2 against 1 minute (email task), 15.2 against 2 (web authentication), 6.8 against 2.8, each p < 0.05, 10 participants. All 10 started from the object they wanted to act on; class names were the main cue for where to look. Source: J. Stylos and B. A. Myers, "The implications of method placement on API learnability", FSE 2008, pp. 105 to 112, http://www.cs.cmu.edu/~NatProg/papers/Stylos2008APIObjectDesigns.pdf, abstract and section 5. Read (assistant, spot-checked: the abstract sentence quoted).
- **Claim.** In a study of 30 professional programmers, 10 per persona, a parameterless constructor followed by setters was "preferred and less problematic for all three programming personas" than required constructor parameters, which "did not prevent errors as expected"; opportunistic and pragmatic programmers assumed a default constructor existed and read the resulting compile error as a syntax problem. The paper reports trends, not statistics. Source: J. Stylos and S. Clarke, "Usability implications of requiring parameters in objects' constructors", ICSE 2007, pp. 529 to 539, http://www.cs.cmu.edu/~NatProg/papers/Stylos2007CreateSetCall.pdf, sections 1, 4.1, 4.9. Read (assistant).
- **Claim, secondary.** Abstract factories and factory methods "were significantly more difficult for participants to use than a standard constructor, causing participants to take up to 10 extra minutes to construct a single object". Source: Stylos, Faulring, Yang, and Myers, VL/HCC 2009, section 2.1, summarizing Ellis, Stylos, and Myers, ICSE 2007, which was not read. Read (assistant).
- **Inferred, for Galoot.** A free function `get<"e41">(x)` cannot be found by member completion from `x.`, which the method-placement study says is where users look first. The cost is small in C++, where free functions are the norm for `std::get`, and the alternative (`x.template get<"e41">()` in generic code) is worse. The create-set-call result does not transfer to value types like `mv`, which have no setters; the transferable part is that users expect a default constructor, which a zero-initialized `Line{}` gives.

### 3.3 Design maxims

- **Claim.** Bloch's slides list: the API should be easy to learn, easy to use without documentation, "hard to misuse", easy to read; "When in doubt leave it out"; "You can always add, but you can never remove"; "Conceptual weight more important than bulk"; "If it's hard to name, that's generally a bad sign"; "Names Matter - API is a Little Language"; "Do what is customary"; "Minimize Mutability"; "Don't Make the Client Do Anything the Module Could Do"; "Don't Violate the Principle of Least Astonishment", "worth extra implementation effort" and "even worth reduced performance"; "Fail Fast - Report Errors as Soon as Possible After They Occur", compile time best; "Three or fewer parameters is ideal"; "Don't use string if a better type exists". Source: J. Bloch, "How to Design a Good API and Why it Matters", slides 4, 12 to 16, 21, 23, 27 to 29, 32, 34, copy at https://github.com/nepaul/nepaul.github.io/files/853852/how.to.design.a.good.API.and.Wht.it.Matters.pdf (dated 2005-10-19; a third-party copy, not the OOPSLA 2006 companion). Read (assistant).
- **Claim.** "With a sufficient number of users of an API, it does not matter what you promise in the contract: all observable behaviors of your system will be depended on by somebody." Source: H. Wright, https://www.hyrumslaw.com/. Read (the first clause found verbatim in the page; the rest Read (assistant)).
- **Claim.** The "pit of success": customers should "simply fall into winning practices by using our platform", quoting Rico Mariani. Source: J. Atwood, "Falling Into The Pit of Success", https://blog.codinghorror.com/falling-into-the-pit-of-success/. Read (assistant, fetch summary); Mariani's post not read.
- **Claim.** Progressive disclosure "improves learnability, efficiency of use, and error rate", and designs beyond two levels "typically have low usability". Source: J. Nielsen, https://www.nngroup.com/articles/progressive-disclosure/. Read (assistant, fetch summary).
- **Claim.** Google's API design review program found that "users suffered a death from a thousand papercuts"; reviews check naming, abstraction level, error messages, and consistency with existing APIs; 35 of 39 surveyed stakeholders felt quality improved (self-report). Source: A. Macvean, M. Maly, J. Daughtry, "API Design Reviews at Scale", CHI 2016 EA, pp. 849 to 858, https://research.google.com/pubs/archive/45294.pdf. Read (assistant).

### 3.4 C++ Core Guidelines interface rules

- **Claim.** I.4 "Make interfaces precisely and strongly typed" ("Types are the simplest and best documentation … and are checked at compile time"); I.9 "If an interface is a template, document its parameters using concepts"; I.23 "Keep the number of function arguments low"; I.24 "Avoid adjacent parameters that can be invoked by the same arguments in either order with different meaning"; C.46 "By default, declare single-argument constructors explicit"; C.164 "Avoid implicit conversion operators" ("Prefer explicitly named conversions until a serious need is demonstrated"); C.167 "Use an operator for an operation with its conventional meaning"; ES.23 "Prefer the `{}`-initializer syntax"; ES.46 "Avoid lossy (narrowing, truncating) arithmetic conversions", with `gsl::narrow_cast<T>(x)` for an accepted narrowing and `gsl::narrow<T>(x)` that throws if a value is lost. Source: https://raw.githubusercontent.com/isocpp/CppCoreGuidelines/master/CppCoreGuidelines.md ("Jun 14, 2026"). Read (C.46, C.164, C.167, ES.23, ES.46, and the I.4 heading myself; the other I titles Read (assistant)).
- **Inferred.** `gsl::narrow<T>(x)` is the closest standard-practice analogue of `truncate`: a lossy conversion named as such, taking the target as a type, checked that nothing of value was lost. It differs in reporting by exception, which S9 excludes.

### 3.5 Naming research

- **Claim.** In a within-subjects study of 72 professional developers, full-word identifiers gave "on average, 19% faster comprehension speed compared to letters and abbreviations", with no significant difference between letters and abbreviations; effect size dz = 0.32. The authors note that common abbreviations may be learned. Source: J. Hofmeister, J. Siegmund, D. V. Holt, "Shorter identifier names take longer to comprehend", SANER 2017, https://www.se.cs.uni-saarland.de/publications/docs/HoSeHo17.pdf. Read (assistant, spot-checked: the 19 percent sentence).
- **Not read.** Lawrie et al. 2006, Butler et al. 2010, and Arnaoudova et al. 2016; their findings are not used.

## 4. Diagnostics and C++-specific evidence

### 4.1 Error-message research and practice

- **Claim.** Developers read compiler error messages, and "the difficulty of reading these messages is comparable to the difficulty of reading source code"; difficulty reading them significantly predicts task correctness (p < .0001, R² = 0.16); 13 to 25 percent of fixations fell on the messages. 56 participants, Java in Eclipse. Source: T. Barik et al., "Do developers read compiler error messages?", ICSE 2017, pp. 575 to 585, https://jssmith1.github.io/assets/pdf/ICSE_2017_EYE.pdf. Read (assistant, spot-checked: the "comparable" sentence).
- **Claim, secondary.** The Becker et al. working group's guidelines, as restated by Sy Brand: increase readability, reduce cognitive load, provide context, use a positive tone, show examples, show solutions or hints, allow dynamic interaction, provide scaffolding, use logical argumentation, report errors at the right time. Source: S. Brand, WG21 P2429R0, "Guidelines" section, https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2429r0.pdf, restating B. A. Becker et al., "Compiler Error Messages Considered Unhelpful", ITiCSE WG Reports 2019, doi 10.1145/3344429.3372508 (abstract read at https://iticse19-wg10.github.io; full text not read). Read (assistant).
- **Claim.** Coding a student's next edit after each error message measures whether the message worked; a vague word in a message ("function") sent a student to the wrong function. Source: G. Marceau, K. Fisler, S. Krishnamurthi, "Measuring the effectiveness of error messages designed for novice programmers", SIGCSE 2011, https://cs.brown.edu/~sk/Publications/Papers/Published/mfk-measur-effect-error-msg-novice-sigcse/paper.pdf, sections 2 and 4. Read (assistant).
- **Claim.** Elm shows the code "exactly as you wrote it", puts short general context above the code and specific hints below, and removes cascading errors; "Compilers should be assistants, not adversaries". Sources: E. Czaplicki, "Compiler Errors for Humans" (2015) and "Compilers as Assistants" (2015), elm-lang.org, read from a 2016 archive copy. Read (assistant).
- **Claim.** rustc's guide: the message "should be general and able to stand on its own"; messages "start with a lowercase letter and do not end with punctuation"; "The word “illegal” is illegal"; the fix belongs in a separate `help` sub-diagnostic, not in the error text; suggestions should not be questions ("did you mean" should be avoided); one problem should not emit several errors. Source: https://rustc-dev-guide.rust-lang.org/diagnostics.html. Read (assistant, spot-checked: all four quotations).
- **Inferred, for Galoot.** C++23 gives a library only the literal text of a `static_assert`, the compiler's own context lines, and the names of the entities in them. Within that, the guidance becomes: one error per mistake; the first error line names the operation as the user wrote it and the mistake in the algebra's terms; the remedy follows after a semicolon (the closest a literal can come to a separate `help`); the user's own terms appear in the context lines, which depends on what the template arguments print as (section 5.3).

### 4.2 Lessons from C++ math and units libraries

- **Eigen.** Static assertions are `static_assert(X, #MSG)` with ALL_CAPS identifiers, some of which carry the fix: "INVALID_VECTOR_VECTOR_PRODUCT__IF_YOU_WANTED_A_DOT_OR_COEFF_WISE_PRODUCT_YOU_MUST_USE_THE_EXPLICIT_FUNCTIONS", "YOU_MIXED_MATRICES_OF_DIFFERENT_SIZES". The documentation's pitfalls page says "do not use the auto keywords with Eigen's expressions, unless you are 100% sure about what you are doing": `auto C = A*B` recomputes or dangles. A 2014 forum case shows a size-mismatch message pointing away from the real cause (a nested scalar type). Sources: `Eigen/src/Core/util/StaticAssert.h` and `GeneralProduct.h` at https://gitlab.com/libeigen/eigen (master); TopicPitfalls at https://libeigen.gitlab.io/eigen/docs-nightly/; forum.kde.org t=119967. Read (assistant).
- **GLM.** `vec * vec` is component-wise, following GLSL ("when a developer knows GLSL, he knows GLM"); messages read "'normalize' accepts only floating-point inputs"; behavior is configured by about twenty `GLM_FORCE_*` macros; `a.xy = b.xy;` compiled and silently did nothing (issue 786, the maintainer "gave up on left side swizzle operators"); `equal(mat, mat)` with an epsilon or an ULP count was ambiguous (issue 905). Sources: https://raw.githubusercontent.com/g-truc/glm/master/manual.md, `glm/detail/func_geometric.inl`, issues 626, 786, 905. Read (assistant).
- **Blaze.** Constraint macros with fixed messages ("Non-dense vector type detected"); `a * b` of two dense vectors is component-wise; size mismatches of static vectors throw at run time. Source: headers of the GitHub mirror https://github.com/STEllAR-GROUP/blaze. Read (assistant). Wiki not read.
- **xtensor.** Lazy by default; "closure semantics are not meant to prevent users from doing mistakes"; returning an `auto` expression that references locals "may produce incorrect results or crash"; a maintainer-confirmed internal message "I should be lesser than sizeof...(Args)". Sources: https://xtensor.readthedocs.io/en/latest/closure-semantics.html and pitfall.html; issue 2395. Read (assistant).
- **mp-units.** "the library is optimized for readable compilation errors and great debugging experience" (P2980 6.3, P3045 9.3); no constructor from a raw value, because `std::chrono::duration`'s "was pointed out to us as a red flag safety issue by several parties already"; `quantity<si::kilo<si::metre>, int> q2 = q1;` from metres "will fail to compile because they try to truncate the quantity value", and the lossy path names its rounding (`q1.in(km, truncated)`); getters of the raw value "will always require a unit … which results in more typing" (P2980, accepted as the price of safety). P1935R2's design principles: "only safe implicit conversions should be allowed", "compile-time safety … (break at compile time, not at runtime)", "user friendly compiler errors", "No macros in the user interface". Sources: https://mpusz.github.io/mp-units/latest/getting_started/faq/ and users_guide/framework_basics/value_conversions/; https://wg21.link/P1935, P2980, P3045. Read (assistant, spot-checked: the FAQ constructor answer and the value-conversion sentence).
- **Boost.Units.** Conversions are explicit by default for "Safety and the potential for unintended conversions leading to precision loss and hidden performance costs"; a quantity cannot be built from a bare value, because a search-and-replace from `si::` to `cgs::` would silently change one metre to one centimetre; a long-open ticket (4988) asks for readable conversion errors, and a maintainer said "the types store a tremendous amount of information and therefore have very long names". Sources: https://www.boost.org/doc/libs/latest/doc/html/boost_units/FAQ.html; https://trac.cpp.al/trac10/ticket/4988; lists.boost.org 2007/03/118837. Read (assistant).
- **Boost.QVM.** Element access by compile-time index as a free function, `A<1>(v)`, and swizzles `XYZ(v)` as free functions, because the library works on user types and `operator[]` must be a member; known quirks recorded by the maintainer include `auto tr = transposed(m);` failing to compile and an `int` argument that compiles and then fails to link. Source: https://www.boost.org/doc/libs/latest/libs/qvm/doc/html/index.html. Read (assistant).
- **Boost.Geometry.** One generic message, "Not implemented for this Geometry type.", fires both for unsupported combinations and for a forgotten include (issues 1239 and 1278), so it cannot tell the user which. Source: `include/boost/geometry/core/static_assert.hpp` and issues at https://github.com/boostorg/geometry. Read (assistant).
- **std::linalg (P1673R13).** "It uses free functions, not arithmetic operator overloading", because "operator* could have multiple meanings for matrices and vectors", operators need a result element type, may need temporaries, and "strongly suggest expression templates. These introduce problems such as dangling references and aliasing"; "C++ auto type deduction makes it easy for users to capture expressions before the expression templates system has the chance to evaluate them". Source: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1673r13.html, section 9.4. Read (all three quotations found verbatim).
- **Inferred, for Galoot.** Galoot already has the main protections these libraries learned late: eager value semantics (no expression templates, so `auto r = gp(a, b);` is safe), explicit conversions with a named lossy path, no configuration macros beyond `GALOOT_CHECKING`, and named functions behind operators. The lesson still open is diagnostics: Boost.Units and Eigen show that type names which encode their structure make long messages, and Boost.Geometry shows that one generic message for several causes leaves the user guessing. Section 5 measures both for Galoot's shapes.

### 4.3 Geometric algebra libraries

Read (assistant) unless marked. URLs: https://github.com/wolftype/versor (README, issues 20 and 22, `vsr_multivector.h`); D. Fontijne, *Efficient Implementation of Geometric Algebra*, PhD thesis, University of Amsterdam, 2007, https://pure.uva.nl/ws/files/4375498/52687_fontijne.pdf; https://github.com/vincentnozick/garamon (`data/cheatSheet.txt`); https://github.com/jeremyong/klein and its docs; https://raw.githubusercontent.com/enkimute/ganja.js/master/README.md and issues 165, 170, 175; https://github.com/pygae/clifford (`clifford/_multivector.py`, historic `docs/issues.rst` at v1.4.0); https://github.com/tBuLi/kingdon (README, HISTORY, `multivector.py`, issue 144); https://github.com/EricLengyel/Terathon-Math-Library (README); https://github.com/laffernandes/gatl (README) and L. A. F. Fernandes, SEMA SIMAI Springer Series 13 chapter (2021); gafro, arXiv 2310.19090.

| Library | Algebra declared by | Component access | Products and operators | How misuse is reported |
| --- | --- | --- | --- | --- |
| Versor | fixed type aliases (`Vec`, `Biv`, `Pnt`, `Dls`) | `v[i]`, members | `*` gp, `^` wedge, `<=` inner, `!` inverse, `~` reverse | implicit conversion between types and algebras compiles and zeroes data (issue 20: "this will create a rotor. When assigning to a vector, it will become zero.") |
| Gaigen 2 | a specification file, generated classes | `m_c[]`, setters | generated `gp`, `op`, `lc` | general to specialized drops coordinates ("ignored … run-time safety checks could be implemented"); the thesis calls the system "over-designed" |
| Garamon | a configuration file, generated `Mvec` | `mv[E12]`, `grade(k)` | `^`, `|`, `<`, `>`, `*`, `!` dual | separate namespaces per algebra; the cheat sheet: "parenthesis needed (because of the priority of the operators)" |
| Klein | none (R(3,0,1) only) | `p.x()`, `p.e032()` | `*`, `^` meet, `&` join, `|`, `!` dual, `~`; sandwich `m(p)` | missing overloads fail to compile; no messages |
| ganja.js | `Algebra(p, q, r)` | `mv.e12` | `*`, `^`, `&`, `<<`, `>>>` sandwich, `!` dual, `~` conjugate (not reverse) | none documented; `Dual(1)` returns `false` (issue 170); arguments coerced to strings (issue 175) |
| clifford (Python) | `layout, blades = Cl(3)` | `blades['e12']`, `X(2)` grade, `X[e12]` | `*`, `^`, `|`, `<<`, `&`, `~` | `ValueError("cannot operate on MultiVectors with different Layouts")` (Read: found verbatim in `_multivector.py`); docs warn "the bit operators ^ << | are evaluated after the normal arithmetic operators" |
| kingdon (Python) | `Algebra(3, 0, 1)`, `Algebra.fromname('3DPGA')` | attributes `x.e12`; a blade the type lacks reads as 0 | `*`, `^`, `|`, `&`, `>>` sandwich, `@` projection, `~` | `AttributeError` for a bad name; version 3.1.0 went back to ignoring blades outside the algebra after users found the error "Weird" (issue 144) |
| Terathon | fixed classes (`Line3D`, `Motor3D`) | `.x`, swizzles, `.v`, `.m` | `^` is wedge or antiwedge by argument types; `!` right complement; `~` antireverse for rigid types, reverse for conformal ones; no product operators | missing overloads fail to compile; README: "`^` has lower precedence than just about everything else, so parentheses will be necessary" (Read: verbatim) |
| GATL | namespaces per model | `take_grade`, `for_each_component` | `*`, `^`, `<`, `>`, `|` dot, `~` | compile errors; README warns that `a + (b ^ c)` needs the parentheses; practical limit about 7 dimensions |
| gafro | fixed CGA, `Multivector<T, blades...>` | `get<blade>()` constrained by `requires` | `+ - ^ | *`; `apply` sandwich that keeps the type | constraint failures |

Further, Read (assistant):

- Terathon gives no geometric-product operators "because they would tend to generate inefficient code and produce intermediate results having unnecessary types", and offers `Transform(p, Q)` with the object first. Read (verbatim in the README).
- Klein recommends the sandwich as a call, `m(p)`, "slower" when written as `m * p * ~m`; its six `operator!` overloads document no sign convention.
- Fontijne's thesis lists "hard to decipher compiler error messages … and the long compile times" as the costs of the template-metaprogramming strategy (section 4.4), and measured a 5-D conformal ray tracer at 101 s against 37 s for the linear-algebra version (Table 7.3).
- kingdon states its cost model openly: the first call of an operation generates code ("a relatively expensive step"), and above six dimensions it stops compiling operators.
- galgebra (not in the brief) changed its default dual sign after users found `B * B.dual() = -I` (pull request 544).

**Inferred, for Galoot.** Four patterns recur. (1) A silent wrong value (Versor's implicit conversion, GLM's no-op swizzle, ganja's `false`, kingdon's 0 for a missing blade) is the complaint users file; a compile error, even a long one, is not. (2) The meaning of `~` and `!` differs across libraries (reverse, conjugate, antireverse; dual, complement, inverse), so a user arrives with a wrong prior and needs the conventions table at hand. (3) No surveyed library reports a mixed-algebra error in the algebras' terms at compile time; clifford does at run time. (4) Every C++ and Python library with `^` warns about precedence.

## 5. Experiment: the same user program, three spellings of the fix

### 5.1 Method

`docs/agent/records/spikes/api-design/fixture.hpp` is a throwaway multivector layer over the real core headers (`metric.hpp`, `blade_set.hpp`, so names parse exactly as `blades<n, "...">` parses them). It offers two spellings of a declared type:

- M, masks: `mv<rga3, blades<4, "e41 e42 e43 e23 e31 e12">>`, as decision 0090 and plan 0016 have it;
- N, names: `mvn<rga3, "e41 e42 e43 e23 e31 e12">`, a type whose template argument is the name list as written, parsed in the algebra.

and four forms of rejecting misuse:

- R: constraints only;
- P: decision 0033's form, a constrained catch-all whose body is `static_assert(false, "literal")` with a placeholder return;
- S: as P, but the `static_assert` tests the failed condition, so the compiler may print it with the user's arguments;
- T: as P, but the body names an undefined class template whose arguments are the user's terms (`no_component_named<"e14", Line>`), with no literal.

`program.cpp` is the program a first-time user writes, with one mistake per case. `run.py` compiles every (compiler, spelling, form, case), 192 compiles, and writes the texts and `out/summary.tsv`. Reproduction, from the worktree root:

```bash
eval "$(tools/jobpool env)"
tools/timed spike - api-design:run -- python3 -I docs/agent/records/spikes/api-design/run.py   # 27 s
```

The probes in `probes/` compile by hand with the same flags; their outputs are in `out/probe-*.txt`. No randomness is involved.

### 5.2 Results

Lines / error lines / line of the first error, from `out/summary.tsv`; "ok" means it compiled. Spellings M and N give the same counts except where marked.

| Case | Mistake | GCC R | GCC P | GCC S | GCC T | Clang R | Clang P | Clang S | Clang T |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | `Point s{1, 2, 3};`, one value short | ok | ok | ok | ok | ok | ok | ok | ok |
| 2 | `make<Point>(1, 2, 3)` | 25/1/2 | 9/1/6 | 9/1/6 | 11/1/6 | 13/1/1 | 8/1/2 | 11/1/2 | 11/1/2 |
| 3 | `Point s{1, 2, 3, 1, 5};` | 4/1/2 | same | same | same | 4/1/1 | same | same | same |
| 4 | `Point s{1, 2, 3, w};`, `w` an `int` | 5/1/2 | same | same | same | 8/1/1 | same | same | same |
| 5 | `get<"e14">(line)`, opposite orientation | 25/1/2 | 9/1/6 | 17/1/6 | 11/1/6 | 13/1/1 | 8/1/2 | 14/1/2 | 11/1/2 |
| 6 | `get<"e1">(line)`, not in the type | 25/1/2 | 9/1/6 | 17/1/6 | 11/1/6 | 13/1/1 | 8/1/2 | 14/1/2 | 11/1/2 |
| 7 | `get<"e5">(line)`, not a blade of rigid(3) | 29/2/16 | 27/2/16 | 35/2/16 | 29/2/16 | 40/2/3 | 35/2/3 | 41/2/3 | 38/2/3 |
| 8 | `const Point s = motor;`, lossy | 4/1/2 | 9/1/6 | 9/1/6 | 11/1/6 | 10/1/1 | 8/1/2 | 8/1/2 | 11/1/2 |
| 9 | `p == v`, rga3 against R(4,0,0) | 28/1/2 | 9/1/6 | 35/2/6 | 11/1/6 | 10/1/1 | 8/1/2 | 28/2/2 | 11/1/2 |
| 10 | scalar `std::string` | 43/4/2 | 34/1/6 | 34/1/6 | 34/1/6 | 13/2/1 | 14/1/2 | 14/1/2 | 14/1/2 |
| 11 | `blades<5, "e1 e5">` in rigid(3) (M); `"e1 e5"` (N) | M 26/4/2, N 22/1/19 | M 9/1/6, N 22/1/19 | as P | as P | M 10/2/1, N 21/1/3 | M 8/1/2, N 21/1/3 | as P | as P |

Findings, all Local:

- **Too few values pass silently.** Case 1 compiles on both compilers in every form with `-Wall -Wextra -Werror`. `probes/too_few.cpp` shows why: `-Wmissing-field-initializers` (in `-Wextra`) fires for a struct with named members (`missing initializer for member ‘with_members::e4’`; Clang: `missing field 'e4' initializer`) but not for a `std::array` member or a C array member, with or without inner braces. GCC's `-Wmissing-braces`, not in `-Wall -Wextra` for C++, flags only the brace style.
- **A counted constructor fixes it and keeps the type's properties.** `probes/counted.cpp`: a type with public `std::array` storage, a defaulted default constructor, and a constructor template constrained to exactly K values, `explicit` when K is 1, is trivially copyable, not an aggregate, usable as a class-type template argument, and not implicitly convertible from a scalar. Too few values give one error on each compiler: GCC "static assertion failed: give one value for each component of the type, in the order the type lists them … • the comparison reduces to ‘(3 == 4)’" (8 lines); Clang "static assertion failed due to requirement 'sizeof...(A) == 4UL': give one value … note: expression evaluates to '3 == 4'" (9 lines).
- **Too many values and narrowing are already clear.** Case 3: GCC "too many initializers for ‘const Point’", Clang "excess elements in struct initializer", 4 lines each. Case 4: GCC "narrowing conversion of ‘w’ from ‘int’ to ‘double’", Clang "non-constant-expression cannot be narrowed from type 'int' to 'double' in initializer list", with a fix-it inserting `static_cast<double>( )`. Brace initialization rejects an `int` variable for a `double` coefficient; the counted constructor of the probe, which converts each value with `static_cast`, accepts it, so a constructor must choose its narrowing rule on purpose.
- **Names in the type print as names.** In spelling N every context line carries the user's names: GCC "X = mvn<rga3, galoot::core::detail::name_list<24>{std::array<char, 24>{"e41 e42 e43 e23 e31 e12"}}>&", Clang "apx::mvn<rga3, name_list<24>{{"e41 e42 e43 e23 e31 e12"}}>". Spelling M prints "X = mv<rga3, galoot::core::blade_set<6>{std::array<galoot::core::oriented_blade, 6>{std::__array_traits<galoot::core::oriented_blade, 6>::_Type{galoot::core::oriented_blade{9, -1}, galoot::core::oriented_blade{10, -1}, …}}}>&" on GCC and "gc::blade_set<6>{{{{9, -1}, {10, -1}, {12, -1}, {6, 1}, {5, -1}, {3, 1}}}}" on Clang. Longest line, form P: case 5, GCC 534 to 265 characters, Clang 218 to 187; case 8, GCC 892 to 363, Clang 268 to 207; case 9, GCC 676 to 270, Clang 248 to 206. Clang's raw (R) messages keep aliases as written in "aka" text ("'Point' (aka 'apx::mv<rga3, gc::blades<4, "e1 e2 e3 e4">>')") in both spellings, as note 0010 found.
- **A C array prints shorter on GCC.** `probes/set_printing.cpp` answers note 0010's open question 4: a two-blade set held in a `std::array` prints as "blade_set<2>{std::array<oriented_blade, 2>{std::__array_traits<oriented_blade, 2>::_Type{oriented_blade{9, -1}, oriented_blade{10, -1}}}}" (first line 194 bytes), in a C array as "blade_set<2>{oriented_blade [2]{oriented_blade{9, -1}, oriented_blade{10, -1}}}" (136 bytes). Clang prints both compactly (167 and 165).
- **Form S adds values cheaply when the condition is numeric.** For `make`, S gives GCC "• the comparison reduces to ‘(3 == 4)’" and Clang "expression evaluates to '3 == 4'" in one error. When the condition is a concept (case 5), S adds the concept's expansion: GCC 9 to 17 lines, Clang 8 to 14; Clang's last note then reads "(6 < 6) evaluated to false", which is machinery. Two `static_assert`s in one catch-all (case 9, S) give two errors.
- **Form T puts the user's terms on the first error line but no sentence.** Clang: "implicit instantiation of undefined template 'apx::no_component_named<name_list<4>{{"e14"}}, apx::mvn<rga3, name_list<24>{{"e41 e42 e43 e23 e31 e12"}}>>'"; GCC: "invalid use of incomplete type ‘struct apx::coefficient_count_differs<…, 3>’". Two to three lines longer than P, and it reads as a library defect ("undefined template") rather than as the user's mistake.
- **An invalid name already reports well, then cascades.** Case 7 prints `blades`' own message ("blades: a name has an index above n, the number of generators") and then the catch-all's message as a second error. `has_component` should be false without the catch-all firing when the name itself is invalid, so that one mistake gives one error.
- **One forwarding overload of `get`.** GCC's raw failure is 25 lines ("there is 1 candidate"), against 34 for note 0012's const and non-const pair. Consistent with note 0012's open question 3; the fixtures differ, so not a controlled comparison.
- **Unqualified `get<"e41">(l)` finds the function by argument-dependent lookup** in every case (C++20 rule for a template-id call), so users need not qualify it.
- **A class-template constraint failure cascades on GCC.** Case 10, form R: 43 lines and 4 errors, the later ones "‘Bad’ does not name a type". A `static_assert` in the class body gives one error (34 lines on GCC, 14 on Clang). Plan 0016 measured 174 and 99 lines when `std::constructible_from<T, coefficient_type>` fails first, because the compiler lists the type's constructors.

## 6. Recommendations for WU8

Each surface: the best-supported choice, the alternatives, the risks, and what a first-time user writes and sees. "Evidence" means section 3, 4, or 5 supports the choice directly; "Judgment" means it does not. Diagnostic texts are the spike's, from `out/` (file named), with the spike's literal messages; WU8's literals will differ in wording. File paths at the head of each text are shortened to the file name.

### 6.1 The three most likely mistakes

Judgment, from the case studies and the shape of the interface: (1) giving the coefficients of a value in the wrong number or order, (2) naming a component with the other orientation (e14 for a Line that stores e41) or one the type does not hold, and (3) assigning a result to a type that drops some of its components. Mixing two algebras is the fourth, and becomes the most common once products exist (WU9).

### 6.2 The `mv` type and its template parameters

- **Choice.** `mv<Alg, Blades, T>` with the algebra first, no default scalar in the core, and model aliases (`rga3::Line`) as the names users type. Evidence for the order: the algebra is the first thing a mixed-algebra diagnostic must name, and it does (cases 9, both compilers). Judgment for the rest. Agrees with plan 0016 items 4 and 11.
- **Alternatives.** (a) The names as the template argument, `mv<rga3, "e41 e42 e43 e23 e31 e12", double>`: every diagnostic then shows the user's names (section 5.2), and the redundant n of `blades<4, …>` disappears. Risks: type identity follows the text, so `"e41 e42"` and `"e41  e42"` would be different types unless the text is normalized; generated results have no written names and would still print as masks; two families of types would need conversions. (b) `blades<rga3, "...">`, which decision 0090 left room for: removes the repeated n and lets a wrong index name the algebra, without (a)'s identity problem. (c) Store blade sets in a C array: GCC's printing loses the libstdc++ wrapper (section 5.2), a cheap gain that changes no interface.
- **Recommendation.** Keep M for WU8 as plan 0016 has it; record (b) and (c) as cheap improvements for WU4's owner or WU16 (owner question 2, section 8); defer (a). Judgment, with Local evidence for what each prints.

```cpp
struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};
inline constexpr auto line_layout = galoot::core::blades<4, "e41 e42 e43 e23 e31 e12">;
using Line = galoot::core::mv<rga3, line_layout, double>;
```

### 6.3 The scalar concept

- **Choice.** `ring_scalar<T>` with the arithmetic requirements first and construction from `coefficient_type` last, as plan 0016 orders them. Evidence: Local (case 10 and plan 0016's probe: constructibility failing first lists every constructor of the type, 174 lines on GCC).
- **Alternative.** Constrain `T` by a short named concept (`std::semiregular`) and check the arithmetic by a `static_assert` in the class body with a literal ("mv: the scalar type must have +, - and *"), which gives one error on GCC instead of four (case 10). This keeps S7's "every template parameter is constrained by a named concept" but moves part of the check out of the constraint, so generic code that asks `requires { typename mv<A, B, std::string>; }` is told yes. Owner question 4 if the cascade matters.
- **Risk.** Equality is not required (plan 0016), so a lane-pack scalar is accepted; `==` on `mv` then does not exist for it, and the message is the compiler's.

What a user sees for `mv<rga3, layout, std::string>` today (form R, `out/clang-M-R-case10.txt`, Clang):

```
program.cpp:84:20: error: constraints not satisfied for class template 'mv' [with Alg = rga3, B = gc::blade_set<4>{{{{1, 1}, {2, 1}, {4, 1}, {8, 1}}}}, T = std::string]
fixture.hpp:91:12: note: because 'std::string' (aka 'basic_string<char>') does not satisfy 'ring_scalar'
fixture.hpp:41:7: note: because 'a - b' would be invalid: invalid operands to binary expression ('const std::basic_string<char>' and 'const std::basic_string<char>')
```

### 6.4 Construction and reading components

- **Choice.** Values are made with exactly one coefficient per component, in the layout's order, and any other count is rejected at compile time with a message; `Line{}` is zero. Evidence: Local (case 1 compiles silently; the counted probe rejects it with "3 == 4"), Bloch's "fail fast" and "hard to misuse", and the case studies' finding that silent wrong values are what users report.
- **How, and the conflict.** In C++23 an aggregate cannot reject too few values, and no warning catches them (section 5.2). The two ways out are a counted constructor (the type stops being an aggregate but stays trivially copyable and structural: Local), or keeping the aggregate and documenting a factory (`make<Line>(…)`, case 2) as the way to construct. The first conflicts with STYLE S8's word "aggregates"; the second leaves the silent form available and is the one users will type, because it is shorter. Owner question 1.
- **Narrowing.** Brace initialization rejects an `int` variable for a `double` coefficient (case 4), with a clear message and a fix-it on Clang. A counted constructor should keep that rule, by requiring that each value convert to T without narrowing (a `requires { T{a}; }` test), rather than `static_cast` as the probe does. Judgment.
- **Reading.** `get<"e43">(x)` reads and writes by name; the storage member stays public for loops and spans (S8). Plan 0016 names it `coefficients`, a full word (Hofmeister et al.); evidence for the word, Judgment for the rest.

What a first-time user writes:

```cpp
Line l{0.0, 0.0, 1.0, 0.0, 0.0, 0.0};   // the z axis through the origin
Line m{0.0, 0.0, 1.0};                   // three values for six components
```

With the aggregate as planned, the second line compiles and `m` holds (0, 0, 1, 0, 0, 0). With a counted constructor, the user sees what the probe shows for its four-component type given three values, `const counted<4> p{1, 2, 3};` (`out/probe-counted-g++-case1.txt`, GCC, lines of source echo omitted):

```
counted.cpp: In instantiation of ‘constexpr counted<K, T>::counted(A ...) [with A = {int, int, int}; long unsigned int K = 4; T = double]’:
counted.cpp:50:29:   required from here
counted.cpp:28:32: error: static assertion failed: give one value for each component of the type, in the order the type lists them
  • the comparison reduces to ‘(3 == 4)’
```

Clang (`out/probe-counted-clang++-case1.txt`): "static assertion failed due to requirement 'sizeof...(A) == 4UL': give one value for each component of the type, in the order the type lists them", then "note: expression evaluates to '3 == 4'".

### 6.5 `apply`

- **Choice.** Keep it in `detail` under that name, as plan 0016 item 12 does; users meet `gp`, `wedge`, and the other public names, never `apply`. Judgment.
- **Risk.** If it ever becomes public, `apply` already means "call with a tuple's elements" in the standard library (`std::apply`), and gafro uses `apply` for the sandwich, so it would mean a third thing. A public name should say what is applied: `apply_plan`, or no public form at all. Judgment.

### 6.6 `truncate` and the lossy conversion

- **Choice.** Conversions between blade sets are explicit; the lossless one is `static_cast<Motor>(line)`; the lossy one has its own name and is never implicit; an implicit or `static_cast` lossy conversion is rejected with a message that names the fix. Evidence: strong. C.164 and ES.46; mp-units and Boost.Units make it their first safety rule; Versor issue 20 and Gaigen 2's thesis are the counterexamples; Local case 8 shows form P gives one error naming `truncate`.
- **Name.** `truncate` (CONVENTIONS 4.5, ratified) says that something is cut off and is a common English word; its risk is that numerics uses it for dropping a fraction (`std::trunc`, mp-units' `truncated` rounding), so a user might read `truncate<Line>(m)` as rounding coefficients. The alternatives are `narrow<Line>(m)`, after `gsl::narrow`, whose semantics (lossy, checked) match; `keep<…>` or `select<…>`, which hide the loss; and `project`, which CONVENTIONS 4.3 already uses for a geometric operation. Recommendation: keep `truncate`, and say in its `///` comment and the guide that it drops components, not digits. Judgment, weak evidence either way.
- **Its argument.** Every C++ conversion users know takes the target as a type: `static_cast<T>`, `gsl::narrow<T>`, `value_cast<T>` in mp-units, `convert_to<T>` in QVM. `truncate<line_layout>(m)` takes a blade set, which a user must name separately. Accepting the type, `truncate<Line>(m)`, follows "do what is customary" (Bloch) and the conversion operator's own form. It is a CONVENTIONS 4.5 change, which plan 0016 item 10 defers to WU16; the evidence says to take it as soon as there is a type to name, and to accept both the type and the set if 4.5 changes. Owner question 5.

What a first-time user writes and sees for the lossy assignment, form P (`out/clang-N-P-case8.txt`, Clang; spelling N shown, spelling M prints the sets as masks):

```cpp
const Motor q{1, 0, 0, 0, 0, 0, 0, 0};
const Point s = q;
```

```
fixture.hpp:160:19: error: static assertion failed: conversion drops components of its input: it is lossy, so request it with truncate<Type>(x)
program.cpp:75:19: note: in instantiation of function template specialization 'apx::mvn<rga3, name_list<32>{{"1 e41 e42 e43 e23 e31 e12 e1234"}}>::operator mvn<name_list<12>{{"e1 e2 e3 e4"}}>' requested here
   75 |   const Point s = q;
1 error generated.
```

### 6.7 `get<"e41">` named access

- **Choice.** `get<"e41">(x)` with the structural name type of `blade_set.hpp`, one forwarding overload, constrained by `has_blade` so generic code can detect it, plus form P catch-alls with one message per cause, as plan 0016 has it. Evidence: Local (single overload shortens GCC's raw failure; ADL finds it unqualified; P gives one error with the name in the context line), note 0012, and Boost.Geometry's counterexample of one message for several causes.
- **Opposite orientation.** Reject, with its own message, as plan 0016 item 9 recommends. Evidence for rejecting: Python libraries that return something for any spelling (kingdon's 0 for a missing blade, reverted after complaints in issue 144) show that a permissive read hides mistakes; a negated read could not be written through, so `get` would have two behaviors (role-expressiveness). Evidence for its own message: the Becker et al. guidelines ("show solutions or hints") and rustc's one-message-per-problem rule. Judgment for the wording: "get: the type stores this blade with the other orientation; read the name the layout gives it and negate the value".
- **An invalid name.** The name parser's message comes first today and the catch-all's follows (case 7); `has_blade` should be false without a second message when the name is invalid. Local.
- **Risk.** Names have one digit per index, so `get` serves n up to 9; above, components are reached by position. The message for that case exists in plan 0016.

What a first-time user writes and sees, form P (`out/gcc-N-P-case5.txt`, GCC, spelling N; the code is the spike's program, shortened, and lines of source echo are omitted from the texts here and below):

```cpp
Line l{0, 0, 1, 0, 0, 0};
double vz = get<"e43">(l);   // 1
double v  = get<"e14">(l);   // the line stores e41 = -e14
```

```
fixture.hpp: In instantiation of ‘constexpr auto apx::get(X&&) [with name_list<...auto...> Name = galoot::core::detail::name_list<4>{std::array<char, 4>{"e14"}}; X = mvn<rga3, galoot::core::detail::name_list<24>{std::array<char, 24>{"e41 e42 e43 e23 e31 e12"}}>&]’:
program.cpp:66:18:   required from here
fixture.hpp:235:17: error: static assertion failed: get<name>: the type has no component of this name with this orientation
```

With spelling M the second line instead lists `galoot::core::oriented_blade{9, -1}` and five more, through `std::__array_traits` (534 characters).

### 6.8 Equality and exactness

- **Choice.** `==` and `!=` compare coefficient by coefficient, exactly, between values of one type; comparing across algebras, blade sets, or scalars does not compile. A tolerance comparison, when it comes (V12), is a separately named function whose tolerance is a named argument, never an overload of `==`. Evidence: V11 and V12; GLM issue 905 (three `equal` overloads with an epsilon and a ULP count were ambiguous); I.24 (two numbers of the same type in either order). Judgment for the naming.
- **Mixed algebras.** Without a catch-all, Clang already names both types through their aliases ("invalid operands to binary expression ('Point' (aka 'apx::mv<rga3, gc::blades<4, "e1 e2 e3 e4">>') and 'const Vector4' (aka 'const apx::mv<r400, …>'))", case 9 R); GCC prints 28 lines. A catch-all of form P gives one error on each and names `rga3` and `r400` in its context line. Plan 0016 leaves `==` across layouts to the compiler's message; that is acceptable for equality, but the products of WU9 should get the catch-all, because mixing algebras there is the most likely mistake of all (section 6.1). Local.
- **Floating point.** The `///` comment says that with a floating scalar `==` is IEEE equality (−0.0 equals +0.0, NaN equals nothing). Evidence: V11, plan 0016 item 13.

```
fixture.hpp:243:17: error: static assertion failed: ==: the operands have different types; compare multivectors of one algebra, blade set and scalar
program.cpp:80:11: note: in instantiation of function template specialization 'apx::operator==<apx::mv<rga3, gc::blade_set<4>{{{{1, 1}, {2, 1}, {4, 1}, {8, 1}}}}>, apx::mv<r400, gc::blade_set<4>{{{{1, 1}, {2, 1}, {4, 1}, {8, 1}}}}>>' requested here
1 error generated.
```

(`out/clang-M-P-case9.txt`.)

### 6.9 Error messages

Evidence: section 4.1, and the forms of section 5. Proposed style for every Galoot literal, consistent with the headers' practice and decision 0033:

1. One mistake, one error. A catch-all must not fire after a more specific check has already failed (case 7); two `static_assert`s in one body give two errors (case 9, S).
2. Start with the operation as the user writes it, then a colon: `get<name>:`, `truncate:`, `mv:`. Lowercase, no final period, no quote characters (plan 0002 risk 8).
3. State the mistake in the algebra's terms (blade, orientation, algebra, layout, scalar), never the machinery (mask, position, plan, constraint).
4. Put the fix after a semicolon, as an instruction: "…; request it with truncate<Type>(x)". This is the nearest a literal comes to rustc's separate `help`.
5. Where a number is the help, assert the numeric condition rather than `false` (form S), so both compilers print the numbers ("3 == 4"). Where the condition is a concept, keep `false` (form P), because the concept's expansion doubles GCC's output.
6. Make the user's terms appear in the context lines: that is what the template arguments print as, which is why the type's spelling (6.2) matters.
7. A distinct message for each distinct cause (opposite orientation, absent, invalid name, two names, too wide an algebra), as plan 0016 lists for `get`.

### 6.10 Names of the operations and operators to come (WU9 and later)

- **Operators.** Keep CONVENTIONS 7.1. Evidence: `*` is the geometric product in every library surveyed that has one; `^` is the wedge everywhere; `&` is the antiwedge (meet in Lengyel's terms) in Klein, ganja.js, kingdon, and clifford; `~` is the reverse in most (Klein, clifford, kingdon, GATL, Garamon, Versor) but the conjugate in ganja.js and the antireverse in Terathon's rigid types; `!` is a dual or complement in Klein, ganja.js, Garamon, and Terathon but the inverse in Versor. Galoot's `~` and `!` match the majority. `%` for the antiproduct is Galoot's own (Versor uses `%` for the commutator); it has the precedence of `*`, which is right for a product. Galoot has no `|`, which avoids the field's disagreement over which inner product it means (symmetric in Klein and clifford, dot in GATL, generic inner in Garamon). Judgment for the last point.
- **Parentheses.** Keep S13's full parentheses in code, tests, and documentation, and say why once in the guide. Evidence: every C++ and Python library with `^` documents the same trap (section 4.3).
- **Function names.** The named functions are the interface (S13). Full words are read faster (Hofmeister et al.); `wedge`, `antiwedge`, `reverse`, `right_complement`, `bulk_dual` are full words, while `gp` and `antigp` are the only abbreviations in the set, which also makes the set inconsistent. They are the field's names in generated code (Gaigen's `gp`, kingdon's `gp`), so users of those tools know them. Owner question 3: `geometric_product` and `geometric_antiproduct` as the names, with `*` and `%` as the short forms. Evidence for full words, Judgment for the trade.
- **Transforms.** `transform(x, V)`, object first, as CONVENTIONS 4.4 has it, matches Terathon's `Transform(p, Q)`; `compose(Q2, Q1)` applying Q1 first matches function composition. Judgment: say in both `///` comments which argument acts first, because I.24's swapped-argument hazard applies to two versors of one type.
- **`format`.** Print blades by their oriented names as the layout declares them, and print the algebra's tag name, so that output and diagnostics use the same words. Judgment.

## 7. Proposed reviewer checklist for public interfaces

A proposal for W5 reviews of a public header, for the owner to adopt, change, or drop. It is not in the governance documents and this note does not edit them.

**Task first.** Before reading the header, write the three tasks the interface exists for, as a user would, from the examples only. Note each place you had to read the implementation.

**Names**

- [ ] Each name is the field's term, spelled as the literature spells it (CONVENTIONS), or a full English word; an abbreviation is one the field uses everywhere.
- [ ] A name means one thing across the whole library, and nothing the standard library means differently (`apply`, `get`, `truncate`).
- [ ] Similar things look alike: the same argument order, the same prefix or suffix for the same idea (`anti`, `left_`, `right_`).

**Construction and conversion**

- [ ] The shortest way to make a value is a correct way. A value cannot be made with too few or too many coefficients, or with a narrowing conversion, without a compile error.
- [ ] A default-constructed value is defined (zero), not indeterminate.
- [ ] Every conversion that can lose information has its own name and is never implicit; the lossless ones are explicit.
- [ ] No hidden default changes a result: scalar type, orientation, dual side, checking mode.

**Misuse and diagnostics**

- [ ] Each likely misuse in the unit's catalogue has a compile-fail test on both compilers that requires the literal message.
- [ ] Each misuse gives one error, and the message is on the first error line after the inclusion context.
- [ ] The message starts with the operation as written, states the mistake in the algebra's terms, and gives the fix after a semicolon. No masks, positions, or internal names.
- [ ] The context lines show the user's own terms (algebra tag, names), not only masks.
- [ ] Distinct causes give distinct messages.
- [ ] Concepts that generic code may probe test data, never call expressions (decision 0033), and the documentation says which probes give a wrong answer.

**Semantics and cost**

- [ ] Every operation that discards components, changes representation, or assumes a property (unit norm, parity) says so in its name or its `///` comment.
- [ ] `auto r = f(a, b);` is always safe: no proxy, no reference into a temporary.
- [ ] Exact comparisons are `==`; tolerance comparisons are named and take a named tolerance.
- [ ] The `///` comment states the cost a user can predict (which blades the result holds; what runs at compile time).

**Examples and readability**

- [ ] Every example compiles and is the idiom you want copied; operators mixed with `^` or `&` are fully parenthesized.
- [ ] A maintainer can find where each behavior lives from its name, and a change to one rule touches one place.

**Before release.** List every interface change the review suggests that would break users later; Hyrum's law says each one costs more after release.

## 8. Owner questions

These arise where the evidence conflicts with, or bears on, ratified governance, conventions, or decisions. None is edited here.

1. **STYLE S8, "multivectors … are aggregates".** Local evidence: an aggregate with array storage accepts too few coefficients and zero-fills the rest with no warning on either compiler under the project's flags; a counted constructor rejects it with "3 == 4" and keeps the type trivially copyable, structural, and with public storage. Options: (a) amend S8 to "trivially copyable structural values with public storage, constructed from exactly one value per component" (recommended); (b) keep aggregates and make a checked factory the documented way to construct, accepting that the shorter brace form stays silent; (c) keep aggregates and document the zero fill. This bears on plan 0016 item 5 (zero initialization), which holds under all three.
2. **How a type prints.** The names-as-written spelling (`mv<rga3, "e41 …">`) makes every diagnostic show the user's names, against masks and libstdc++ internals today, but brings type identity by text and a second family of types. Cheaper steps that change no interface: a C array inside `blade_set` (GCC's printing loses 58 bytes of wrapper per set in the probe) and `blades<Alg, "...">` (no repeated n). Is any of these wanted before WU16, given that C9 asks for errors "in terms of the signature, basis elements, or object classes"?
3. **`gp` and `antigp`.** The only abbreviated names in the operation set (CONVENTIONS 7.1). Rename to `geometric_product` and `geometric_antiproduct` before release, keeping `*` and `%`? The cost of renaming grows after release.
4. **A numeric-condition variant of form P (decision 0033).** Where the help is a number (a coefficient count, a generator count), `static_assert(count == expected, "literal")` prints the values on both compilers in one error. Allow it beside form P? And, for the scalar check of the class template, is a `static_assert` in the body under a weaker named constraint acceptable under S7, to remove GCC's four-error cascade?
5. **`truncate<Type>(x)`.** CONVENTIONS 4.5 takes a blade set. Every C++ conversion users know takes a type. Accept a type as well, as soon as a type can be named (plan 0016 item 10 defers it to WU16)?

## 9. What was not verified

- No user was observed; all usability claims for Galoot are inferred.
- The diagnostic texts come from a throwaway fixture, not from `mv.hpp`; WU8's texts will differ in wording and in line numbers, and must be read again (plan 0016 section 8 schedules that reading).
- Only GCC 16.2.1 and Clang 23.1.4 with libstdc++ were used; MSVC and libc++ were not.
- The compile-time cost of a names-as-written type and of a counted constructor was not measured.
- Claims marked Read (assistant) were not re-read by me except where spot-checked.
