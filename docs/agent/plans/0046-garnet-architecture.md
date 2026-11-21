# 0046 Garnet: architecture and roadmap

- Status: proposed 2026-11-21 by Claude (agent, Fable 5.2), chief architect and planner, authorized by the owner for this unit. From `main` at `c95fee5`, on branch `plan/0046` in the worktree `galoot-wt/plan-0046`. No unit below has started. Section 7 (quad and hex) and the parallel parts of section 9 are provisional: the learning cycles on quad and hex meshing and on parallel and memory-efficient meshing were still running when this was written, and a revision pass follows their closing notes.
- Source of scope: the coordinator's brief of 2026-11-21 and the owner's constraints for garnet recorded there; the closing notes of the four finished learning cycles in the knowledge base (`mesh-generation/`, `cg-kernels/`, `ga-geometry-applications/`, `robust-geometry/`); `math-tools/docs/agent/meshing/findings-for-up-stack.md`; Galoot's README, guide, roadmap (plan 0001, milestone M6), decisions 0211, 0243, 0248 and 0251, the governance documents, the handoff of 2026-11-15, and plans 0043 to 0045.
- Siblings: plans 0043, 0044 and 0045 (release 1.0.1). This plan changes no file they own and asks nothing of Galoot before 1.0.1 ships (section 8.4).
- This file lives in Galoot's records only until garnet's repository exists. Unit G1 (section 15.1) copies it into garnet as that repository's first plan.

## 1. Departures, ambiguities, and readings (C20)

1. **Reading depth.** The notes the brief names were read in full or in their load-bearing sections: the meshing findings, `implications-for-a-ga-library.md`, the mesh-generation index, `gap-analysis-for-galoot.md`, `predicate-inventory.md`, `api-lessons.md`, `geometric-algebra-coverage.md`, `where-ga-pays-off.md`, `exact-predicates-on-doubles.md`, the evaluation-order, contraction and recommendation sections of `geometric-algebra-predicates.md`, the index and open questions of the robust-geometry README, the perturbation note's scheme list, `mesh-data-structures.md` and `quad-and-hex-meshing.md`. The other notes of those directories were read by their headings only. Galoot's `CONVENTIONS.md`, the body of the guide beyond the scalar section, and the bodies of plans 0043 to 0045 beyond their goals, unit tables and shared-file sections were not read.
2. **No experiment was run and no page was fetched.** Every number here is quoted from the notes, with its exercise where the note gives one, or is layout arithmetic marked [derived]. Statements about parallel Delaunay practice and about quad and hex methods beyond the first-pass note are from the planner's background knowledge, marked [background], and are the first things the revision pass must check against the two running cycles.
3. **The integration branch.** Galoot's governance says `develop`; the branch is `main` (plan 0045 section 6 handles the wording). This plan says `main` for both repositories.
4. **"Vendor no code" and Galoot.** Read as: garnet's tree holds no copy of anyone's code, Galoot's included. Garnet reaches Galoot through the build system (section 8.1).
5. **"Vendor no code" and the test framework.** Galoot keeps doctest in its tree. Read strictly, garnet may not, and doctest is not installed on this machine. Section 16 item O3 asks the owner; the plan's default is a test-only fetch pinned by commit.
6. **Performance targets.** Only the owner sets bounds (Galoot C14, adopted in section 12). The targets of section 9.4 are proposals; they bind when the owner ratifies them (O9).
7. **Costs.** The token and time estimates of section 15 scale from Galoot's records (plan 0042 used about 2.3 M tokens over eleven units). They are judgments with a stated spread, not measurements.

## 2. Purpose, non-goals, and the dependency budget

### 2.1 Purpose

Garnet is a mesh generator and the geometry library under it. Given points, segments, polygons, surfaces or an implicit function, it produces triangle, tetrahedral, polyhedral (Voronoi), quadrilateral and hexahedral meshes, measures their quality, and reads and writes the common mesh file formats. It is a C++23 library with a small command-line program, under the MIT license, written from published algorithms.

Three properties set it apart from the incumbents, and the knowledge base records that no single existing tool has all three ([gap analysis](../../../../knowledge-base/cg-kernels/gap-analysis-for-galoot.md), "What the landscape leaves open"):

1. **Every geometric decision is exact on every finite double input.** A *predicate* is a yes-or-no geometric question, such as "is this point inside that circle?", answered by the sign of a polynomial in the input coordinates. Triangle, TetGen and Geogram give wrong answers, crash or abort on inputs with very small or very large coordinates (meshing findings, sections 1 and 2); garnet does not.
2. **Permissive and self-contained.** MIT, no bundled code, and a default build that needs a C++23 compiler, CMake and Galoot's headers, nothing else.
3. **Readable.** One written convention per concept, results whose types name their geometric meaning, options as named fields, and one way to do each thing.

### 2.2 Non-goals

- No CAD kernel and no reading of CAD formats (STEP, IGES, B-rep) in the default build. An optional adapter is not excluded later; nothing in the roadmap builds one.
- No curved-object kernel (exact arrangements of circles or algebraic curves), no mesh Booleans, no Nef polyhedra. These are research-grade and well served elsewhere.
- No lazy exact number type. Exactness whose cost is local to the call is preferred to a global expression graph whose cost surfaces elsewhere ([API lessons](../../../../knowledge-base/cg-kernels/api-lessons.md), lesson 5).
- No tolerance inside a predicate, ever. Tolerances exist only in input cleaning and in quality thresholds (section 9.1).
- No solver framework. Where a method needs a sparse linear solve, garnet carries one small conjugate-gradient solver of its own and nothing more general.
- No GPU code, no distributed-memory (MPI) meshing, no Windows or macOS support, and nothing on the owner's out-of-scope list (x87, MSVC, Cilk, the Java rung).
- Garnet does not grow Galoot. What Galoot must not absorb is listed in section 8.5.

### 2.3 What "lightweight" and "self-contained" mean: the dependency budget

A dependency is anything that must be present for a step to work. The budget is per tier, and adding to a tier is an owner decision.

| Tier | Step | Allowed | Excluded |
| --- | --- | --- | --- |
| 0 | Use the library and the command-line program (the default build) | A C++23 compiler (GCC or Clang), the C++ standard library, CMake, Galoot's headers at a pinned release | Everything else: Boost, Eigen, GMP, MPFR, TBB, OpenMP, zlib, HDF5, any CAD kernel |
| 1 | Run garnet's tests | Tier 0 plus one test framework (doctest), fetched or found, never stored in the tree | Any incumbent mesher or kernel as a source of expected values |
| 2 | Develop garnet | Tier 1 plus Python tools run through `uv` with a locked environment (exact reference values, plots, mutation scripts), `clang-format`, `clang-tidy`, the sanitizers, and Gappa with Rocq from the math-tools toolset for filter constants | Any tool that needs a paid license or a network service |
| 3 | Optional comparisons (`GARNET_BENCH=ON`, off by default) | Triangle, TetGen, Geogram, CGAL (with GMP and MPFR), VoroCrust, Verdict, each found at a path the user gives, never downloaded or bundled by garnet | Any of these in tiers 0 to 2 |

Consequences that the rest of the plan relies on:

- **Exact arithmetic is garnet's own.** The exact stages use expansion arithmetic and a fixed-capacity big integer written for garnet (section 4.2). The measured design in the knowledge base used GMP for its last fallback; garnet replaces that with its own integer so that tier 0 stays empty of libraries. Unit X3 measures the cost of that choice.
- **Threads are the standard library's.** No OpenMP and no TBB (section 9.2).
- **File formats that need a library are out of the default build.** Compressed and HDF5-based formats wait for an optional adapter (section 7.3).
- **Size.** Proposed limits for the first release: the default build compiles from empty in under two minutes on eight cores, and no public header pulls in more than Galoot's core, the standard library and garnet's own headers. Both are measured from unit G1 on.

## 3. Module map and layering

A module is a directory of headers (and, for the compiled modules, sources) with one responsibility. A module may use only modules in lower rows of the table. The dependency graph has no cycle. "Galoot" says how the module uses Galoot; section 8 gives the detail.

| Layer | Module | Responsibility | Form | Galoot |
| --- | --- | --- | --- | --- |
| 9 | `cli` | The `garnet` program: triangulate, tetrahedralize, refine, voronoi, remesh, convert, quality, check | compiled | no |
| 8 | `bench` (optional), `adapters` (optional) | Comparisons against the incumbents; conversions to other libraries' types | separate targets, off by default | point types only |
| 7 | `io` | Readers and writers for mesh and geometry files; input cleaning | compiled | no |
| 6 | `delaunay2`, `refine2`, `delaunay3`, `voronoi`, `remesh`, `stuffing`, `tet` (boundary recovery and refinement), `quadhex` (provisional) | The meshing algorithms | compiled | through layers 2 and 3 only; `quadhex` may use rotors |
| 5 | `quality` | Element quality measures and mesh validity checks | header-only | no |
| 4 | `mesh` | Mesh containers, adjacency views, the mutable triangulation and surface kernels, attributes | header-only templates over the index width | no |
| 3 | `geometry`, `spatial` | Primitives and constructions; bounding boxes, space-filling curves, grids, bounding-volume trees | header-only | heavily (`geometry`); no (`spatial`) |
| 2 | `predicates` | Exact sign-returning predicates on doubles and on homogeneous constructions; perturbed variants; batched variants | header-only, `constexpr` | derivation, second-route oracle, predicates on meets |
| 1 | `numeric` | Error-free transformations, expansions, intervals, the wide integer | header-only, `constexpr` | each type is a Galoot scalar |
| 0 | `base`, `par` | Index types, errors, checking macros, small containers, arenas, the random generator, counters; the thread pool and parallel loops | header-only (`base`), compiled (`par`) | no |

Two rules keep the layering honest. First, layers 0, 4, 5, 7 and the combinatorial parts of layer 6 never include a Galoot header: combinatorics, containers, file formats and quality formulas gain nothing from the algebra ([where GA pays off](../../../../knowledge-base/ga-geometry-applications/where-ga-pays-off.md)). Second, algorithms (layer 6) never compute a sign themselves: every decision goes through `predicates`, and every new coordinate through a named construction in `geometry`.

The public interface of each module follows, at the level of names and responsibilities.

### 3.1 `base` and `par`

- `VertexId`, `ElementId`, `CornerId`: distinct index types, 32-bit by default, 64-bit by a template parameter of the mesh types, so that a vertex index cannot be passed where an element index is expected.
- `Error` and `Result<T>` (an alias of `std::expected<T, Error>`): the one way a garnet operation reports failure (section 9.1).
- `GARNET_CHECKING`: the checking mode, as Galoot's. Preconditions the types cannot express are checked in constant evaluation always and at run time when it is 1.
- `SmallVector`, `Arena`: containers for cavity lists and per-thread scratch memory.
- `Random`: one seeded generator, written from its published definition, used wherever an algorithm randomizes. No time-based seed anywhere.
- `Counters`: per-stage predicate counts and construction counts, compiled out unless `GARNET_STATS` is 1, so that garnet can report the same predicate mix the findings measured for the incumbents.
- `par::ThreadPool`, `par::for_each_index`, `par::reduce`, `par::sort`: section 9.2.

### 3.2 `numeric`: scalars for the predicate stages

- **Error-free transformations.** `two_sum`, `two_diff`, `two_product` (with a fused multiply-add). An error-free transformation returns the rounded result of an operation together with its exact rounding error, as two doubles.
- **`Expansion<N>`.** An exact sum of at most N doubles of decreasing magnitude, with exact addition, scaling and multiplication. Fixed capacity, no heap. Marked exact through `std::numeric_limits`, which is Galoot's hook for exact scalars.
- **`Interval`.** A pair of doubles that encloses a real number. Built from error-free transformations in round-to-nearest, so it sets no rounding mode and stays correct under the compiler optimizations that the knowledge base measured breaking Boost.Interval with GCC 16 ([gap analysis](../../../../knowledge-base/cg-kernels/gap-analysis-for-galoot.md), candidate 2).
- **`WideInt<Bits>`.** A fixed-capacity signed integer for the last fallback, sized by the bit bound of the predicate it serves (about 11,000 bits covers a degree-5 determinant over the full binary64 exponent range [derived]: five factors, each at most 53 bits of significand after scaling by up to 2,098 binary places).
- **Range checks.** `expansion_precondition(degree, inputs)`: the loop over trailing zeros and magnitudes that says whether an expansion stage is exact for these inputs ([exact predicates on doubles](../../../../knowledge-base/robust-geometry/exact-predicates-on-doubles.md), "The real precondition").

Nothing here is a general number library: no division, no square root on the exact types, no arbitrary precision.

### 3.3 `predicates`

Every predicate takes plain doubles (or the homogeneous types of section 3.4), is correct for every finite input, and returns a type that names the question:

| Result type | Values | Predicates |
| --- | --- | --- |
| `Turn` | `clockwise`, `collinear`, `counterclockwise` | `orient2d` |
| `PlaneSide` | `below`, `on`, `above` | `orient3d`, `coplanar_orientation` |
| `CircleSide` | `outside`, `on`, `inside` | `in_circle`, `in_sphere`, `in_diametral_circle`, `in_diametral_sphere`, `in_lens`, `power_test_2d`, `power_test_3d` (each at every arity a triangulation needs for coplanar, collinear and equal points) |
| `Order` | `less`, `equal`, `greater` | `compare_lexicographic`, `compare_distance`, `compare_power_distance`, `sign_of_dot` |
| `Crossing` | `disjoint`, `touching`, `proper`, `overlapping` | `segment_segment`, `segment_triangle`, `triangle_triangle`, `ray_triangle` |

- **The cascade.** Each predicate is evaluated in stages, cheapest first: a semi-static filter (a floating-point evaluation compared with a proved error bound, with guards for underflow and overflow), an interval stage, an expansion stage with its precondition checked, and the wide-integer fallback, which is correct for every finite input. This is the cascade the knowledge base measured correct on all 688,864 cases where Shewchuk's code, TetGen and Geogram were wrong on thousands (exercise num-B1).
- **Translated evaluation.** Every predicate subtracts one input point first. Evaluating the algebra's formula on raw coordinates makes the error grow with the square or cube of the distance from the origin ([GA predicates](../../../../knowledge-base/robust-geometry/geometric-algebra-predicates.md), recommendation 1).
- **Perturbed variants.** `in_circle_perturbed`, `in_sphere_perturbed` and the perturbed power tests never return `on` (section 9.1). They are separate names: a plain predicate always reports an exact zero as zero.
- **Predicates on constructions.** `orient2d(HPoint2, ...)`, `in_circle` with a homogeneous argument, `plane_side(Plane, Plane, Plane; Plane)` and the like: the composed polynomial with the sign of the weight multiplied in, interval stage first, then exact.
- **Batched variants.** `orient3d_batch`, `in_sphere_batch` over structure-of-arrays input: a filter over four lanes, then the scalar cascade for the undecided lanes. The measured gain is 1.6 to 4 times on easy input (num-B7).
- **Conventions.** One document fixes every sign against one reference (Shewchuk's definitions for the classical four), and tests pin both signs of each.

### 3.4 `geometry`: primitives and constructions

- **Stored points are plain.** `Point2` and `Point3` are two and three doubles. Mesh arrays store these, not Galoot's four-coefficient points, so that a 3D vertex costs 24 bytes and arrays have the layout the batch predicates read.
- **Homogeneous objects are Galoot's.** `HPoint2`, `HPoint3`, `Line2`, `Line3`, `Plane` are Galoot types (the rga3 preset in 3D; R(2,0,1) in 2D), over `double` for rounded work and over `Interval` or `Expansion` where a predicate will read them. A *homogeneous* point carries a weight w beside its coordinates, and stands for the point (x/w, y/w, z/w); keeping the weight avoids the division, so a later predicate on the point is still a polynomial in the inputs and can be decided exactly.
- **Tuples of points.** `Segment`, `Triangle`, `Tetrahedron`, `Ray`, `Box`.
- **Constructions, named by their exactness.** `join` and `meet` return homogeneous objects and never divide. `rounded_circumcenter`, `rounded_off_center`, `rounded_midpoint`, `rounded_centroid`, `rounded_projection`, `rounded_closest_point` return plain points and say in their name that the result is rounded. `to_point(HPoint3)` is the one place a weight is divided out.
- **Classification instead of variants.** `classify(meet)` returns `proper`, `parallel` or `contained` by one exact predicate, so an intersection never needs a variant result type (API lessons, lesson 4).
- **Motions.** `Motion2`, `Motion3` (Galoot motors) with `apply(motion, mesh)`, which collects the motor into a matrix once for the batch.
- **Later, with Galoot's conformal preset:** `Circle`, `Sphere`, `bisector`, `radical_plane`, `power_center`.

`spatial` holds what is basis-dependent and therefore outside the algebra: `Box` operations, `hilbert_order` and `morton_order`, `brio_order` (the biased randomized insertion order that makes point location by walking fast), a uniform grid, and a bounding-volume tree with `closest_point` and `ray_cast` queries for remeshing and projection.

### 3.5 `mesh`: arrays and indices

The core is arrays of numbers indexed by integers, nothing else.

- **Containers.** `TriMesh`, `QuadMesh`, `TetMesh`, `HexMesh`: a coordinate array and a fixed-stride array of vertex indices per element. `PolyMesh2`, `PolyMesh3`: variable-size polygons and polyhedra in compressed-row form (an offsets array into an index array), for Voronoi cells and mixed elements. `MixedMesh`: fixed-stride blocks by element type, for I/O.
- **Attributes.** Named, typed arrays attached to vertices, elements or corners (boundary markers, region ids, weights, sizing values), added at run time.
- **Adjacency views**, built on request from a container and never stored inside it: `neighbors(mesh)` (the element across each face; for triangles this is the corner table's opposite array), `vertex_elements(mesh)` (compressed rows), `edges(mesh)`, `boundary(mesh)`.
- **Mutable kernels** for the algorithms: `Triangulation2` and `Triangulation3` (each element stores its vertices and its neighbors, with ghost elements closing the convex hull), and `SurfaceKernel` (index-based half-edges with opposite half-edges stored in pairs) for edge flip, split and collapse with the link condition checked.
- **Layout for parallel work.** Structure-of-arrays, 32-bit indices by default, no pointers, vertices renumbered along the Hilbert curve after construction so that neighbors in space are neighbors in memory, and per-thread arenas whose blocks are appended to the shared arrays at fixed points (section 9.3).
- **Checks.** `is_valid(mesh)`: index ranges, adjacency symmetry, manifoldness, consistent orientation.

### 3.6 `quality`

Pure functions of coordinates: angles and dihedral angles, edge ratio, radius-edge ratio, radius ratio, aspect measures, scaled Jacobian for quadrilaterals and hexahedra, signed volume by `orient3d` for the validity of an element, histograms and a summary structure. Each function names its definition and its range, and each is tested against a 30-digit reference (section 10.1). Verdict was measured wrong on 499 of 1,361 tetrahedra for one measure (findings, section 3), so garnet takes no value from it.

## 4. Two-dimensional meshing

- **`delaunay2`.** Incremental Bowyer-Watson insertion over `Triangulation2`, points ordered by `brio_order`, located by walking. Returns the Delaunay triangulation, a map from duplicate input points to their representative, and statistics. Collinear input returns the sorted path with a status that says the input is one-dimensional. Divide and conquer is faster sequentially on some inputs; incremental insertion is chosen because the same kernel serves constraints, refinement and the 3D code, and unit X5 measures what that costs against Triangle.
- **Constrained Delaunay.** `insert_segments` forces input segments into the triangulation by cavity retriangulation (Shewchuk and Brown's method). Segments that cross are split at their intersection, which is kept as a homogeneous point for every predicate that needs it and rounded once, by a declared rule, when it becomes a vertex.
- **`refine2`.** Ruppert's algorithm with Chew's second variant and off-centers as options, diametral lenses for encroachment, concentric shells for small input angles, an area bound, a sizing function, and holes and regions by seed points. Steiner points are rounded constructions; once rounded they are input points like any other, so every later decision about them is exact.
- **Interface shape.** `triangulate(points, options)`, `triangulate(planar_straight_line_graph, options)`, `refine(triangulation, criteria)`. Options and criteria are structures of named fields; no switch strings.

## 5. Three-dimensional triangulation, Voronoi and power diagrams

- **`delaunay3`.** Incremental insertion over `Triangulation3` with `brio_order`, the perturbed in-sphere test, and cavities built by conflict walks. A *regular* triangulation (the weighted version, where each point has a weight and the in-sphere test becomes the power test) is the same code with the weighted predicate. Sequential first, then parallel (section 9.2).
- **`voronoi`.** The Voronoi diagram is the dual of the Delaunay triangulation, and the power diagram of the regular triangulation. `voronoi2`, `voronoi3`, `power_diagram`: cells as `PolyMesh`, clipped to a box or to a convex or general boundary. A Voronoi vertex is a circumcenter, a construction; where a clipping decision depends on it, the decision is the composed predicate on the defining points, not a test on a rounded vertex. The measured Voronoi meshers decide with absolute tolerances near 1e-10 and fail under scaling (findings, section 5), so exact clipping is where garnet improves on them.
- **VoroCrust-style conforming Voronoi meshing** (ball covers, mirrored seeds, cells that need no clipping) is a later, research-grade phase (R2 in section 14).
- **`tet`: boundary recovery and quality refinement.** A general-purpose tetrahedral mesher must force input facets into the mesh and refine for quality. This is the hardest module in the plan: the knowledge base calls it "a long project" with no theory for removing slivers. It is scheduled after the 3D Delaunay code is fast and after a study unit (section 14, phase 6), and the plan's position is that garnet writes it, because linking TetGen (AGPL) or CGAL (GPL) cannot be part of a default MIT build.
- **`stuffing`.** Isosurface stuffing: tetrahedra from an implicit function over a body-centered cubic lattice refined by an octree, with proved dihedral-angle bounds. Small, numerically benign, and the first producer of volume meshes with a quality guarantee. Its octree is shared with the hexahedral methods.

## 6. Surface remeshing

`remesh` works on `SurfaceKernel`: isotropic remeshing (split long edges, collapse short ones, flip to even out valence, smooth tangentially, project back with `spatial`'s closest-point query), simplification by edge collapse with quadric error metrics, and Laplacian and tangential smoothing. Features are edges marked by an attribute or by a dihedral-angle threshold. Nothing here is predicate-critical except the checks that an operation does not fold or invert the surface, which use `orient3d`.

## 7. Quad and hex family, I/O, adapters, and the benchmark layer

### 7.1 Quad and hex (provisional)

The first-pass note says plainly that no hex method combines robustness, quality and generality, and that field-guided methods are dominated by numerical optimization, not predicates. The plan therefore orders the family by how much of it is settled and how little it needs beyond garnet's own modules:

1. **Containers, quality and I/O for quadrilaterals and hexahedra** (scaled Jacobian, validity). Settled; part of phases 2 and 3.
2. **Grid-based methods.** A balanced quadtree and octree with template-based transitions (Schneiders' method and its successors), boundary fitting by projection and smoothing, giving all-quad and all-hex meshes that are robust but have poor elements near the boundary [background]. Needs only `spatial`, `mesh`, `quality` and the closest-point query.
3. **High-order 2D quad meshing in the style of HOHQMesh**: a subdivision of a background grid fitted to curved boundaries, with boundary curves carried to high polynomial order, and hexahedra by extrusion and sweeping of the 2D mesh [background]. Needs curve representations (polynomial and spline arcs evaluated by garnet) and a high-order element container.
4. **Field-guided quad meshing**: a cross field, then a parametrization whose integer grid lines are the mesh. Needs a sparse linear solver and a rounding strategy; the dependency budget allows garnet's own conjugate-gradient solver and no mixed-integer library. Whether a method within that budget gives usable meshes is not known to this plan.
5. **Frame-field hex meshing.** Research only. This is the one place the algebra's rotors might matter, and unit X8 tests that before any design depends on it.

Items 2 to 5 are placeholders until the learning cycle's notes arrive. The revision pass must settle: which octree transition scheme to implement and what its element-quality bounds are; whether HOHQMesh-style meshing needs anything outside the budget; which field-guided pipeline, if any, fits a library with no mixed-integer solver; and what high-order elements require of `mesh` and `io`.

### 7.2 Adapters (optional headers)

Conversions between garnet's arrays and other libraries' types (Eigen matrices in the libigl convention, CGAL kernel points), and a set of function objects that satisfies CGAL's documented traits concept for Delaunay triangulations, so that CGAL's triangulation can run on garnet's predicates for comparison. Each adapter includes the other library's headers only when the user includes the adapter.

### 7.3 `io`

Readers and writers written from the format specifications, in this order of need:

| Phase | Formats |
| --- | --- |
| 2 | Triangle's `.node`, `.ele`, `.poly`; TetGen's `.node`, `.ele`, `.face`; OFF; VTK unstructured grid (`.vtu`, uncompressed) |
| 3 | Gmsh MSH 4.1 and 2.2 (text); OBJ (geometry subset); STL (text and binary); PLY |
| 5 and later | MEDIT `.mesh`; high-order element blocks for the quad and hex family |
| Not in the default build | Compressed VTK, CGNS, Exodus, XDMF with HDF5: each needs a library and waits for an optional adapter |

`io` also holds input cleaning, the only home of tolerances on coordinates: `merge_duplicates`, `snap`, `remove_degenerate_elements`, each with its tolerance as a named argument and a report of what it changed. Readers validate (finite coordinates, index ranges) and return a `Result`.

### 7.4 `bench`: the optional link layer

Off by default, built only with `GARNET_BENCH=ON`, and never installed. It is not part of the library and nothing in layers 0 to 9 may include it.

- **Triangle, TetGen and VoroCrust run as separate processes** on files that garnet writes and reads back. Out-of-process use keeps TetGen's AGPL and Triangle's research-only terms away from any garnet binary and makes a crash or a timeout of the tool an ordinary result. The options go through typed structures that render the tools' switch strings.
- **Geogram and CGAL are linked** into benchmark executables only, because the comparison needs their predicate counters and their sequential and parallel timings in-process. Those executables are never distributed.
- Every tool is found at a path the user passes (`GARNET_TRIANGLE_EXE`, `GARNET_GEOGRAM_ROOT`, and so on). On the development machine these point into the math-tools toolset. Garnet's build never downloads one.

## 8. The Galoot boundary

### 8.1 How garnet reaches Galoot

Through CMake only: `GARNET_GALOOT_SOURCE_DIR` names a checkout (on this machine the sibling directory), and when Galoot has a repository URL, `FetchContent` at a pinned release tag becomes the default. Garnet's tree never holds Galoot's headers. Garnet uses Galoot's public interface only, as any user would, and pins one Galoot release at a time; moving the pin is a unit with its own checks (section 8.3, item 2).

### 8.2 What garnet takes from Galoot as released (1.0.0)

- The core: declaring an algebra from its metric; the wedge (join) and antiwedge (meet); `dot` and `antidot`; `project`; `transform` and `compose`; exact change of basis; the `ring_scalar` and `exact_scalar` concepts; evaluation in constant expressions; the checking mode.
- The rga3 preset: `Point`, `Line`, `Plane`, `Motor`, `Flector`.

And the four uses, in order of importance:

1. **Homogeneous constructions and the predicates on them** (production code). A meet over `Interval` is a sound filter and over `Expansion` an exact stage, from one formula. This is the advantage the knowledge base measured: a predicate on a meet evaluated as the composed polynomial was right every time where rounding the point first was wrong in up to 37% of cases.
2. **A second route in tests.** The wedge of the input points over an exact scalar is an independent oracle for every hand-written predicate and its sign.
3. **Derivation.** Each predicate, its sign and its filter constant are derived from the algebra's formula in a development tool, by running the formula over a scalar that carries error bounds, as the knowledge-base exercises did.
4. **Rigid motions** of primitives and meshes.

The production code of the classical predicates on input points does not call Galoot. It is the translated difference form written out, because the generic product on raw coordinates costs 2.25 to 2.4 times the multiplications and has a filter bound more than four times larger (where GA pays off, rank 6).

### 8.3 What garnet needs Galoot to add, and when

None of these blocks garnet: for each, garnet has a workaround inside its own tree until the Galoot release exists. Each is a Galoot unit under Galoot's governance with its own plan.

| # | Addition to Galoot | Kind | Proposed release | Garnet needs it by | Until then |
| --- | --- | --- | --- | --- | --- |
| 1 | **Enclosure scalars stated in the contract.** The guide says the ring laws must hold; interval arithmetic satisfies only inclusion (every result encloses the exact value). State that a scalar whose operations enclose is supported, that every product's result then encloses the exact coefficient whatever order the terms are evaluated in, and pin it with a test over a small interval type in Galoot's tests | Guide text and one test; a new documented guarantee, so a minor release | 1.1.0 | Phase 2 (predicates on meets) | Garnet tests the property itself and treats it as unguaranteed |
| 2 | **Evaluation order stated.** A sentence that the order of operations of a generated product is a fixed function of the algebra, the layouts and the Galoot version, and may change between versions. Garnet then derives filter constants by running the product over a bound-carrying scalar and rederives them when it moves its Galoot pin. No interface for reading a product's terms is requested | Guide text | 1.1.0 | Phase 2 | The interval stage needs no constant; semi-static filters on meets wait |
| 3 | **Norms and `unitize` in rga3**: `bulk_norm`, `weight_norm`, their squares, and `unitize`, each stating what it assumes | Preset (rga3 0.2.0), zero core diff | 1.1.0 | Phase 2 (distances, projections) | Garnet writes them in `geometry` from `dot` and `antidot` |
| 4 | **A 2D rigid preset, rga2**: `Point`, `Line`, `Motor`, `Flector` of R(2,0,1), in the pattern of rga3 | Preset, zero core diff | 1.1.0 | Phase 3 (2D constructions) | Garnet declares R(2,0,1) through the core in an internal header and swaps it for the preset later |
| 5 | **The conformal presets, cga3 (milestone M6) and cga2**: points, dual spheres and circles, reading a center and a squared radius out of a sphere, bisectors and radical planes. With a documented warning that a rounded conformal point must not be fed to an exact predicate | Preset, zero core diff (decision 0211) | 1.2.0 | Phase 5 (Voronoi and power-diagram constructions, `Circle` and `Sphere`) | Garnet's power predicates are the lifted determinant and need no preset; the constructions wait |

Sequence against Galoot's releases:

- **1.0.1** (plans 0043 to 0045, cleanup only): garnet asks nothing and touches nothing. Garnet pins 1.0.0 and moves to 1.0.1 when it is tagged. Items 1 and 2 are guide text, and plan 0044's unit U5 edits the guide; they are still kept out of 1.0.1, because a new guarantee does not belong in a patch release (plan 0045, item O10, takes the same position).
- **1.1.0**: items 1 to 4, one plan. Small: two guide passages, one test, one preset in an existing pattern, a handful of preset functions. It should start when 1.0.1 has shipped and garnet's phase 1 is under way, so that garnet's needs are confirmed by use first.
- **1.2.0**: item 5 (M6). Decision 0243 item 3 leaves a contraction build to "the M6 revisit"; garnet's position for that revisit is in section 9.1 (garnet's own units name their fused operations and compile with contraction off, so it needs nothing from Galoot there).

### 8.4 No collision with the 1.0.1 plans

This plan adds one file, `docs/agent/plans/0046-garnet-architecture.md`, and changes nothing else in Galoot. Plans 0043 to 0045 own tests, tools, headers, the README, the guide and the release records; nothing in section 8.3 starts before their release is tagged.

### 8.5 What Galoot must not absorb

Galoot's constitution already places "geometric modeling, meshing, robust predicates" outside its core (C2). To keep that line where the pressure will come:

- No sign-returning predicate, filter, error bound or tolerance.
- No number type: intervals, expansions and wide integers live in garnet.
- No symbolic perturbation, lexicographic order, bounding box, spatial sort or anything else that depends on the choice of basis.
- No mesh container, adjacency structure or file format.
- No compiled component, thread or dependency.
- No stored conformal point over `double` offered as an input to exact tests.

If garnet finds it needs to edit Galoot's core, the core is missing an extension point (C5), and that is raised as a Galoot decision, not patched around.

## 9. Engineering baseline

### 9.1 Language, build, layout, and policies

**Language and compilers.** C++23, GCC 16 and Clang 23 on Linux with libstdc++, no compiler extensions, the warnings and static checks of Galoot's STYLE (S3, S4) with garnet's own configuration files. A predicate header stops compilation with `#error` under any option that relaxes IEEE 754 semantics (S15).

**Floating-point settings.** Garnet's own translation units compile with `-ffp-contract=off`, and every fused multiply-add is written as `std::fma`. The filter constants are the ones derived for the written operations. The default target is x86-64-v3, which has the fused instruction; without it `std::fma` is a library call, still correct, measured 2.3 to 9.3 times slower (num-E20). A predicate never reads the sign bit of a zero (GCC rewrites a negated `fma` in a way that changes it; num-E7).

**Build system.** CMake 4.4 or later (Galoot's own minimum) with Ninja and presets: debug, release, address and undefined-behavior sanitizers, and thread sanitizer, for each compiler. Targets: `garnet::headers` (layers 0 to 5), `garnet::garnet` (the static library of the compiled modules), `garnet::cli`. Install rules and a package for `find_package` come with the first release, because a compiled library needs them.

**Header-only or compiled, by module.** Small, hot, generic or `constexpr` code is header-only: `base`, `numeric`, `predicates`, `geometry`, `spatial`, `mesh`, `quality`. Algorithms, `io` and `par` are compiled, behind plain interfaces on `double` coordinates and the two index widths. The reason is compile time, the cost users of template-heavy geometry libraries complain of most (API lessons, lesson 6): a user of the mesher compiles declarations, not the mesher. Inside the predicate headers the filter path is inline and the exact stages are separate functions marked for no inlining, so the common path stays a few instructions.

**Coordinates are `double`.** Algorithms are not generic over the coordinate type. Single-precision input is widened on reading. Exactness is defined for binary64 inputs and nothing else.

**Repository layout.**

```text
garnet/
  LICENSE  README.md  AGENTS.md  CMakeLists.txt  CMakePresets.json
  include/garnet/<module>.hpp        one umbrella header per module
  include/garnet/<module>/...        its headers
  src/<module>/...                   sources of the compiled modules
  apps/garnet/                       the command-line program
  test/<module>/<component>_test.cpp
  test/reference/                    generated exact reference values, with their generators' hashes
  test/families/                     generators of the degenerate input families
  bench/                             optional comparisons (tier 3)
  tools/                             check, mutate, derive (filter constants), reference (uv project)
  docs/guide/                        user documentation, one page per module
  docs/theory/                       one note per algorithm, with its sources
  docs/agent/                        governance, plans, decisions, reviews, provenance, lessons, records
```

**Errors.** Three kinds, kept apart:

- *A violated precondition* (a non-finite coordinate passed to a predicate, an index out of range) is a defect in the caller. It is checked when `GARNET_CHECKING` is 1 and stops with a message in the terms of the geometry.
- *An input the operation cannot serve* (a file that does not parse, a self-intersecting boundary, all points collinear when a triangulation was asked for) returns `Result` with an `Error` that says what and where. The public entry points validate their input once (finite coordinates, index ranges), so the inner loops need no checks.
- *Resource exhaustion* propagates as `std::bad_alloc`. Garnet throws nothing else and never calls `abort`.

**Exactness tiers.** Every function that touches coordinates belongs to one tier, and its name or type says which:

| Tier | Meaning | Where |
| --- | --- | --- |
| Exact decision | The sign is that of the real-number expression, for every finite double input | every predicate |
| Exact construction | A homogeneous object over an exact or interval scalar; predicates on it are exact decisions | meets and joins that feed a predicate (crossing constraints, clipping) |
| Rounded construction | A double result within a stated error; from then on it is an input | Steiner points, centroids, projections; named `rounded_...` |
| Tolerance | A comparison against a named constant chosen by the user | input cleaning and quality thresholds only |

The trap the knowledge base measured, an exact test on a stored rounded object answering a question about a different object in up to half of near-degenerate cases, is closed by construction: no garnet type stores a rounded plane, line or lifted point that a predicate later reads.

**Symbolic perturbation: yes, for the circle and sphere family only.** With exact predicates a result of zero means the input is exactly degenerate (four points on a circle, five on a sphere), and the Delaunay triangulation is then not unique. *Symbolic perturbation* breaks the tie by a fixed rule, as if each point had been moved by an infinitely small amount. Garnet uses the cheap scheme the knowledge base describes for Delaunay and regular triangulations: only the lifted coordinate is perturbed, the tie is decided by at most four orientation tests, points never move, and no flat tetrahedron arises (Devillers and Teillaud; CGAL does the same). The order of the perturbation is the lexicographic order of the coordinates, not the position in the input, so the triangulation is a function of the point set. Reasons:

1. The triangulation becomes unique, which is what makes the output independent of insertion order and of thread count (determinism, below) and makes two runs comparable element by element in tests.
2. The algorithms lose their degenerate branches for cocircular and cospherical input, the families on which the incumbents were measured failing or slowing most.

Orientation tests are not perturbed. Collinear and coplanar input is handled where it arises: duplicates are detected by exact comparison and mapped, a point on an edge or a face is handled by the cavity algorithm, and input of lower dimension returns a status. Unit X1 checks the derived tie-breaking sequences against a brute-force oracle before any triangulation depends on them.

**Determinism.** The same input, options and garnet version give bit-identical output on both compilers, at every optimization level, and for every thread count. Means: no contraction and named fused operations; the perturbation above; one seeded generator; total orders with ties broken by index in every sort and priority queue; no iteration over hash containers in any path that affects output; a canonical renumbering before output; and parallel algorithms whose result is defined independently of scheduling (section 9.2).

**API style, for readability.**

1. Free functions on plain data. No kernel or traits class parameter; a concept only where two models exist.
2. A function's name says what it returns and how exact it is.
3. Options are structures with named fields and defaults; results are structures with named fields. No out-parameters, no switch strings, no configuration macros beyond `GARNET_CHECKING` and `GARNET_STATS`.
4. A result enumerator names the geometric meaning, never `positive` or `1`.
5. One convention per concept, written once, pinned by tests of both signs.
6. A reader should follow any algorithm's source beside its theory note: the source names the steps as the note does.

**Documentation.** A README, a guide page per module, and a theory note per algorithm that defines every term at first use and cites the papers the implementation follows. Every code block in the README and the guide is compiled and run as a test, as in Galoot.

### 9.2 Parallelism

**Default model: a thread pool of garnet's own on `std::jthread`, with three loop forms** (`for_each_index`, `reduce`, `sort`) and fixed chunking. No OpenMP, no TBB, no parallel standard algorithms. Reasons:

1. **Dependencies.** libstdc++'s parallel algorithms need TBB. OpenMP needs a runtime that differs between GCC and Clang, and Clang's is a separate package. Either would be garnet's first tier-0 library.
2. **Determinism.** The loop forms garnet needs can fix their chunk boundaries and their order of combination, so results do not depend on the number of threads. A general task scheduler does not promise that.
3. **Checkability.** A small pool on standard primitives runs cleanly under the thread sanitizer, which the style rules require for threaded tests.
4. **Readability.** One file a reader can hold in mind, against a runtime nobody reads.

**What runs in parallel, in order of confidence.**

- *Embarrassingly parallel* (settled): batched predicates, quality measures, adjacency construction, sorting along the space-filling curve, smoothing by independent sets, I/O parsing by blocks.
- *Delaunay insertion* (provisional, unit X6). Two published families [background]: lock or reserve the cavity of each insertion and retry on conflict (Geogram's and CGAL's parallel triangulations; the deterministic-reservations method of Blelloch and others, which commits in each round exactly the insertions that would win in sequential order and is therefore deterministic), or partition the points along a space-filling curve, triangulate the parts independently and repair the seams (the approach behind the fastest published shared-memory code, by Marot, Pellerin and Remacle). The plan's default is deterministic reservations, because garnet requires thread-count-independent output; the perturbation makes the target triangulation unique, so correctness of the parallel result is checkable against the sequential one exactly. X6 measures what determinism costs against the partition approach.
- *Refinement* (provisional): rounds of independent cavities chosen by a deterministic priority.

The thread count is an option, default the number of performance cores; nothing in garnet reads or changes processor settings.

### 9.3 Memory layout

- Coordinates: structure-of-arrays or interleaved doubles, chosen per container by measurement (unit X7); 16 bytes per 2D vertex, 24 per 3D vertex.
- A triangle in `Triangulation2`: three vertex indices and three neighbor references, 24 bytes with 32-bit indices. A tetrahedron in `Triangulation3`: four and four, 32 bytes. A neighbor reference encodes the neighbor and which of its faces is shared, so crossing a face needs no search.
- Flags (constrained, ghost, marked) in a separate byte array, so the hot arrays stay dense.
- No per-element heap allocation anywhere. Cavities and queues use per-thread arenas reset between insertions.
- Vertices and elements are renumbered along the Hilbert curve before output and at chosen points during construction.

Expected footprint [derived] (layout arithmetic, not a measurement): a 2D Delaunay triangulation has about two triangles per point, so 16 + 2 × 24 = 64 bytes per point plus flags and work space; a 3D one of random points has between six and seven tetrahedra per point [background], so about 24 + 6.7 × 32 = 238 bytes per point plus flags and work space.

### 9.4 Performance targets (proposed; O9)

The findings fix what to aim at: in 3D there are about 44 in-sphere calls and 4 orientation calls per input point, so the in-sphere test is the one to make fast; and the incumbents differ most on degenerate families, where between 20% and 99.9% of their calls fall through to exact arithmetic. All times are medians on one pinned performance core of the development machine under the timing lock.

**Predicates on easy input** (uniform points; reference: the best scalar and batch times of exercise num-B7).

| Predicate | Best measured scalar (ns) | Target scalar (ns) | Measured batch (ns per predicate) | Target batch |
| --- | --- | --- | --- | --- |
| `orient2d` | 6.5 | ≤ 7.5 | 4.3 | ≤ 5.0 |
| `orient3d` | 8.1 | ≤ 9.5 | 5.0 | ≤ 6.0 |
| `in_circle` | 9.0 | ≤ 10.5 | 5.6 | ≤ 6.5 |
| `in_sphere` | 17.4 | ≤ 20 | 7.0 | ≤ 8.0 |

The measured cascade's scalar in-circle and in-sphere filters were latency-bound (18 and 28 ns); the scalar targets above require a filter tree with independent products, which unit X2 designs and proves.

**Share of calls that reach the expansion stage or beyond** (reference: findings, section 1).

| Family | Incumbents measured | Target |
| --- | --- | --- |
| Uniform, Gaussian, clustered | 0% (TetGen's box filter fails on 98% of clustered calls, then its adaptive stages decide) | 0 calls in 10⁸ |
| Integer grid | CGAL 0%; Geogram and TetGen 20 to 48% | ≤ 1%: exact zeros are certified by the interval stage, then decided by the perturbation's orientation tests |
| Rounded points on a sphere (in-sphere) | CGAL 36%, TetGen 76%, Geogram 99.9% | ≤ 40% |
| Points on a circle (in-circle) | Geogram 100% | ≤ 40%, and no worse than the best incumbent measured in the same run |

**Predicates on hard input** (reference: num-B7): in-sphere on rounded-degenerate input ≤ 2,000 ns (the measured cascade: 2,008; CGAL: 1,140); on exactly degenerate input ≤ 100 ns (CGAL: 54; the measured cascade: 362); on wide-exponent input ≤ 3,300 ns (the cascade with an interval stage: 3,278; CGAL: 286). The last two gaps to CGAL are the evidence that the middle of the cascade is not yet well designed; closing them is X2's question, and these three targets are to be reset from its result.

**Whole algorithms.** Garnet's counters must show at most 1.2 times the measured predicate mix (3 orientation and 9 in-circle calls per point in 2D; 4 and 44 in 3D). On one million uniform points, sequential: 2D Delaunay within 1.5 times Triangle's time; 3D Delaunay within 1.5 times Geogram's sequential time. Parallel 3D Delaunay on ten million points: at least 5 times the sequential speed on the eight performance cores. Peak memory: at most 100 bytes per point in 2D and 300 in 3D. On every degenerate family of section 10.3: a correct result, with no crash, abort or timeout, at any cost. That last line is a requirement, not a target.

The incumbent times are measured in the same session as garnet's, because the findings' own wall times were taken under load and are stated to be noisy.

## 10. Verification

### 10.1 Oracles

An oracle is an independent source of the right answer. No incumbent tool supplies an expected value (Galoot C15, adopted).

1. **Exact reference values**, generated by Python integer and rational arithmetic in the `uv` tool environment, by two routes that must agree (Python's integers and FLINT), stored under `test/reference/` with the generator's hash and a regeneration check.
2. **The algebra as a second route.** The wedge of the inputs over `WideInt`, through Galoot, against every hand-written predicate.
3. **The exact checker.** `garnet check`: for a triangulation, that every element is positively oriented, that every interior face is locally Delaunay by an exact in-sphere test computed by a different route from the mesher's (an untranslated determinant over `WideInt`), that the boundary is the convex hull, that adjacency is symmetric and that the element counts satisfy Euler's relation. For a constrained triangulation, that every input segment is present. The checker for math-tools' exercises (`check_dt`, on GMP) is the owner's own earlier work and serves as a cross-check of garnet's checker in tier 3.
4. **CGAL with the inexact-constructions kernel**, optional, as a differential oracle: measured correct on all 50 of its degenerate runs. On input without exact degeneracies the two triangulations must be identical after canonical numbering; on degenerate input both must pass the exact checker and agree in element count.
5. **The 30-digit quality reference**: 128 elements evaluated at 30 digits and checked against closed forms to 1e-40 (findings, section 3), regenerated by garnet's own script and compared with the math-tools table.

### 10.2 Property and metamorphic tests

A property test checks a law on generated inputs; a metamorphic test checks that a known change of the input changes the output in the known way.

- Properties: the empty-circle and empty-sphere property; 2n − 2 − h triangles for n points with h on the hull; adjacency is an involution; refinement output meets its angle bound except where the input's small angles forbid it; every quality measure stays in its range and is invariant under rigid motion to a stated tolerance.
- Predicate laws: antisymmetry under a swap of two arguments, invariance under cyclic permutation, the consistency of `in_circle` with `orient2d` on the same four points, and the perturbed predicates never returning `on`.
- Metamorphic: permuting the input order gives the identical canonical triangulation; scaling all coordinates by a power of two gives identical connectivity; translating by a vector that keeps every coordinate exactly representable gives identical connectivity; changing the thread count changes nothing; the two compilers give identical bytes.

### 10.3 Fuzzing on the degenerate families

Seeded generators under `test/families/`, written for garnet from the knowledge base's descriptions of the adversarial families: integer grids; rounded points on circles and spheres; collinear and coplanar sets; duplicates and points one unit in the last place apart; lattices of subnormal coordinates; magnitudes near 1e200 and 1e-200; mixed scales; the small-normal family that broke every expansion-based code without a precondition check; wide exponent ranges; tight clusters; thin slabs; points on the moment curve. Each algorithm runs on each family at several sizes and seeds, and each output goes through the exact checker. File readers are fuzzed with libFuzzer under Clang. With no CI, every gate states its fuzzing budget (iterations and wall time) and the run is recorded with its seeds.

### 10.4 Mutation

A mutant is a small deliberate defect; a test suite is trusted on a component to the extent it rejects that component's mutants. For `numeric` and `predicates`, each unit lists its mutants in advance (a changed filter constant, a dropped guard, a flipped sign, swapped arguments, a skipped stage), and all must be rejected before the merge. For the algorithms, a sampled set per gate, with survivors explained or killed. The tool is a small script in `tools/`, in the manner of Galoot's.

### 10.5 Benchmark protocol

- **Inputs:** the families of section 10.3 plus uniform, Gaussian and clustered points, at 10⁴ to 10⁷ points; planar straight-line graphs and surface models generated by garnet's own scripts or downloaded by a script with recorded hashes and licenses (no third-party model is stored in the tree).
- **Tools:** garnet, and optionally Triangle, TetGen, Geogram and CGAL, each only within its stated preconditions, at recorded versions.
- **Measures:** wall time (median of at least five runs on pinned performance cores under the timing lock), peak resident memory, predicate calls per stage, element counts, quality histograms (minimum and maximum angles or dihedral angles, radius-edge ratio), the exact checker's verdict, and counts of failures and timeouts.
- **Record:** one row per run with the machine state before and after (the findings note that theirs was not recorded), the job pool status, the compiler and its flags. Heavy runs go under `memguard.sh`.
- Incumbent results are recorded and compared; only garnet's own targets gate.

## 11. License hygiene

The rule: garnet is implemented from published descriptions, holds no code that garnet's contributors did not write for it, and records where each idea came from.

1. **Sources by class.** (A) Papers, books, theses, format specifications and manuals: read freely and cited. (B) Code under permissive terms or in the public domain (Geogram, VoroCrust, Shewchuk's `predicates.c`): may be read to understand behavior; never copied, translated or paraphrased. (C) Code under restrictive terms (CGAL, TetGen, Triangle, Gmsh): the agent implementing a module does not open class C code for that module at all. If a behavior can be learned only from such code, a separate reading agent writes a behavior note in the knowledge base, in prose, with no code, identifiers or structure, and the implementer works from the note and the papers.
2. **Provenance log.** `docs/agent/provenance/<module>.md`, one row per source used by a unit: the citation, its URL and hash, its class, what was taken from it (an algorithm, a formula, a constant, a table of cases), and which agent read it. A table of cases transcribed from a paper (the stuffing stencils, the octree transition templates) is recorded as data from that paper and rechecked by a test.
3. **In the source.** Each file carries the MIT identifier. Algorithms cite their papers in the theory notes and, where it helps the reader, in a comment. No comment or user document cites a plan, a decision or an agent.
4. **Review.** The reviewer of a unit checks that every algorithm in the change has a provenance row and that the implementer's source list holds no class C code.
5. **The tree.** No submodule, no archive, no copy and no patch of third-party code. `bench` calls the tools' public interfaces and command lines only.
6. **Test data.** Generated by garnet's scripts, or public-domain data fetched by a script with a recorded hash. Fetches are plain, unauthenticated, and carry no personal identifier.
7. **Configuration files** (`.clang-format`, `.clang-tidy`, presets) may start from Galoot's: same owner, same license. The provenance log says so.

## 12. Governance and workflow

Garnet gets its own `docs/agent/` with its own numbering from 0001.

**Adopted from Galoot, adapted in wording only:** the clean-room and license provision (C16), layering and the dependency rule (C13), compactness (C12), nothing hidden (C8), one convention per concept (C10, C11), mathematics as the reference (C15), measured cost with bounds set only by the owner (C14), decisions recorded when made (C18), proportion (C19), honest reports (C20), small units with a definition of done (W1, W2), written plans (W3), review by someone other than the author who does not share the author's context (W5), branches with `main` for integrated work and `master` for the owner's releases (W7), delegation rules (W14), the evidence kinds and status tags (V1, V10), seeded randomized tests (V7), the rule that signs and zero tests need exact evidence (V13), the shared-machine rules (P11, P14: job pool, timing lock, `memguard.sh`, no processor settings), and the style document nearly whole.

**Simplified:**

| Galoot | Garnet | Why |
| --- | --- | --- |
| A convention trace gate with a traced specification | One conventions document, with a test named beside each convention, checked at review | Garnet has perhaps twenty sign and orientation conventions, not a derived algebra's hundreds |
| Blind review by an Opus-level agent of every code change | Proportional: Opus-level for layers 1 and 2, every public interface and every parallel change; Sonnet-level for `io`, tools, `bench` and records | C19: most depends on the predicates |
| Two audits and a full matrix at a release | For 0.x releases, one audit and the matrix of both compilers with sanitizers; two audits from 1.0 | Pre-1.0 interfaces may change |
| A run log with compaction, cost models, convention records | A run table, the agent-cost table, and the provenance log | Fewer record kinds to keep true |
| No performance gates before release | Each phase's exit gate includes its ratified targets | Performance is one of garnet's three stated aims |

**Added:** the license-hygiene procedure of section 11, with its review item; the exactness tiers as a constitutional provision; determinism as a provision; the dependency budget as a provision.

**Roles and model levels.** Specialists are general-purpose agents with a role brief. Opus at high effort is the default; Sonnet and Haiku take small scoped work; Fable only where the owner permits it.

| Phase | Planner | Implementers | Test authors | Reviewers | Gate |
| --- | --- | --- | --- | --- | --- |
| 0 scaffold and governance | Opus | Sonnet (scaffold), Opus (governance drafts) | Sonnet | Sonnet; the owner ratifies governance | coordinator |
| 1 numeric and predicates | Opus | Opus (numerical expert) | Opus | Opus, blind | Opus auditor; Fable recommended (O14) |
| 2 geometry, mesh, quality, first I/O | Opus | Opus (`geometry`, `mesh`), Sonnet (`io`, `quality`) | Sonnet with Opus-written properties | Opus for interfaces, Sonnet otherwise | Opus auditor |
| 3 2D meshing | Opus | Opus | Opus | Opus, blind | Opus auditor |
| 4 3D Delaunay and parallelism | Opus; Fable recommended for the parallel design (O14) | Opus | Opus | Opus, blind, plus a thread-sanitizer run as evidence | Opus auditor |
| 5 to 8 | Opus | Opus; Sonnet for I/O and bench | Opus | proportional | Opus auditor |
| Reading and extraction for any phase | | Haiku or Sonnet at low effort, their output treated as data and sampled | | | |

Every hand-back opens with departures, skipped checks and ambiguities, and the coordinator writes each agent's cost to garnet's agent-cost table.

## 13. What each phase still needs to learn

| Topic | State | Needed by |
| --- | --- | --- |
| Parallel and memory-efficient meshing practice | cycle running | phase 4 (X6, X7); section 9.2 is provisional until then |
| Quad and hex methods (octree, high-order, field-guided) | cycle running | phase 8; section 7.1 is provisional |
| The middle stages of the in-sphere cascade | open question in the findings and the robust-geometry notes | phase 1 (X2) |
| The predicate mix of constrained Delaunay | not measured | phase 3 (measured there with garnet's counters) |
| Perturbation rules for power tests, consistent with the unweighted ones | derived in the literature, not checked here | phase 1 (X1) |
| 3D boundary recovery and sliver removal | first pass only; "study before writing" | phase 6 (study unit S1) |
| Polyhedral volume structures (half-face, generalized maps, incidence arrays) | not studied | phase 5 (`PolyMesh3`) |
| Restricted Delaunay surface meshing | skimmed | phase 7, if surface meshing from implicit or parametric input is wanted |
| Isosurface stuffing's stencil tables and angle bounds, rechecked | open question | phase 5 |
| Exact predicates for sphere-triple intersections (VoroCrust seeding) | open question | phase R2 |

## 14. Roadmap

Each phase has an entry gate (what must be true to start) and an exit gate (what the audit checks). A release is the owner's act.

| Phase | Contents | Entry | Exit gate | Depends on learning |
| --- | --- | --- | --- | --- |
| 0 | Repository scaffold; governance and conventions; license procedure | Owner rulings O1 to O4 | Both compilers build and test from empty; the check script passes; governance ratified | none |
| 1 | `numeric`; `predicates` for input points (classical four, power tests, the small ones, perturbed and batched variants); experiments X1 to X3 | Phase 0 | Zero wrong signs on the num-B1 families and garnet's own; all listed mutants rejected; the ratified predicate targets met or the shortfall reported with X2's result | X1, X2 |
| 2 | `geometry` (primitives, meets, predicates on meets), `spatial`, `mesh`, `quality`, first `io`, the exact checker | Phase 1; Galoot 1.1.0 wanted, not required | Predicates on meets exact against the oracle; quality measures agree with the 30-digit reference; round trips of every format | none |
| 3 | `delaunay2`, constrained Delaunay, `refine2`; `bench` against Triangle and CGAL; the command-line program; **release 0.1.0** | Phase 2 | Checker passes on every family; the 2D targets; Triangle's refinement results matched in element count and angle bounds within a stated margin on its own test inputs | CDT predicate mix (measured here) |
| 4 | `delaunay3` and regular triangulations, sequential then parallel; `voronoi` and power diagrams with clipping; `bench` against Geogram, TetGen, CGAL; **release 0.2.0** | Phase 3; the parallel cycle's closing notes; X6 | Checker passes on every family at every thread count with identical output; the 3D targets; thread sanitizer clean | parallel cycle, X6, X7 |
| 5 | `remesh` (isotropic remeshing, simplification, smoothing); `stuffing`; conformal constructions; **release 0.3.0** | Phase 4; Galoot 1.2.0 wanted for the constructions | Manifold, unfolded output on the test surfaces; stuffing's dihedral bounds hold on every output element | stencil recheck; polyhedral structures |
| 6 | `tet`: study unit S1, then 3D boundary recovery, then quality refinement and sliver treatment; **release 0.4.0** | Phase 4; S1's report and an owner ruling on scope (O11) | Input facets recovered on the test set with every element positively oriented; quality comparable to TetGen's on shared inputs, margin set by S1 | S1 |
| 7 | Surface meshing from implicit and parametric input (restricted Delaunay), if wanted | Phases 4 and 5; a learning cycle | to be set by that cycle | restricted Delaunay |
| 8 | Quad and hex (provisional): octree and quadtree methods; high-order 2D quad and swept hex; a field-guided experiment | Phase 3 for 2D, phase 5 for the octree; the quad and hex cycle's closing notes; the revision of section 7.1 | to be set in the revision pass | quad and hex cycle, X8 |
| R1 | 1.0: interface freeze of layers 0 to 6 as then implemented | Phases 0 to 6 and at least two outside uses | two audits, full matrix | none |
| R2 | VoroCrust-style Voronoi meshing with exact power predicates | Phase 4; a study | research; no date | sphere-triple predicates |

Phases 5 and 8's 2D part can run beside phase 4 if the owner wants breadth before depth; the table's order is the recommendation (depth first, because every later module inherits the 3D kernel's quality).

**Experiments, each a unit with a question, a method and a decision it settles.**

| Unit | Question | Method | Settles |
| --- | --- | --- | --- |
| X1 | Are the tie-breaking sequences for the perturbed in-circle, in-sphere and power tests correct and mutually consistent? | Derive them; check against a polynomial-in-ε exact scalar over Galoot and against brute-force Delaunay triangulations of degenerate sets (unique result, right element count, no flat element) | The perturbation rule (section 9.1) |
| X2 | Which filter tree and which middle stages give the in-sphere targets? | Candidates: a filter tree with independent products; exact products of exact differences (Shewchuk's stage B idea); a first-order correction stage; the interval stage. Measure time and fall-through on every family; prove the chosen constants with Gappa and Rocq | The cascade's final shape; the hard-input targets |
| X3 | What does garnet's own `WideInt` fallback cost against GMP? | Time both on the families that reach the last stage | Confirms tier 0 holds no big-number library, or reports the price |
| X4 | Does keeping constraint intersections homogeneous change constrained Delaunay results or cost against rounding at once? | Both variants on crossing-heavy input | The rounding rule for crossing constraints |
| X5 | What does incremental insertion cost in 2D against divide and conquer? | Garnet's counters and times against Triangle's two algorithms | Whether a second 2D construction algorithm is worth carrying |
| X6 | Deterministic reservations or partition and repair for parallel 3D Delaunay? | Prototype both on the sequential kernel; measure speedup, memory and determinism | The parallel design of phase 4 |
| X7 | Which array layout and which renumbering points give the memory and time targets? | Cache-miss and time measurements on the 3D kernel | Section 9.3's open choices |
| X8 | Do rotors help frame interpolation and smoothing against quaternions and matrices? | Accuracy and cost on a frame-field smoothing kernel | Whether `quadhex` uses Galoot |

## 15. The first three units, costs, and risks

### 15.1 Unit G1: the repository scaffold

- **Goal.** A garnet repository in which the next unit can add a header, a test and a tool without deciding anything about structure.
- **Role and model.** Build expert, Sonnet; Sonnet reviewer.
- **Steps.**
  1. Create `/home/brianm/projects/utils/math-core/garnet`, `git init` with the branch `main`; `master` is created by the owner at the first release.
  2. `LICENSE` (MIT, the owner's byline as the owner gives it), a README of one page (what garnet is, its state, how to build), `AGENTS.md` (what an agent reads first; the shared-machine rules by reference to the job pool, the timing lock and `memguard.sh` at their paths).
  3. The layout of section 9.1, with `base` holding the index types, `Error`, `Result` and the checking macro, each with a test.
  4. `CMakeLists.txt` and `CMakePresets.json`: the presets of section 9.1; the options `GARNET_BUILD_TESTS`, `GARNET_BENCH` (off), `GARNET_GALOOT_SOURCE_DIR`; the test framework by the owner's ruling on O3; the warning set; `-ffp-contract=off`; the target baseline by O4.
  5. `.clang-format` and `.clang-tidy`, started from Galoot's, with the provenance row.
  6. `tools/check`: formatting, static checks, build and tests for a named preset, with a non-zero exit when no test ran. `tools/reference/`: a `uv` project with a lock file and one generator (exact `orient2d` cases) to prove the path from Python to a generated header and its regeneration check.
  7. A consumer smoke test: a project outside the tree that adds garnet with `add_subdirectory` and links `garnet::headers`.
  8. `docs/agent/`: this plan copied as plan 0001; empty decisions, reviews, lessons; the provenance template; the agent-cost table.
- **Definition of done.** `tools/check` passes on the debug, release and sanitizer presets of both compilers from empty build trees; the from-empty build time and peak memory are recorded; the smoke project builds; no file in the tree comes from a third party; the review record is committed.
- **Out of scope.** Any geometry.

### 15.2 Unit G2: governance and conventions

- **Goal.** Garnet's own constitution, workflow, verification, performance and style documents, its conventions document, and the license-hygiene procedure, drafted for the owner's ratification.
- **Role and model.** Planner, Opus; an Opus reviewer who has not seen this plan's reasoning checks the drafts against Galoot's originals for anything dropped without a stated reason.
- **Content.** Section 12's adoptions and simplifications written out as provisions; sections 2.3, 9.1 (exactness tiers, determinism, errors) and 11 as provisions; the conventions document with the result types of section 3.3, the sign of each classical predicate stated against Shewchuk's definitions with a worked numeric example of each sign, orientation of elements (counterclockwise triangles, positively oriented tetrahedra with the rule written out), local numbering of faces and corners, and index origin in files.
- **Definition of done.** Each provision is traceable to a line of this plan or to an owner ruling; each convention names the test that will pin it; the owner has ratified or returned each document; a prepared list of the departures from Galoot's governance is in the report.
- **Runs beside G1.** It needs only the directory.

### 15.3 Unit N1: the numeric scalars

- **Goal.** `two_sum`, `two_diff`, `two_product`, `Expansion<N>`, `Interval`, `WideInt<Bits>` and the expansion precondition check, each `constexpr`, each a Galoot `ring_scalar`, each verified.
- **Role and model.** Numerical expert, Opus; a separate Opus test author; Opus blind review.
- **Sources (class A).** Shewchuk's 1997 paper on adaptive-precision arithmetic for the expansion algorithms; the knowledge-base notes on error-free transformations, on interval implementation without rounding modes, and on the exact precondition of an expansion stage. No code is opened.
- **Interface.** As section 3.2. `Interval` offers addition, subtraction, multiplication, negation, construction from a double and from an integer, `lower`, `upper`, `contains_zero`, `sign` (returning a definite sign or "undecided"). `Expansion` offers the ring operations, `sign`, and an estimate. `WideInt` offers the ring operations, shifts, comparison, `sign`, and exact construction from a double scaled by a power of two.
- **Evidence.**
  - Exact reference values from the Python tool by two routes, for sums and products on adversarial operands (cancellation, subnormals, the largest and smallest normal numbers, powers of two and their neighbors).
  - Properties: an expansion's value equals the `WideInt` value of the same computation; an interval always contains the `WideInt` value; every type satisfies the ring laws it claims (`Interval`: inclusion only) on generated operands; all of it also in constant evaluation.
  - A Galoot wedge over each type equals the wedge over `WideInt` (containment for `Interval`): the first use of the boundary of section 8.
  - Behavior stated and tested at the edges: what `Expansion` does on overflow and underflow of a component (the precondition check predicts it), and `Interval` near the ends of the range.
  - The listed mutants (a dropped error term, a wrong branch in the two-sum, an off-by-one in a carry, a widened interval made narrower) all rejected.
- **Definition of done.** The evidence above on both compilers with sanitizers; the cost of each operation recorded in ns on a pinned core; the provenance rows; the blind review record.
- **What follows.** P1 (`orient2d` and `orient3d` through the full cascade, which fixes the pattern), then X1 and X2, then P2 (the circle and sphere family, perturbed and batched).

### 15.4 Cost estimates

Tokens are totals as Galoot's agent-cost table counts them, including reviews and the coordinator's share. Elapsed time assumes Galoot's observed pace, with the owner answering rulings within a day. The spread is plus or minus 50%, wider where the table says so.

| Item | Tokens | Elapsed |
| --- | --- | --- |
| G1 | 0.4 M | 1 day |
| G2 | 0.6 M | 1 to 2 days, plus ratification |
| N1 | 1.2 M | 2 to 3 days |
| Phase 1 in all (N1, P1, P2, X1 to X3, gate) | 5 M | 1.5 weeks |
| Phase 2 | 5 M | 1.5 weeks |
| Phase 3 (to release 0.1.0) | 7 M | 2 weeks |
| Phase 4 (to release 0.2.0) | 10 M | 3 weeks |
| Phase 5 | 6 M | 2 weeks |
| Phase 6 | 10 to 20 M (research-grade; the spread is the estimate) | 4 to 8 weeks |
| Phase 8 | not estimated until the revision pass | |
| Galoot 1.1.0 (items 1 to 4 of section 8.3) | 1.5 M | 3 to 4 days |
| Galoot 1.2.0 (M6) | 3 M | 1 week |

To release 0.2.0, a 2D and 3D Delaunay and Voronoi mesher with exact predicates and parallel construction: about 28 M tokens and eight to ten weeks.

### 15.5 Risks

| Risk | Likelihood | Effect | Response |
| --- | --- | --- | --- |
| The in-sphere cascade misses its targets on hard input | medium | 3D construction slower than Geogram on degenerate families, though correct | X2 before the 3D kernel; targets reset from its result; correctness is never traded |
| Thread-count-independent output costs too much speed | medium | The parallel target is missed | X6 measures it; the owner may relax determinism to "per thread count" for one named mode (O7) |
| 3D boundary recovery proves larger than estimated | high | Phase 6 slips or stalls | S1 first; phases 5 and 8 do not wait for it; isosurface stuffing gives a working volume mesher meanwhile |
| A contributor copies or paraphrases restricted code | low, with a high cost | License contamination | Section 11: implementers never open class C code; provenance rows; the review item |
| Galoot's compile-time cost in garnet's headers | medium | Slow builds, large memory in tests (Galoot's own known friction) | Galoot is included only by `geometry` and the meet predicates; the compiled library hides it from users; build cost is measured from G1 |
| The quad and hex section changes substantially | high | Rework of section 7.1 and phase 8 only | Marked provisional; nothing earlier depends on it |
| An own `WideInt` is too slow or subtly wrong | low | Slow last stage; or wrong signs | Two-route oracle; X3; the last stage was measured at microseconds per call and is rare |
| Agents drift from the style under volume | medium | Unreadable modules | The readability rules are review items; theory note and source reviewed together |

## 16. Owner's decisions

Each item gives the question, the options with their tradeoffs, and a recommendation. Items O1 to O4 are needed before unit G1; the rest before the phase named.

- **O1. Create the repository.** Garnet at `/home/brianm/projects/utils/math-core/garnet`, branch `main` for integrated work, `master` for releases, no remote yet. Alternative: wait for the two running learning cycles. Waiting costs time and gains nothing, since phases 0 to 3 do not depend on them. **Recommend: create it now.**
- **O2. How garnet obtains Galoot.** (a) A path to a checkout now, `FetchContent` at a pinned tag once Galoot has a URL; (b) a git submodule; (c) a copy of the headers. (b) and (c) put Galoot's code in garnet's tree, against "vendor no code". **Recommend (a).**
- **O3. The test framework.** (a) doctest fetched at configure time, pinned by commit hash, used by tests only; (b) doctest from a system package, which the owner would install; (c) a small harness of garnet's own. (a) needs the network once per fresh build tree; (b) needs an installation; (c) costs perhaps 0.3 M tokens and loses a tool the agents already know from Galoot. None stores code in the tree. **Recommend (a), with (b) used automatically when the package is present.**
- **O4. The default instruction-set target.** (a) x86-64-v3 (with fused multiply-add) by default, baseline as an option that stays correct but slower; (b) baseline by default as in Galoot. The predicates' exact stages are built on the fused operation. **Recommend (a).**
- **O5. Symbolic perturbation.** Yes for the circle and sphere family, ordered by coordinates; no for orientation (section 9.1). The alternative, handling every cocircular case in the algorithms as CGAL does in 2D, keeps "zero means degenerate" visible to callers but gives up uniqueness, and with it the simplest route to determinism. Plain unperturbed predicates remain available under their own names. **Recommend yes, confirmed by X1.**
- **O6. Exact arithmetic without GMP.** Garnet's own `Expansion` and `WideInt` in the default build; GMP only in optional comparisons. The cost is about 1 M tokens of careful work and a slower last stage; the gain is an empty tier 0. **Recommend own.**
- **O7. Parallel model and determinism.** A pool on standard threads, and output independent of thread count as a requirement. Alternative: OpenMP (less code, a runtime dependency, weaker determinism). If X6 shows determinism costing more than a third of the speedup, the owner chooses between the requirement and a named non-deterministic fast mode. **Recommend the pool and the requirement.**
- **O8. Header-only or compiled.** Header-only for layers 0 to 5, a compiled static library for algorithms, I/O and threads (section 9.1). All header-only, as Galoot, would cost users long compiles; all compiled would give up `constexpr` predicates. **Recommend the split.**
- **O9. The performance targets of section 9.4.** Ratify as written, ratify the easy-input and whole-algorithm rows now and the hard-input rows after X2, or record without gating. **Recommend the second.**
- **O10. Galoot's next releases.** 1.1.0 with the enclosure-scalar statement, the evaluation-order statement, rga3 norms and `unitize`, and the rga2 preset; 1.2.0 with the conformal presets (M6). Alternative: one release with all five, which delays the small items behind the large one. **Recommend two releases, 1.1.0 planned as soon as 1.0.1 is tagged.**
- **O11. 3D boundary recovery and refinement.** (a) Garnet writes them (phase 6), after a study; (b) garnet stops at Delaunay, Voronoi and stuffing, and users who need boundary recovery link TetGen or CGAL themselves. (b) is cheap and leaves garnet short of "general-purpose" in 3D. **Recommend (a), with the scope set after study S1; decide then, not now.**
- **O12. Quad and hex within the budget.** Whether field-guided methods may add a dependency (a mixed-integer or sparse direct solver) as an optional feature, or must fit garnet's own small solver. **Recommend: no new dependency; decide finally in the revision pass.**
- **O13. I/O scope.** The formats of section 7.3, with HDF5-based and compressed formats outside the default build. **Recommend as listed.**
- **O14. Governance and model levels.** Ratify the simplifications of section 12 (through unit G2's drafts), and say whether Fable may be used for the phase 1 gate and the parallel design of phase 4. **Recommend ratifying, and permitting Fable for those two.**
- **O15. Restricted-license tools.** Triangle, TetGen and VoroCrust only as separate processes; Geogram and CGAL linked only into undistributed benchmark executables; implementers never open class C source (section 11). The stricter alternative, never linking CGAL at all, loses the best differential oracle. **Recommend as written.**
- **O16. Coordinates and platforms.** `double` coordinates only; Linux with GCC and Clang only. **Recommend both for 0.x.**
- **O17. Breadth or depth after 0.1.0.** 3D next (phase 4), or surface remeshing and 2D quad meshing first. **Recommend 3D next.**

## 17. Next step

On the owner's rulings for O1 to O4: units G1 and G2 start together, then N1. The coordinator sends the closing notes of the two running cycles for the revision of sections 7.1, 9.2, 9.3 and phases 4 and 8.
