# Task 5 brief — version core caller-owned builders

Implement only Task 5 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The
`ABI-safe Value and Builder Types` section of the design and the refined Task 5
steps are authoritative. Work test-first, do not push, and do not edit the
ignored progress ledger. Keep every intermediate commit buildable.

## Scope and staging

Version these evolvable caller-owned inputs:

- `dcc_message_builder_t`
- `dcc_poll_builder_t`
- `dcc_allowed_mentions_builder_t`
- `dcc_embed_builder_t`
- `dcc_modal_builder_t`
- `dcc_autocomplete_choice_t`
- `dcc_autocomplete_builder_t`
- `dcc_application_command_option_builder_t`
- `dcc_application_command_builder_t`

Each layout begins, in that order, with:

```c
size_t size;
uint32_t version;
uint64_t present;
```

Give every type its own version constant, public presence-bit constants,
constant initializer macro, and null-safe initializer. Add missing initializers
for poll, allowed mentions, and command options. Initialization and every
setter remain allocation-free. Remove the redundant `has_*` bytes from these
types; the mask is authoritative.

This task does not redesign `dcc_component_builder_t` or
`dcc_component_v2_builder_t`; Tasks 10 and 14 own those layouts. Fixed leaf
records embedded by value or used in fixed arrays, including embed
field/footer/media/author and poll media/answer records, must not be enlarged in
ABI 2. Keep 1.x raw-JSON and legacy-component builder members only as
transition-only, size-gated fields so current tests stay green; Task 14 removes
them before release.

## ABI and presence rules

- Read `size` first. Read `version` only when its field is covered, and read
  `present` only when the complete common prefix is covered. Accept larger
  sizes and version 1; reject zero/short size and every unsupported version.
- Use subtraction-based field coverage checks after verifying the offset, so
  `offset + width` cannot overflow.
- Define one presence bit per logical setter/serialized field group. Reject
  unknown bits and any set bit whose complete pointer/count or grouped value is
  not covered by `size`.
- An optional uncovered or absent field is not read and behaves as absent. A
  field required for the active builder/type returns `DCC_ERR_INVALID_ARG` when
  absent or uncovered. Preserve current null/empty serialization semantics in
  this ABI task; a successful setter updates the bit consistently with that
  behavior.
- Every setter validates builder prefix/version and target-field coverage
  before its first write. Rejection returns `DCC_ERR_INVALID_ARG` and leaves
  the entire declared object and surrounding canaries unchanged.
- Initializers zero the current layout, publish current `size` and version, and
  set no fields present except values explicitly supplied by that initializer.
  Null initializer input remains a no-op.

Centralize these rules in small internal helpers per family or a shared private
helper. Do not expose implementation-only macros from public headers.

## Versioned array stride

Pointer-plus-count arrays of versioned values cannot use `array[i]`, because an
older DCC 2 caller compiled a smaller stride. For embed-builder arrays,
autocomplete-choice arrays, top-level command arrays, recursive command-option
arrays, and every transitive use:

1. validate the first element and take its declared `size` as the byte stride;
2. reject a stride below that type's minimum prefix, arithmetic overflow, or an
   address unsuitable for the required alignment;
3. locate later elements with checked byte arithmetic;
4. require every element to declare the same stride and supported version;
5. validate/read each element only through covered-field helpers.

Update message/embed/autocomplete/command serializers, validators, and command
registry ownership code accordingly. Registry copies must allocate current
full-layout arrays, initialize each destination, copy only covered/present
fields, and deep-copy every retained string/array recursively. Do not use raw
source struct assignment or `memcpy(sizeof(current_type))`. Cleanup always
walks the normalized destination layout, including partial-OOM rollback.

## Required RED and regression coverage

Create and register `dcc_builder_v2_abi_smoke`. First capture a RED showing the
current layouts lack version/presence validation and serializers read poisoned
suffix fields.

The green fixture must cover all nine types and include:

- initializer macro/function values, null init, exact current layout, accepted
  larger size, zero/short size, bad version, and unknown bits;
- hand-checked historical prefixes with poisoned suffixes and external
  canaries; optional suffix omission and required-field rejection;
- uncovered-bit rejection without reading poisoned pointer/count/value bytes;
- every setter family on a valid current builder plus representative covered
  and uncovered historical prefixes, with byte-for-byte no-mutation failure;
- explicit zero scalar values and the current null/empty pointer/count behavior;
- two-element historical-stride embed, autocomplete-choice, command, and nested
  command-option arrays with distinct output, plus mismatched stride/version
  and overflow rejection;
- command-registry deep-copy lifetime: mutate/free all caller-owned strings and
  arrays after registration, then serialize/use the normalized copy; inject
  allocation failures at nested stages and prove exact rollback/cleanup;
- existing message, poll, allowed-mentions, embed, modal, autocomplete,
  command-registry, interaction-flow, App-listener, HTTP, Sugar transition, and
  package-consumer behavior.

Use aligned byte fixtures and checked casts; do not create undefined behavior
inside the test. All poison/canary assertions must be deterministic under
normal, ASan/UBSan, and MinGW builds.

## Verification and handoff

Run at minimum:

- `dcc_builder_v2_abi_smoke` and all existing builder/component/command tests;
- HTTP, command registry, App listener/structured-error, interaction-flow, and
  package-consumer focused targets;
- standalone C11 and C++17 compilation for all five public headers;
- native and i686/x86_64 MinGW layout/header/source probes;
- focused ASan/UBSan and TSan where registry ownership can race;
- the full enabled CTest suite after focused verification is green;
- public-surface, transition, source-layout, package/install, and diff audits.

Commit implementation/tests with `feat: version public message builders`.
Write a separate `task-5-implementer-report.md` beside this brief containing
the RED evidence, exact layouts/minimum prefixes, changed files, verification
commands/results, sanitizer/platform evidence, known pre-existing failures,
and the implementation commit hash. Commit the report separately and stop for
review.
