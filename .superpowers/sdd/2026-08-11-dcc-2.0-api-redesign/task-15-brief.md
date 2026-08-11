# Task 15 brief — publish and gate the DCC 2.0.0 Stable contract

Implement only Task 15 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The release section
of the design, the completed Task 6–14 contracts, and this brief are
authoritative. This brief supersedes older snapshots that say 231 global REST
endpoints or seven focused App aggregate headers. The final contract is exactly
224 canonical REST endpoints and eight App headers (`base`, `options`,
`lifecycle`, `listeners`, `context`, `modules`, `store`, and `env`).

Work test-first. Do not push, tag, publish, or edit the ignored progress ledger.
Use clean build, install, staging, and archive directories. Task 14's public ABI
removal and this task's major-version/symbol-baseline reset form one atomic
cutover commit; never record either half independently.

## Outcome

Publish one internally consistent **DCC 2.0.0 Stable** contract across source,
runtime, documentation, shared-library metadata, package metadata, generated
artifacts, CI, and release automation. The release is ready only when:

- the project/package/runtime version is exactly `2.0.0`;
- shared libraries advertise ABI major/SOVERSION `2`;
- LLAM `2.2.0` is the exact minimum supported runtime dependency and LLAM ABI
  major 2 is enforced;
- the final public headers, checked DCC 2 symbol baseline, and actual exports
  agree;
- the strict REST manifest contains exactly 224 canonical endpoints plus 60
  generic operations, for 284 exported public `dcc_rest_*` operations;
- every public declaration and Bot macro has useful ownership/behavior
  documentation, and generated reference pages are reproducible;
- no primary page markets the release as Beta or teaches a removed DCC 1 path;
- supported platform, sanitizer, package, install, and publish gates all pass.

## Frozen release contract

Set and audit every version surface from one project version rather than
maintaining unrelated literals:

- `project(dcc VERSION 2.0.0)` and generated `DCC_VERSION_*` values;
- runtime `dcc_version()`/doctor output and version-oriented CTests;
- target `VERSION 2.0.0` and `SOVERSION 2` for every shared DCC library;
- `dcc.pc`, CPack, binary/source archive names, SBOM, release notes, and package
  consumer metadata;
- `DCCConfigVersion.cmake` generated with `SameMajorVersion` compatibility;
- workflow input/default/tag validation and issue-template examples;
- README, changelog's new release heading, release guide, troubleshooting, CLI,
  and other current-release copy.

The stable release accepts the exact tag `v2.0.0` and sets GitHub prerelease to
false. Release scripts must reject `2.0.0-beta`, `v2.0.0-beta`, build metadata,
a leading/trailing-space variant, a different tag, or a version that merely
shares the numeric base. Do not use a broad case-insensitive `Beta` ban:
historical 1.5.0 changelog text and functional domain examples such as a “Beta
role” remain valid. Scope the Stable-copy audit to current release metadata and
primary marketing/guidance pages, while separately banning removed API snippets
outside the migration guide and historical material.

LLAM stays exactly at the already selected floor of 2.2.0; do not lower it and
do not accidentally raise it. Audit CMake discovery, installed package config,
install scripts, doctor output, README/docs, workflow checkout/pin, release
notes, and SBOM together. The release workflow uses the immutable commit tested
for LLAM 2.2.0 and verifies the LLAM runtime ABI major is 2.

Before release, the three focused environment functions must have only their
canonical names:

- `dcc_app_env_load_file`
- `dcc_app_env_load_dotenv`
- `dcc_app_env_validate_requirements`

The old spellings are ABI-removal seeds, not aliases. `<dcc/app.h>` includes
`env.h` as its eighth focused header. The strict surface and package audits
must fail if the aggregate falls back to seven headers or reintroduces
`app/legacy.h`.

## DCC 2 symbol baseline

Create `tools/api_v2_symbols.txt` from the final clean public header tree after
all REST migrations and immediately before the atomic cutover commit. Do not
copy a pre-migration count. The preflight figures of 1,789 `DCC_API` names in
the transition tree and 1,344 after deleting only Sugar/legacy are diagnostic
snapshots, not the expected DCC 2 total.

The baseline contract is:

1. Parse complete external `DCC_API` function declarations from every installed
   public header with a comment/string-aware scanner. Do not use every global
   `dcc_*` token returned by `nm` as the source of truth.
2. Resolve duplicate declarations before generation. In particular,
   `dcc_rest_get_current_application` was declared from two public owners in
   the preflight tree; the final API has one owning header and one declaration.
3. Write one symbol per line, bytewise sorted and unique, with deterministic LF
   output. A `--check` mode regenerates in memory and fails on missing, extra,
   duplicate, unsorted, or stale entries.
4. Inspect the newly built shared library and require the platform's externally
   exported DCC public set to equal the baseline. Fix visibility/version-script
   leaks instead of adding private helpers to the public baseline. Require a
   declaration, definition, and export for every baseline entry, and reject all
   immutable DCC 1 removal seeds.
5. On ELF use an available `llvm-nm`/`nm` read of defined dynamic externals; on
   Mach-O normalize the leading underscore; on Windows use `llvm-nm` or
   `dumpbin /EXPORTS`. A missing usable symbol inspector is a hard failure, not
   “symbols not available.”
6. The release audit must obtain a merge base with `origin/main` and read that
   revision's project major. A major increase from 1 to 2 permits the one-time
   reset represented by this file. At major 2, removing any baseline symbol is
   forbidden. Additions require an intentional baseline update in the same
   change; the current headers and baseline must still be exactly equal.
7. A missing remote, merge base, baseline, shared library, generated include
   tree, manifest input, Python interpreter, or symbol tool fails the release
   job. Source-package verification applies the same checked baseline rather
   than silently skipping origin or binary checks.

Update `tools/audit_public_api.py` and `tools/audit_release_contract.py` around
this model. The REST audit is an independent semantic gate and must assert the
strict global total of 224 canonical endpoints split 41/35/47/57/44 across
Tasks 6–10, exactly 60 generic operations, zero transition helpers or
composites, one declaration/definition per endpoint, and no App REST mirror.
It must also keep the seven unavailable candidate operations—Create Guild,
Delete Guild, Modify Guild Integration, Sync Guild Integration, Bulk Edit Guild
Command Permissions, Create Guild From Template, and the obsolete bot-facing
Get Current User DMs—out of the active surface. Do not derive or overwrite the
hand-reviewed endpoint manifest from whatever names happen to be in the
headers.

## Declaration comments and generated reference

Documentation generation does not compensate for undocumented declarations.
First place an immediate public documentation comment on every `DCC_API`
declaration and every exported Bot macro. The preflight scan found at least
1,202 undocumented declaration occurrences and only 147 documented ones;
duplicate declarations and later tasks can move those raw counts, so the
release invariant is zero undocumented final declarations/macros rather than a
fixed denominator.

Each comment must state, as applicable:

- the operation and success/failure result;
- ownership of every pointer parameter and return value;
- borrow/copy/clone/create/take semantics and the exact validity boundary;
- nullability, pointer/count pairing, size/version and presence-bit rules;
- callback `user_data` lifetime and concurrency/reentrancy expectations;
- request-handle acceptance, cancellation, wait, result clone, and destroy
  behavior;
- cleanup required for an owning result or value;
- important thread-safety, ordering, or local-rejection guarantees.

Create a deterministic standard-library generator, preferably
`tools/generate_api_reference.py`, with `--write` and `--check` modes. It reads
the immediate documentation comment plus complete declaration/signature,
mirrors each installed header's relative path below `docs/reference/api/`, and
generates a categorized Bot/App/Core/REST index. Pages include a generated-file
marker, source-header link, signature, summary, ownership/lifetime/nullability,
error/behavior notes, and links to related value lifecycle functions. Bot
macros receive equivalent behavior and evaluation/ownership notes.

Generation must be byte-for-byte deterministic: stable path/symbol ordering,
LF endings, no timestamps, no machine paths, and no locale-dependent sorting.
Reject a missing page, stale page, duplicate symbol owner, undocumented public
item, handwritten edit under the generated tree, or orphan page. `mkdocs.yml`
links the generated category/header indexes without requiring a hand-maintained
entry for every symbol.

Turn the monolithic catalog in `docs/api.md` into task-oriented guidance and
links to generated reference pages. Rewrite the primary documentation set that
the preflight identified—README plus the affected public pages under `docs/`
and `mkdocs.yml`—so it presents one Bot quickstart, focused App/Core escape
hatches, request ownership, ordered interactions, tagged Components v2, and
the canonical `DCC_UI_*` vocabulary. Replace or retire the public Sugar guide;
only `docs/migration-1-to-2.md` teaches old-to-new spellings. Do not edit the
historical plan/spec merely to hide their design history.

Mark documentation code fences that are promised as complete examples with an
explicit compile-test attribute/comment. Extract and compile every marked C
block as C11 and every marked C++ block as C++17 with warnings as errors,
against the installed package. Pseudocode must be visibly marked and is not a
way to exempt a purported quickstart. No documented happy path may discard a
returned status.

## Release and package gates

Close the current platform gaps rather than relying on the host's default
static build:

- **Linux:** build shared and static configurations with tests, examples,
  tools, and benchmarks. Run the complete enabled CTest suite plus the full
  ASan/UBSan configuration. Inspect the actual shared object and require an
  ABI-2 SONAME such as `libdcc.so.2`; compile and run clean installed-package C
  and C++ consumers.
- **macOS:** run the complete enabled suite including cluster coverage, build
  the dylib, and inspect its install name/current/compatibility metadata for
  ABI major 2. Compile and run clean installed-package C11/C++17 consumers.
- **Windows:** use the supported ClangCL configuration with `/W4 /WX`, compile
  every installed header alone as C and C++, run the enabled suite and package
  consumers, and validate exports with `llvm-nm` or `dumpbin /EXPORTS` rather
  than skipping the symbol audit.
- **FreeBSD/OpenBSD/NetBSD:** install Python and a C++ compiler, enable and run
  the supported tests, and execute the strict surface, public API, release,
  canonical endpoint, source-package, and installed-consumer gates. Remove the
  current special build of `dcc_sugar_smoke`; do not mark the release green
  with all tests skipped.

For every platform/package format:

- begin with an absent build directory, install prefix, staging root, and
  archive extraction directory;
- positively require `<dcc/bot.h>`, the eight focused App headers, focused Core
  and REST headers, generated version metadata, CMake config/version files,
  `dcc.pc` where applicable, the 224-endpoint manifest, the symbol baseline,
  migration guide, and generated reference;
- negatively require absence of `dcc/sugar.h`, the `dcc/sugar/` directory,
  `dcc/app/legacy.h`, declaration fragments, obsolete CMake targets, and DCC 1
  exported symbols in binary and source archives;
- configure, build, and run consumers through the installed CMake package and
  pkg-config path without falling back to source-tree headers;
- run the canonical benchmark set and archive results so the performance work
  remains observable; benchmark execution failure is a release failure, while
  a threshold change requires a separately reviewed policy rather than an
  ad-hoc release exception.

Update `tools/release_check.sh`, `tools/package_release.sh`,
`tools/package_release_windows.ps1`, `tools/audit_source_package.py`, and the
Linux/macOS/Windows/BSD workflows together. The release workflow must depend on
successful docs/reference freshness, strict API/REST audits, sanitizer jobs,
and the complete package matrix. It may not publish artifacts produced from a
different commit. Upload uses the exact `v2.0.0` tag, marks the release Stable
and non-prerelease, and occurs only after every dependency is green.

## Required RED tests and implementation sequence

1. Extend release assertions to require 2.0.0, SOVERSION 2, exact LLAM 2.2.0,
   exact Stable tag/prerelease state, eight App headers, canonical env names,
   224 REST endpoints, a fresh symbol baseline, generated reference, and clean
   package negative members. Capture distinct RED failures against 1.5.0 and
   the missing baseline/reference.
2. Add symbol-generator/audit fixtures for comments and strings, multiline
   declarations, duplicates, unsorted/stale baselines, same-major removal,
   major reset, missing origin, missing symbol tools, Mach-O underscores, and
   representative Windows export output. Capture RED before changing the
   compatibility logic.
3. Add documentation-generator fixtures for nested header paths, multiple
   declarations, Bot macros, ownership fields, deterministic output, stale and
   orphan pages, undocumented declarations, CRLF input, and duplicate owners.
4. Add the missing declaration/Bot comments, then generate and check the API
   reference. Rewrite primary guidance and compile every marked snippet against
   a clean installed package.
5. Prepare version/SOVERSION/LLAM/package changes, derive the baseline from a
   new shared build, and combine them with Task 14's held public deletion in the
   atomic ABI cutover described below.
6. Run the complete normal, sanitizer, platform, package, source-archive,
   benchmark, docs, symbol, and publish-preflight matrices. Resolve failures;
   do not waive them by narrowing test discovery or adding transition
   allowlists.

## Atomic commit sequence

Use this sequence so each preparation stage is reviewable while the ABI break
itself is indivisible:

1. `chore: add DCC 2 migration tooling` — Task 14 tool and migration fixtures.
2. `refactor: migrate repository consumers to DCC 2` — examples, generators,
   behavioral tests, and package consumers; compatibility wrappers still exist.
3. `refactor: isolate DCC 1 app adapters` — final private canonical engines,
   moved context/env/wait/intents ownership; installed compatibility remains.
4. `docs: document canonical DCC 2 declarations` — public source comments,
   generator/tests, and reference scaffolding without claiming the release.
5. `feat!: remove DCC 1 public API` — one coordinated Task 14/15 cutover that
   deletes the headers and 361 exports, promotes `dcc_app_run_with_signals` to
   the focused lifecycle header, fixes the eight-header aggregate and env
   names, sets 2.0.0/SOVERSION 2, records the final symbol baseline, enables
   strict same-major/removal checks, and adds clean archive/install negatives.
6. `release: prepare DCC 2.0.0 API` — generated reference, primary Stable docs,
   changelog/release metadata, cross-platform workflow gates, and final verified
   release automation.

No commit between steps 4 and 5 may expose the removed ABI at version 1.5.x or
advertise 2.0.0 with the old surface. Generate the committed baseline and
reference from the exact step-5 tree and a clean step-5 shared build. If that
cannot be done in one index, leave the deletion uncommitted and coordinate the
cutover; do not manufacture an allowlist for the intermediate state.

## Rollback and publication risks

- Treat the step-5 cutover as the smallest rollback unit. Revert it in full;
  never restore only headers without their definitions, or restore ABI 1
  metadata around ABI 2 symbols.
- A baseline made from a stale static library, stale install prefix, or the
  earlier 1,344-name diagnostic snapshot can permanently bless the wrong ABI.
  Delete all outputs and compare final headers, definitions, and shared exports
  before committing it.
- A partially migrated generator can keep emitting removed APIs even when the
  repository itself is green. Generate and compile every preset from scratch in
  the release job.
- Broad archive-copy rules can reintroduce deleted Sugar files from a dirty
  stage. Build source and binary packages from clean staging roots and audit
  extracted members negatively.
- Missing `origin/main`, `nm`, Python, C++, or platform tests currently permits
  silent success in parts of the pipeline. Every missing prerequisite is fatal
  for release jobs.
- Do not move or recreate the release tag after publication. Create `v2.0.0`
  only from the fully verified final commit, upload checksum/SBOM artifacts from
  that same commit, and leave prerelease false.

## Verification and handoff

Run at minimum:

- all documentation-comment, reference-generation, symbol-baseline, release,
  strict surface, project-layout, source-package, official-surface, and
  224-endpoint audits in write-check round trips;
- every installed header alone as C11 and C++17 with warnings as errors;
- a full tests/examples/tools/benchmarks build and complete enabled CTest suite
  from a new normal build directory;
- a new full ASan/UBSan build and complete enabled CTest suite, plus the focused
  TSan jobs owned by concurrent App/REST code;
- actual Linux SONAME, macOS dylib metadata, and Windows export inspection;
- clean installed C/C++ consumers and clean binary/source package extraction on
  every supported platform;
- generated-project compilation for every preset, documentation snippet
  compilation, benchmark execution, workflow dependency validation, and an
  exact-tag dry-run that cannot publish.

Write `task-15-implementer-report.md` beside this brief with RED evidence, the
final symbol count and baseline hash, duplicate-resolution notes, documentation
coverage totals, generated page inventory, exact version/LLAM/SOVERSION values,
shared-library metadata, package member lists, platform/sanitizer/test results,
benchmark artifacts, commit hashes, and the release dry-run result. Commit the
report separately and stop for independent review; pushing, tagging, and
publishing happen only after that review explicitly approves the final tree.
