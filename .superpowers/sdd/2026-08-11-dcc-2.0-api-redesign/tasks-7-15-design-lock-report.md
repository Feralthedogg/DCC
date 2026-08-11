# Tasks 7–15 design-lock report

Date: 2026-08-11

## Status

The implementation contract is frozen across Tasks 7–15. Exact per-task
layouts, endpoint rows, validation masks, removal inventories, test matrices,
and commit constraints live in the nine task briefs beside this report. The
central design and implementation plan have been synchronized to these briefs.

“Frozen” means an implementer does not invent another callback model, endpoint
overload, response model, listener ABI, resource policy, or release boundary.
A discovered factual defect is fixed in the authoritative brief and propagated
through this report/design/plan before production code proceeds.

## Authoritative inputs

In descending specificity:

1. the current Task 7–15 brief for the task being implemented;
2. `docs/superpowers/specs/2026-08-11-dcc-2.0-api-redesign-design.md` for
   cross-task contracts;
3. `docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md` for order,
   checkpoints, and commit boundaries;
4. completed Tasks 1–6 behavior and checked manifests/tests;
5. official Discord documentation for endpoint/schema facts.

Older snapshots are explicitly non-authoritative when they mention 231 active
endpoints, final 60/284 REST exports, a fixed 283 Bot macro count, a public
interaction-flow struct/init/deinit, an inline `dcc_bot_run`, or a mandatory
Bot name.

## Frozen surface arithmetic

### Endpoints

| Task | Exact active endpoints | Cumulative |
| --- | ---: | ---: |
| 6 | 41 | 41 |
| 7 | 35 | 76 |
| 8 | 47 | 123 |
| 9 | 57 | 180 |
| 10 | 44 | 224 |

The seven rejected candidates exist only in the removal ledger: Create Guild,
Delete Guild, Modify Guild Integration, Sync Guild Integration, Bulk Edit Guild
Command Permissions, Create Guild From Template, and obsolete bot-facing Get
Current User DMs.

### Generic REST

| Boundary | Generic exports | Endpoint exports | Total exported `dcc_rest_*` |
| --- | ---: | ---: | ---: |
| Task 10 | 60 | 224 | 284 |
| Tasks 11–13 | 61 | 224 | 285 |
| Task 14/15 final | 35 | 224 | 259 |

The Task 11 addition is `dcc_rest_runtime_stats`. The final 35 names are listed
exactly in Task 14. The three initializers for call options, request descriptor,
and firewall options are static-inline source API, not exported operations.

### Bot macros

There is no fixed 283 count. Task 13 records the canonical POSIX/Clang C11
static-consumer compiler dependency artifact and adds exactly 30 Bot-owned
definitions. Final canonical C count is dependency records + 30 and at most
300. C++ omits exactly the nine C-only variadic UI macros. Task 13 also smokes
the 12 POSIX/Windows × C/C++ × static/shared-build/shared-consumer profiles;
Task 15 baselines their exact resolved expansions. Uppercase `DCC_*`
constructors are functions and are tracked by the inline API baseline, not the
macro dump.

## Cross-task transport contract

`dcc_rest_call_options_t` ends in this exact order after its historical prefix:

```text
audit_log_reason
auth_mode
auth_token
flags
```

`dcc_rest_request_desc_t` is exactly `size, version, method, path,
content_type, body, body_len, const dcc_rest_call_options_t *options`. The
pointed options record is independently validated/normalized and borrowed only
through submit return; null selects defaults. No extensible options value is
embedded in the raw descriptor.

The only flags are sensitive request body and sensitive result body. Typed
manifest policy ORs mandatory sensitivity into caller flags. Add Guild Member,
Add Group DM Recipient, and Create or Join Lobby force sensitive request bodies.
OAuth forms/headers force request sensitivity; exchange/refresh results force
result sensitivity.

Every request preserves a canonical nonsecret operation separately from its
owned wire path. The exact 17 interaction/webhook token paths are sensitive.
All 17 also force sensitive results; Create Webhook, Get Channel Webhooks, and
Get Guild Webhooks make the exact REST result set 20, while OAuth exchange and
refresh are the exact separate two-operation set.
Raw route keys use SHA-256 of the complete path; neither credentials nor token
segments enter logs, diagnostics, observers, or public result identity. Every
secret temporary/final allocation is length-tracked and securely wiped,
including replaced buffers, clones, rollback, cancel, retry, and teardown.

`dcc_rest_result_t` has a final `uint64_t flags` suffix. Result access stays raw
bytes plus nine scalar/object helpers. This is documented as typed request/raw
result. Optional owned domain decoders are a post-2.0 roadmap and never run
unless called.

## Typed request details locked across tasks

- Task 7 owns typed message-reference and attachment metadata; raw equivalents
  coexist only until Task 14.
- Task 8 owns exact guild/member/role bodies and the Add Member secret rule.
- Task 9 shares neutral localization/application context types and uses the
  command-sync plan as sole scope/delete/dry-run authority. Apply deep-copies
  its executable plan before returning success, so source plans can deinit.
- Task 10 completes the 224 rows, uses neutral role-connection localization,
  forces the two remaining JSON secrets, and returns one managed-message
  aggregate result with exact failing stage/status provenance.
- Task 14 moves all five OAuth2 helpers onto request/result handles; code
  exchange supports a presence-gated 43..128-character PKCE verifier and wipes
  it as secret.

## Bounded runtime and ordered interactions

Task 11 makes `dcc_interaction_flow_t` opaque. Public lifecycle is create with
`client + interaction + out` and consuming destroy. There is no public init,
deinit, layout, or state snapshot.

One client registry is shared by App contexts and standalone flows. Only a new
identity reaches middleware/user dispatch. Live/completed duplicates are
suppressed before user code. Every live identity reserves a future tombstone
slot; retirement cannot fail due to a full table. Tombstones retain IDs,
SHA-256 token fingerprint, and a checked/saturating monotonic expiry, never raw
tokens. Unexpired tombstones are never evicted.

One queue mutex owns FIFO/projected/committed state and auto-defer arbitration.
Only the head enters REST, and the successor waits until callback plus observers
return. A private post-terminal hook connects Task 4 completion to the pump.

The eleven client resource options and defaults are:

```text
REST queued requests                  4096
REST request bytes                    32 MiB per request
REST queued bytes                     128 MiB
REST active bytes                     128 MiB
REST response bytes                   32 MiB per response
interaction live queues               1024
interaction tombstones                65536
interaction tombstone TTL             20 minutes
interaction actions                   32 per queue, active included
interaction reserved bytes            32 MiB per queue
interaction reserved bytes            128 MiB total
```

All paths use exact measure, atomic reserve, then exact build. No uncharged
temporary or raw growth realloc is allowed. Queue identity, node, and payload
have distinct reservations. Only payload transfers to REST while the node stays
interaction-owned through post-hook free; copied token/key identity is charged
before copy and held until tombstone conversion. Fixed registry/tombstone
storage is checked/preallocated from count bounds.
`DCC_ERR_RESOURCE_LIMIT` is appended. Runtime stats are one versioned,
linearizable snapshot with exact fields/rejection counters fixed in Task 11.

## Builder and listener ABI

Task 12 replaces the Components-v2 field bag with the exact tagged union and an
independent expected-layout skeleton. Contexts are ANY, MESSAGE_LEGACY,
MESSAGE_V2, and MODAL. Legacy messages allow at most five top-level Action Rows;
V2 uses its separate 40-total grammar. Validate, measure, and serialize share
one stride-aware traversal.

Button fields follow a complete style matrix: styles 1–4 require custom ID,
forbid URL/SKU, and require label or emoji; Link requires URL, forbids custom
ID/SKU, and requires label or emoji; Premium requires nonzero SKU and forbids
custom ID, URL, label, and emoji. Entity-select defaults are user-only,
role-only, channel-only, or user/role for Mentionable; String Select has no
`default_values`.

Task 13 normalizes the listener graph before adding Bot helpers:

- config/listener point to policy, bindings, validators, and validation policy;
- policy points to optional cooldown;
- array element leaves and target variants/union are fixed for ABI major 2;
- target union is the listener's frozen final tail;
- `dcc_app_listen()` synchronously deep-copies all descriptors/strings/arrays;
- `dcc_listener_init()` is a same-name static-inline function.

`dcc_bot_config_t` contains size, version, App options pointer, token-env,
default-error toggle, and signal toggle—no unused name. `DCC_BOT_CONFIG()` takes
no argument. `dcc_bot_run` is one exported function with a private managed-entry
guard and blocking no-leak cleanup. Its liveness precondition is returning user
code; teardown does not claim to cancel a non-returning callback.

## ABI baseline model

Every public aggregate receives independent `layout`, `direction`, `lifetime`,
`writer_policy`, and `append_policy` values. Layout is exactly opaque/fixed/
size-extensible/versioned; lifetime is trivial/borrowed/owning/state-handle.
Size-only and size+version both support immutable prefixes. Fixed layout never
grows in ABI 2; ownership never decides layout comparison.

An extensible child cannot appear by value before later parent fields. No
externally linked ABI function passes/returns an extensible record by value.
Caller-compiled static-inline constructors may do so; their signatures and
record prefixes are separately frozen. Arrays either use explicit historical
stride or fixed element layout.

Same-major comparison freezes every existing record's direction, lifetime,
writer/append/stride policy, initializer and empty predicate, cleanup/consume
operation, ownership anchor/minimum success size, required logical groups,
ownership/producer boundary, and old-field meanings and units in addition to
its layout rules. Lifecycle changes require a newly named type/API.

The audit covers every public record writer, including mutation, parse/build/
snapshot output, deinit/reset, and open/close—not only `_init`. Each writer is
fixed-layout, caller-size/version-bounded with atomic covered-field commits, or
opaque create/open plus consuming destroy/close. Historical canaries exercise
all writer classes; current-layout `memset`/assignment is forbidden for an
extensible output.

Owning outputs are never implicitly replaced. A producer accepts only a valid
initializer-empty record; populated output returns `DCC_ERR_STATE` before
allocation or input/stream consumption and remains bytewise unchanged. Success
also requires complete coverage through the ownership anchor and logical
view/count group; shorter/partial coverage fails pre-allocation and unchanged.
It commits a temporary atomically; reuse is deinit plus reinitialize. Snapshot
parse, plan build, and replay-next carry the explicit matrix.

Command registry, component session/store/listener, replay recorder/player,
firewall, store, and interaction flow are opaque runtime handles. The unused
public command-registry entry record is internalized. Command-registry snapshot/
plan and replay record remain bounded owning values with caller-compiled
initializers and exact deinitializers.
Their exact rows use `layout=size_extensible`, `lifetime=owning`,
`writer_policy=covered_atomic`, and `append_policy=suffix_same_layout`;
snapshot is output, plan/replay are inout.
Replay's exact prefix is `size, state, kind, ts_ms, event, payload,
payload_len`: null state is borrowed recorder input, while non-null state owns
one player-output allocation. Deinit never frees borrowed bytes, is idempotent,
and non-null-state records are move-only.

The final checked artifact set is:

- previous-Stable compat-base pin plus the complete reviewed 1→2 removed/
  changed external, inline, type, and macro ledger;
- every installed-header FunctionDecl classified by source spelling, linkage,
  storage, definition, and owner;
- every installed header's transitive external/inline/type/macro reachability
  set in each exact platform/language/build-role profile;
- per-profile CMake package/target/helper/interface and pkg-config module/
  Cflags/Libs semantics;
- external symbol names;
- normalized external declarations/signatures;
- every lowercase/uppercase public inline callable;
- every typedef and enum numeric value;
- every public record layout/lifecycle/extension class;
- compiler-independent conditional source definitions for every public
  `DCC_*` macro per owner header;
- exact resolved macro expansions for all 12 platform/language/build-role
  profiles;
- focused canonical POSIX/static Bot dependency/full macro dumps.

Complete current hashes prove artifact freshness. Same-major compatibility is
type-kind-specific: exact scalar/alias/callback and opaque identities, preserved
prior enum name/value pairs, exact fixed records, and immutable extensible-
record prefixes with suffix-only growth.
Plain/default-linkage externs, `extern inline`, non-inline header definitions,
storage ambiguity, and duplicate owners are fatal before declaration↔definition↔
export comparison.

The initial 1→2 reset pins previous Stable `v1.5.0` at
`6ba1e29a45ec3617cf17e05b716c54b8094bbf96`, then checks the exact DCC 1
removed/changed ledger, current artifact freshness, and synthetic smaller-DCC2-
prefix canaries. Later 2→2 comparisons pin the latest previous Stable v2
tag/commit and run real old-
header/new-library probes. `origin/main` is ancestry evidence only; compat-base
equal to HEAD is fatal. Resolved macros compare only within a matching profile;
missing mode-appropriate probe evidence is fatal on every supported ABI family.
The reset ledger is exact per name/key and prior fingerprint with task/reason;
wildcards are forbidden. Reconstructed v1.5.0 removed/changed sets must equal it,
and every unlisted prior item remains subject to the structural comparator.

The 2→2 source gate also requires every prior installed header path and a
per-profile superset of the external, inline, type, and macro keys transitively
reachable from it. Header removal/rename and lost umbrella/focused-header
re-exports are fatal even when physical owner declarations are unchanged. The
initial 1→2 reset only establishes this DCC-2 baseline.

The `SameMajorVersion` promise is backed by the prior installed-package
contract: `find_package(dcc CONFIG)`, `dcc::dcc`, the exact public hot-reload
helper grammar/behavior, normalized required target interface, and pkg-config
`dcc`/`DCC`/Cflags/Libs/private dependency roles remain exact per matching
profile. Package/subdirectory/provided profiles require
`Requires.private: llam >= 2.2.0`; 2.1.9 is rejected and 2.2.0 static-links/
runs. Prebuilt mode verifies the LLAM version/ABI/library receipt and private-
link roles. The installed config resolves direct/transitive/genex target
includes to exactly one real LLAM header and proves version >=2.2.0 plus ABI
major 2; missing, ambiguous, or unprovable headers fail. The required-version
input is non-configurable except for an exactly equal compatibility override.
Unmodified compat-base CMake and pkg-config consumers configure,
build, and run against the new install; the major reset only seeds this DCC-2
baseline.

## Complete Task 14/15 cut boundary

Preparation commits may add migration tools, private engines, tests, coexisting
replacements, and migrate repository consumers. They retain old public ABI.

One shared `feat!` commit applies every held Task 14 public cut and simultaneously
sets 2.0.0/SOVERSION 2 plus all final baselines. Held work includes:

- Sugar/legacy App deletion and 361 App/context removals;
- generic REST 61-to-35 and old request/future/response helpers;
- OAuth2, firewall, response-helper, component, message, and modal replacements;
- command-registry/component-session/replay/store opaque-state replacements and
  the complete public-writer bounded-write cut;
- component header/13 source/21 export deletion;
- raw builder field/setter deletion;
- final App aggregate/env names and every same-name ABI replacement.

There is no commit that advertises ABI 1 with any half of this public cut.

## Documentation, package, and performance gates

Every public external/inline function, macro, typedef/type, record field, and
enum enumerator is documented at source and rendered into deterministic
per-header reference. Complete snippets compile as C11/C++17 against a clean
install. Primary guidance contains no removed path or Beta claim and accurately
describes typed request/raw result semantics.

Source-package verification runs from the release checkout with five mandatory
inputs: clean extracted archive root, reference repository, full pinned
previous-Stable compat-base hash, a matching shared library built only from that
compat-base, and a shared library built only from the archive. Two build
receipts and a hashed compatibility-provenance bundle prevent skipped or
substituted inputs.

Performance uses at least 30 paired AB/BA batches after warmup. Batch `ns/op`
median/p95 use Hyndman-Fan type 7; CV uses sample standard deviation. Release
fails after two noisy reruns, over 10% median regression, over 20% p95
regression, or deterministic allocation/copy growth. Raw JSON records exact
units, pair order, hashes, counters, and `hf7`.

## Execution readiness

Task 7 RED is committed and Tasks 7–15 may now be implemented in order. A task
is complete only after its focused RED becomes green, the full required gates
pass from clean/current build roots, its implementer report records evidence,
and an independent review finds no release-blocking contract deviation.

No push, tag, or publication is implied by this design lock.
