# Task 15 brief — publish and gate the DCC 2.0.0 Stable contract

Implement only Task 15 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The release section
of the design, the completed Task 6–14 contracts, and this brief are
authoritative. This brief supersedes older snapshots that say 231 global REST
endpoints or seven focused App aggregate headers. The final contract is exactly
224 canonical REST endpoints and eight App headers (`base`, `options`,
`lifecycle`, `listeners`, `context`, `modules`, `store`, and `env`).

Work test-first. Do not push, tag, publish, or edit the ignored progress ledger.
Use clean build, install, staging, and archive directories. Every held Task 14
public cut and this task's major-version/baseline reset form one atomic cutover
commit; never record either half independently. The held set includes the
App/Sugar removals, generic REST 61-to-35 reduction, OAuth2/firewall/response
replacements, component-builder and message/modal raw-surface removal, and all
Task 14 opaque-state/bounded-writer plus other public signature/layout
replacements. Tasks 6–13 already on the
unreleased 2.0 branch are outside this cutover boundary.

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
- the strict REST manifest contains exactly 224 canonical endpoints plus 35
  externally exported generic symbols, for 259 exported public `dcc_rest_*`
  symbols, while its three public initializers remain checked static inlines;
- every public declaration, static-inline callable, and public macro has useful
  ownership/behavior documentation, and generated reference pages are
  reproducible;
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
- installed `dcc-config-version.cmake` generated with `SameMajorVersion`
  compatibility;
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
`DCC_LLAM_REQUIRED_VERSION` is a non-cache project constant; if backward
compatibility leaves an override input visible, configure accepts only the
literal `2.2.0` and hard-fails both lower and higher values before discovery or
generation. No source-package consumer can rewrite the installed floor/expected
ABI major through `-D`.

Before release, the three focused environment functions must have only their
canonical names:

- `dcc_app_env_load_file`
- `dcc_app_env_load_dotenv`
- `dcc_app_env_validate_requirements`

The old spellings are ABI-removal seeds, not aliases. `<dcc/app.h>` includes
`env.h` as its eighth focused header. The strict surface and package audits
must fail if the aggregate falls back to seven headers or reintroduces
`app/legacy.h`.

## DCC 2 API and ABI baselines

Create `tools/api_v2_symbols.txt` from the final clean public header tree after
all REST migrations and immediately before the remaining-compatibility cutover
commit. Do not copy a pre-migration count. The preflight figures of 1,789 `DCC_API` names in
the transition tree and 1,344 after deleting only Sugar/legacy are diagnostic
snapshots, not the expected DCC 2 total.

The baseline contract is:

1. Parse every compiler `FunctionDecl` physically spelled in an installed DCC
   header, excluding declarations whose source location belongs to a system or
   dependency header. Combine the C11/C++17 AST with comment/string/macro-aware
   source tokens and classify each declaration exactly once as
   `public_external` or `public_inline`. External linkage requires a source-
   spelled `DCC_API` on the owning declaration and entry in the external
   baseline. The only header-defined function form is a complete `static inline`
   definition; public names `dcc_*` and uppercase `DCC_*` enter the inline
   baseline. Reject plain extern/default-linkage declarations, `extern inline`,
   non-inline static prototypes/definitions, external header definitions,
   `DCC_API` combined with static/inline, and any ambiguous/unclassified storage
   class before symbol comparison.
2. Resolve duplicate declarations before generation and require one owning
   installed-header declaration/definition per public key. In particular,
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
6. `tools/release_compat_base.json` pins the current version, previous Stable
   tag, its full 40-hex commit, and comparison mode. For 2.0.0 it must name
   `v1.5.0`, full commit
   `6ba1e29a45ec3617cf17e05b716c54b8094bbf96`, and `major_reset`; later 2.x
   releases name the latest prior Stable v2 tag and `same_major`. The audit
   resolves the tag from the reference repo,
   requires an exact hash match, requires that commit to be a strict ancestor
   of release HEAD, and rejects `compat_base == HEAD`. A 1→2 major reset is
   permitted once; in same-major mode removing any baseline symbol is forbidden.
   Additions require an intentional current update and current headers/baseline
   equality. Only after the complete FunctionDecl inventory passes may
   declaration↔definition↔export equality run; `nm` tokens alone never define
   the source API.
7. `origin/main` is only an ancestry/source-check input. Its recomputed merge
   base is recorded for diagnostics and provenance but is never the compatibility
   baseline, because a post-merge/tag workflow may have `origin/main == HEAD`.
8. A missing remote, pinned Stable tag/compat base, baseline, shared library,
   generated include tree, manifest input, Python interpreter, or symbol tool
   fails the release job. Source-package verification applies the same checked
   baseline rather than silently skipping origin or binary checks.

The pinned file for this release is exactly:

```json
{
  "schema": 1,
  "current_version": "2.0.0",
  "previous_stable_tag": "v1.5.0",
  "compat_base": "6ba1e29a45ec3617cf17e05b716c54b8094bbf96",
  "comparison_mode": "major_reset"
}
```

It is a reviewed input, never regenerated from the currently checked-out branch
or from the newest reachable commit during `--check`.

`tools/api_v2_major_reset_1_to_2.json` is the other immutable reset input. It
contains the pinned prior/current identity plus these exact arrays:

```text
removed_external, changed_external
removed_inline,   changed_inline
removed_types,    changed_types
removed_macros,   changed_macros
```

Every external entry records name, prior owner header, complete normalized prior
declaration and fingerprint, owning Task 6–14 number, and a reviewed reason.
A changed external also records the exact current owner/declaration/fingerprint.
Inline/type/macro entries record owner, kind, stable key, prior fingerprint,
task, and reason; changed entries also record the expected current fingerprint.
Macro entries identify conditional-source versus each resolved profile. There
are no wildcard, prefix-only, directory-only, or category-count entries.
The existing `tools/api_v2_removed_names.txt` remains a lexical negative-surface
seed and never substitutes for this complete ABI/source reset ledger.

The reset checker reconstructs prior FunctionDecl inventory, symbol/declaration,
inline, type, record, macro-source, and 12-profile macro artifacts from the exact compat-base using
the same scanners. It also builds and inspects the compat-base shared library,
requiring its public exports to equal its public `DCC_API` declaration set.
For every category, the computed prior-minus-current keys and prior-to-current
fingerprint changes must equal the reviewed ledger exactly. Every ledger prior
fingerprint must match the reconstructed compat-base; every changed-current
fingerprint must match the fresh current baseline. Any prior public item not in
the ledger must pass the ordinary same-major structural comparator even during
`major_reset`. Thus a ledger-unlisted removal or incompatible change is fatal,
while current additions are accepted only when present in the fresh final
baseline. Release `--check` never regenerates or broadens this ledger.

`tools/audit_source_package.py` has one exact release mode and is launched from
the checkout after extracting and building the source archive:

```text
--archive-root <clean extracted tree>
--reference-repo <release checkout containing origin/main>
--compat-base <full 40-hex commit pinned to the previous Stable tag>
--compat-shared-library <shared library built only from compat-base>
--built-shared-library <shared library built only from archive-root>
```

All five arguments are mandatory and canonicalized. The script proves the
reference repository HEAD is the release source commit, recomputes and matches
the supplied compatibility base against `tools/release_compat_base.json` and
the resolved previous Stable tag, proves it is a strict ancestor and not HEAD,
reads its project major, and rejects a dirty reference path that changes tracked
inputs. It separately recomputes the `origin/main` merge base only for ancestry
diagnostics/provenance; that moving branch result never selects comparison
artifacts.
The current shared-library path must be under a clean build directory whose
configured source is `archive-root`; the compat library must be under a separate
clean build whose configured source hash equals compat-base. Both build receipts
record source-tree hash, compiler, options, and binary hash, and use matching
platform/ABI/toolchain profiles. The archive's current headers/baselines always
regenerate byte-for-byte and both newly built libraries are inspected.

Historical compatibility then has exactly two modes. If the compat-base project
major is 2, read its actual DCC 2 baselines and headers, run the structural
same-major comparison, and generate/compile/link/run real historical-header/new-
library probes. If the prior major is 1 and current major is 2, this is the
intentional one-time reset: require the complete immutable DCC 1 removal/reset
ledger above, current artifact freshness, and Task 14's synthetic smaller-DCC2-
prefix canaries, but never pretend that a DCC 1 header can compile or link compatibly
against DCC 2. Any other major transition is fatal. A path inside the checkout
masquerading as an archive binary, a shallow/missing reference,
abbreviated/different compat base, a compat base equal to HEAD, or a prebuilt
current/compat binary without the matching receipt is fatal.

The source archive also contains a deterministic compatibility-provenance
bundle with previous Stable tag, compat-base hash, auxiliary `origin/main`
merge-base hash, prior/current majors, comparison mode, current artifact hashes,
and a SHA-256 member manifest. In `same_major` mode it contains
prior baseline/package-contract hashes plus the exact compat-base historical
CMake/pkg-config consumer source hashes and generated real historical probe sources. In
`major_reset` mode it instead contains the exact reset-ledger hash, reconstructed
prior artifact hashes, inspected compat-library hash/export set, and the
synthetic smaller-DCC2-prefix probe sources/hashes. Checkout-mode audit
regenerates and verifies that bundle before packaging; extracted-tree audit
verifies its member hashes and matches it to the required reference repository.
The bundle supports reproducibility but never replaces the external reference
arguments in the release gate.

Symbol names alone do not protect ABI. Generate and check these additional
reviewable artifacts from the same final header tree:

- `tools/release_compat_base.json` and
  `tools/api_v2_major_reset_1_to_2.json`: the exact reviewed previous-Stable pin
  and complete per-category intentional removal/change ledger above;
- `tools/api_v2_function_inventory.json`: every installed-header FunctionDecl's
  owner, normalized signature, storage/linkage/definition flags, source-spelled
  `DCC_API` presence, `public_external`/`public_inline` class, and fingerprint;
- `tools/api_v2_header_reachability.json`: every installed header path in every
  exact platform/language/build-role profile, plus the bytewise-sorted
  transitive external, inline, type, and public-macro keys reachable by
  including that header;
- `tools/api_v2_package_contract.json`: per supported package profile, the
  CMake package/config/version names, exported target names, public helper
  command signatures/semantics, normalized required target include/link/
  compile-feature/compile-definition interface, and pkg-config module/file,
  `Name`, `Cflags`, `Libs`, `Requires.private`, and `Libs.private` semantics;
- `tools/api_v2_declarations.json`: owner header, normalized complete
  declaration, calling convention, and SHA-256 fingerprint for every
  `DCC_API` function;
- `tools/api_v2_inline_api.json`: owner, normalized signature, and SHA-256
  fingerprint for every public static-inline callable whose function name is
  either `dcc_*` or uppercase `DCC_*`, including the three REST initializers,
  listener/config/context/UI helpers, and same-name initializer functions;
- `tools/api_v2_types.json`: owner, kind, normalized complete declaration, and
  SHA-256 fingerprint for every public typedef, including scalar/alias,
  function-pointer/callback, enum, struct, and union typedefs, plus every enum
  enumerator's exact numeric value;
- Task 14's `tools/api_v2_records.json`: independent layout, direction,
  lifetime, writer, append, stride, initializer/empty, cleanup, and immutable-
  prefix fields for every public aggregate;
- `tools/api_v2_macro_sources.json`: compiler-independent records for every
  public `DCC_*` macro in every installed header, containing owner header,
  name, object/function kind, arity/variadic form, and the ordered normalized
  preprocessor-condition plus replacement-list branches from the defining
  source; comments and strings are parsed rather than matched by `#define`
  grep;
- the exact 12 resolved expansion files matching
  `tools/api_v2_macros_{posix_clang,windows_clangcl}_{c,cpp}_{static,shared_build,shared_consumer}.txt`:
  bytewise-sorted owner/name/kind/arity/normalized-expansion records for every
  public `DCC_*` macro, with compiler/builtin macros excluded;
- `tools/api_v2_bot_macros_posix_clang_c_static.txt` and
  `tools/api_v2_bot_macros_posix_clang_cpp_static.txt`: exact copies of Task
  13's canonical focused full-Bot expansion artifacts used for the budget.

Macro ownership is derived from the defining installed header rather than from
only what an umbrella transitively exposes. The profile matrix is exact:
POSIX/Clang and Windows/ClangCL, each in C11 and C++17, each as `static`
(neither `DCC_SHARED` nor `DCC_BUILD`), `shared_build` (both defined), and
`shared_consumer` (only `DCC_SHARED` defined). The audit records compiler path
and version plus all predefined and explicit role inputs, proves its sentinel
survived preprocessing, and specifically asserts the expected `DCC_API` and
`DCC_DEPRECATED` branch in every profile. A missing compiler/profile is a hard
release failure, not a skipped row.

The source artifact is the portable conditional-definition contract; a resolved
artifact is compared only with the same platform/compiler/language/build-role
profile. C/C++ artifacts explicitly record language-conditional absence. The
focused Bot files remain Task 13's canonical POSIX/static budget gate; they do
not substitute for the complete installed-header source artifact or 12-profile
expansion baseline.

Normalization is comment/string aware, preserves pointer qualification,
parameter order, variadic/calling-convention attributes, and canonicalizes only
irrelevant whitespace/parameter spelling. It never reduces a declaration to a
name. Current headers must regenerate every artifact byte-for-byte.

When both compat-base and current project majors are 2, the release audit reads
compat-base copies, not just simultaneously edited current baselines. Full
normalized declaration hashes always prove that every current artifact is
fresh; same-major cross-revision compatibility uses this exact
kind-specific comparator instead of requiring every complete type hash to stay
identical:

- each prior external or inline callable keeps its complete signature and
  calling convention;
- scalar/alias and function-pointer/callback typedefs keep their exact
  normalized target, parameters, qualification, and calling convention;
- an opaque typedef keeps the same public name/tag identity and remains opaque;
- every prior enum name/value pair stays exact; new enumerators are allowed only
  when all profile probes preserve the enum's ABI size/alignment and every old
  value;
- a record with `layout=fixed` keeps its complete declaration, size, alignment,
  fields, offsets, and array stride regardless of lifetime; and
- a record with `layout=size_extensible` or `layout=versioned` is compared through
  `api_v2_records.json`, not complete-declaration hash equality: every prior
  field keeps name/type/order/offset/meaning, total alignment is unchanged, and
  growth follows its exact append policy. `lifetime=owning` never changes this
  layout decision.

For every prior record, including opaque and fixed records, the comparator also
freezes the non-layout contract: `direction`, `lifetime`, `writer_policy`,
`append_policy`, array-stride policy, initializer identity, initializer-empty
predicate, cleanup/consume identity, ownership anchor, minimum success size,
required logical view/count group, ownership/producer boundary, and every
existing field's semantic meaning and units/range remain exact. A same-layout
borrowed-to-owning change, `full_fixed`-to-`covered_atomic` change, cleanup
rename, consume-semantics change, or reinterpretation of an old field is fatal.
A different lifecycle is introduced only through a separately named new
type/API, never by regenerating the current manifest.

Installed-header reachability is a separate same-major source-compatibility
gate. The reachability artifact uses the same exact 12 profiles as the resolved
macro matrix and records, for each physical installed header, all transitively
visible external/inline/type/macro keys after including only that header. In
`same_major` mode every prior installed header path must still exist and its
prior reachable set must be a subset of the current set in the matching
profile. This catches a lost umbrella or focused-header re-export even when the
physical owner declaration is unchanged. A removed or renamed installed header
is fatal. The initial `major_reset` establishes the DCC-2 reachability baseline;
it does not compare DCC-1 reachability.

Installed-package consumption is independently compatibility-significant
because `dcc-config-version.cmake` advertises `SameMajorVersion`. The DCC-2
package contract fixes `find_package(dcc CONFIG)`, config/version files,
`dcc::dcc`, and the public
`dcc_add_hot_reload_module(target [SOURCES] sources... [OUTPUT_NAME name])`
command including its module/C11/output/link behavior. For each matching
static/shared and platform profile, it records prefix-independent target
interfaces: installed include root, C11 feature, shared-consumer definition,
LLAM/Threads/OpenSSL/platform link roles, and any conditional exported worker
target. It also fixes pkg-config module/file `dcc`, `Name: DCC`, the include and
`-ldcc` roles, and normalized private LLAM/platform dependencies. Package,
subdirectory, and provided-LLAM profiles require the exact semantic floor
`Requires.private: llam >= 2.2.0`; `pkg-config --static dcc` rejects a 2.1.9
fixture and accepts, links, and runs with 2.2.0. Prebuilt/direct-library
profiles instead record and verify the receipt's detected LLAM version, ABI
major/minor, library hash, and normalized `Libs.private` role before a static
consumer may link/run. Paths and
versions are normalized, but names, helper grammar/behavior, and required
interface semantics are exact for ABI major 2; new optional targets/helpers may
be added without changing an existing key.

In `same_major` mode the audit loads this artifact from compat-base and current,
compares matching profile semantics, then checks out the manifest-named actual
historical CMake and pkg-config consumer sources from compat-base. Without
editing them, it configures/builds/runs both against only the new clean install
prefix (and its packaged dependencies). Current editable consumers run too but
cannot substitute for this gate. A removed/renamed package, target, helper,
module, required interface change, missing historical fixture, or historical
consumer failure is fatal. The initial `major_reset` creates the DCC-2 package
baseline and runs current consumers only; it does not require DCC-1 consumers
to build against DCC 2.

The installed CMake config does not trust target existence or only the
`find_package` version. It recursively resolves the selected `llam::runtime`
target's direct and transitive interface include graph, evaluates BUILD/INSTALL
generator-expression branches for installed consumption, canonicalizes and
deduplicates paths, and must find exactly one real `llam/runtime.h`. It parses
that header and hard-fails unless the version is at least 2.2.0 and
`LLAM_ABI_VERSION_MAJOR == 2`; zero candidates, multiple ambiguous candidates,
missing headers, unevaluable interface paths, or an indirect target that cannot
be proven are fatal. Fixtures cover direct includes, transitive interface
targets, BUILD/INSTALL genex, missing header, 2.1.9, valid 2.2.0, and ABI major
3. Source configuration fixtures also reject lower and higher required-version
cache overrides.

Each prior macro source record keeps its normalized conditional definition and
each resolved record keeps its expansion within the matching profile. New
types, enumerators, macros, and permitted record suffixes require an intentional
current baseline update, but editing a current header and baseline together
cannot hide a compat-base incompatibility. Macro bodies stay thin and delegate
behavioral fixes to exported functions.

In `same_major` mode, generate two probe programs from compat-base and current
record manifests and compile/run them separately with the same platform
compiler. Compare `sizeof`, alignment, every stable field offset/size, and
array-stride assumptions. `layout=fixed` rows must match completely. An
extensible row follows its recorded suffix/new-version policy; old-field
offsets/sizes and total alignment remain unchanged, and no externally linked
ABI function may pass or return it by value. Caller-compiled public static-inline value constructors may pass or
return an extensible descriptor by value because no cross-library calling
convention is involved; those function signatures and the record's historical
prefix remain separately frozen. Real historical-header/new-library canaries
are the second same-major gate. In `major_reset` mode, Task 14's separately
compiled synthetic smaller-DCC2-prefix canaries are mandatory instead. Missing
the mode-appropriate headers/probe sources, compiler execution, or a manifest
classification is fatal.

Update `tools/audit_public_api.py` and `tools/audit_release_contract.py` around
this model. The REST audit is an independent semantic gate and must assert the
strict global total of 224 canonical endpoints split 41/35/47/57/44 across
Tasks 6–10, exactly 35 externally exported generic symbols, zero transition helpers or
composites, one declaration/definition per endpoint, and no App REST mirror.
It must also keep the seven unavailable candidate operations—Create Guild,
Delete Guild, Modify Guild Integration, Sync Guild Integration, Bulk Edit Guild
Command Permissions, Create Guild From Template, and the obsolete bot-facing
Get Current User DMs—out of the active surface. Do not derive or overwrite the
hand-reviewed endpoint manifest from whatever names happen to be in the
headers.
The semantic audit additionally requires the exact 17 `sensitive_path` rows,
the exact 20 forced `sensitive_result` REST rows (those 17 plus create/channel-
list/guild-list webhook), and the exact two forced OAuth result operations. It
cross-checks manifest/policy source enforcement rather than trusting booleans.

## Declaration comments and generated reference

Documentation generation does not compensate for undocumented declarations.
First place an immediate public documentation comment on every `DCC_API`
declaration, every public static-inline `dcc_*` or `DCC_*` callable, and every
public installed `DCC_*` macro. Also document every public typedef (including
opaque handles and scalar aliases), struct/union/enum declaration, public
record field, and enum enumerator. A type needs its own lifecycle/extension
contract even when every function using it is documented; a field/enumerator
needs its semantic meaning and units/range rather than only a repeated name.
Immediate leading comments or unambiguous same-line field/enumerator comments
are accepted, but one group comment may not silently cover unrelated members.
The preflight scan found at least
1,202 undocumented declaration occurrences and only 147 documented ones;
duplicate declarations and later tasks can move those raw counts, so the
release invariant is zero undocumented final public items rather than a fixed
denominator.

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
macros receive equivalent behavior and evaluation/ownership notes. The same
tree renders every entry in `api_v2_types.json` and `api_v2_records.json`, with
enum values, field order/types, size/version/presence rules, units/ranges,
extension classification, ownership, producer lifetime or cleanup function,
and links to consuming operations. Opaque handles are included even though
they have no visible fields.

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

## Performance regression gate

Stable requires a measured regression gate, not merely a benchmark executable
that exits zero. Create a versioned workload manifest and one common JSON
artifact schema. The manifest records each workload's semantic fixture hash,
baseline source commit, adapter/harness version, optimization flags, expected
output hash, iteration policy, and deterministic allocation/copy counters.
Freeze the equivalent pre-change baseline before replacing each covered path;
changing a fixture, threshold, baseline commit, or adapter is a separately
reviewed benchmark-policy change.

Build baseline and current executables from their recorded sources and run them
on the same otherwise-idle host. Use identical compiler family/options,
interleave baseline/current order, perform at least three untimed warmups and
30 paired timed batches per executable, and make each batch long enough to
avoid timer quantization. Alternate pair order deterministically as AB, BA,
AB, BA so each pair contains one baseline and one current sample. One sample is
the batch's monotonic `elapsed_ns / exact_operation_count` in double precision;
no per-operation timer calls are allowed.

Sort the 30-or-more batch `ns/op` samples independently for baseline and
current. Compute median and p95 with Hyndman-Fan type 7 linear interpolation:
for zero-based sorted values, `h = (n - 1) * p`, then interpolate between
`floor(h)` and `ceil(h)`. Compute CV as sample standard deviation (`n - 1`
denominator) divided by the arithmetic mean, separately for baseline and
current. Report those statistics, bytes/second where a byte workload exists,
exact operations, allocations, allocated bytes, and copied bytes. A run is
valid only when both configured CV bounds (default 5%) are met; at most two
complete reruns are allowed, after which noise is a release failure rather
than a pass.

For every workload, fail when current median latency is more than 10% above its
paired baseline or current p95 is more than 20% above baseline. Also fail an
unexpected deterministic allocation/copy-count increase even when wall-clock
noise hides it. Sanitizer jobs run benchmark correctness only; timing gates use
optimized native builds. Archive the raw per-batch JSON plus environment,
compiler, CPU, manifest hash, and both executable hashes from the release job.
Each raw sample records pair index, AB/BA order, executable role, monotonic
elapsed nanoseconds, operation count, sample `ns/op`, allocation/copy counters,
and the fixed quantile algorithm identifier `hf7`; the schema rejects mixed
sample units or another quantile rule.

The required semantic workload set is:

1. Gateway payload validate-only and parse-plus-dispatch;
2. listener dispatch with one and 32 matching listeners;
3. normal-message plus tagged button/select validate-measure-serialize;
4. Components-v2 layout validate-measure-serialize;
5. typed REST validation/serialization/admission through the interceptor;
6. Task 11 FIFO enqueue, terminal pump, and dependent-drain behavior.

Every timed path first proves the same expected semantic output. Do not publish
“fastest Discord library” or Twilight/Serenity comparison claims from this
internal regression suite; such a claim requires a separate reproducible,
equivalent cross-library benchmark.

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
- run the canonical paired benchmark set, enforce its median/p95 and
  allocation/copy thresholds, and archive the raw artifacts; execution,
  stability, semantic-output, or threshold failure is a release failure.

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
   declarations, duplicate owners, plain/default-linkage externs, `extern
   inline`, non-inline static or external header definitions, `DCC_API` storage-
   class conflicts, unsorted/stale baselines, same-major removal,
   major reset, missing/moved previous Stable tag, compat-base mismatch or
   equality with HEAD, origin/main already containing HEAD, missing symbol
   tools, missing/mismatched compat shared-library receipt, reset-ledger wildcard,
   stale prior/current fingerprint, ledger-unlisted removal/change, each
   non-layout record-axis/lifecycle mutation, ownership-anchor/minimum-size
   mutation, initializer/cleanup rename, field-meaning or units change, lost
   umbrella/focused-header re-export, removed/renamed installed header, CMake
   package/target/helper/interface mutation, pkg-config module/Cflags/Libs or
   LLAM-floor mutation, 2.1.9 false acceptance, 2.2.0 false rejection,
   lower/higher cache override, direct/transitive/genex header discovery,
   missing/ambiguous header, ABI-major-3 target, prebuilt-receipt mismatch,
   historical-consumer failure, Mach-O underscores, and
   representative Windows export output. Capture RED before changing the
   compatibility logic.
3. Add macro-source parser and 12-profile expansion fixtures, including
   conditional branches, C-only absence, static/shared build roles, POSIX/
   Clang versus Windows/ClangCL exports, and same-profile-only comparison.
4. Add documentation-generator fixtures for nested header paths, multiple
   declarations, Bot macros, ownership fields, deterministic output, stale and
   orphan pages, undocumented declarations, CRLF input, and duplicate owners.
5. Add paired benchmark-runner fixtures for manifest/hash mismatch, wrong
   semantic output, order interleaving, warmup/timed batch counts, CV retry and
   exhaustion, median/p95 regressions, allocation/copy regressions, and stable
   raw JSON output. Capture RED against the execution-only benchmark gate.
6. Add the missing declaration/Bot comments, then generate and check the API
   reference. Rewrite primary guidance and compile every marked snippet against
   a clean installed package.
7. Prepare version/SOVERSION/LLAM/package changes, derive the baseline from a
   new shared build, and combine them with Task 14's held App/Sugar deletion in
   the atomic remaining-compatibility cutover described below.
8. Run the complete normal, sanitizer, platform, package, source-archive,
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
   applies every held Task 14 public removal/replacement (App/Sugar, generic
   REST, OAuth2, firewall/response helpers, components, message/modal raw
   builders, opaque runtime-state conversions, and bounded public writers),
   promotes `dcc_app_run_with_signals` to the focused lifecycle
   header, fixes the eight-header aggregate and env names, sets
   2.0.0/SOVERSION 2, records every final API/ABI baseline, enables strict
   same-major/removal checks, and adds clean archive/install negatives.
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
- complete installed-header FunctionDecl classification with zero plain extern,
  storage-class ambiguity, or declaration/definition/export mismatch;
- all 12 installed-header reachability profiles with every header present and
  no same-major lost transitive external/inline/type/macro key;
- every package-contract profile plus unmodified compat-base CMake/pkg-config
  consumers configured, built, and run against the new install prefix;
- compiler-independent macro-source regeneration plus all 12 exact resolved
  macro profiles, with major-reset or same-major provenance as appropriate;
- exact major-reset ledger equality against reconstructed v1.5.0 artifacts and
  matching inspected compat/current shared-library exports;
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
final symbol count and baseline hash, compat-base/reset-ledger and both library
hashes, duplicate-resolution notes, documentation
coverage totals, generated page inventory, exact version/LLAM/SOVERSION values,
shared-library metadata, package member lists, platform/sanitizer/test results,
benchmark artifacts, commit hashes, and the release dry-run result. Commit the
report separately and stop for independent review; pushing, tagging, and
publishing happen only after that review explicitly approves the final tree.
