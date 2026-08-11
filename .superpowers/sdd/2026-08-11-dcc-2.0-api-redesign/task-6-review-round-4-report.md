# Task 6 review round 4 — exact multipart payload preflight

## Outcome

The round-4 production and regression changes are committed as
`f3381dcea17f1db8b6c5d6cfe22e2a7f5dbd5ea3`
(`fix: measure multipart payloads before allocation`; 21 files,
+1124/-228), based on `65de159`. No Task 7 work was started and no push was
performed.

Message, initial-interaction-response, and webhook-execute endpoints now
measure the exact JSON payload that will be placed in `payload_json` before
the first construction allocation. They combine that exact value length with
all multipart file parts through the same checked arithmetic used by the
normal multipart builder. An unrepresentable aggregate therefore fails local
preflight with `DCC_ERR_INVALID_ARG`, a null request output, no admission, and
no callback or observer delivery.

No public header, public record layout, or `DCC_API` declaration changed. All
new measurement declarations are private internal declarations. Raw JSON
escape hatches remain opaque and are measured byte-for-byte without parsing,
normalization, or semantic reinterpretation.

## Correction to the round-3 report

The round-3 report said that JSON larger than the minimum `{}` multipart
field could legitimately reach a later `DCC_ERR_NOMEM`. That statement was
incorrect for arithmetic representability failures.

The exact serialized length is deterministic and can be obtained without
allocating. When files are present, endpoint preflight must combine that exact
length with the multipart headers, file data, CRLFs, and closing boundary
before any JSON or multipart buffer growth. Only an actual allocator failure
after a representable aggregate passes preflight remains a legitimate
`DCC_ERR_NOMEM`; payload-plus-files arithmetic overflow is now always a local
`DCC_ERR_INVALID_ARG`.

## Root cause

Round 3 added checked multipart aggregation but endpoint preflight supplied a
fixed `payload_json={}` field. A file fixture could therefore produce a valid
minimum envelope of `SIZE_MAX - 1` while the real message, interaction
wrapper, or merged webhook object added more bytes. The endpoint then passed
preflight and reached one or more serializer-buffer growth attempts before
the aggregate was rejected.

The three representative RED results were:

```text
message rejects actual payload length overflow before allocation rejection detail status=invalid argument request=0x0 endpoint=0 json=1 multipart=0 capture=0/0 callback=0/0 observer=0/0
interaction rejects actual payload length overflow before allocation rejection detail status=invalid argument request=0x0 endpoint=1 json=1 multipart=1 capture=0/0 callback=0/0 observer=0/0
webhook rejects actual payload length overflow before allocation rejection detail status=invalid argument request=0x0 endpoint=0 json=1 multipart=1 capture=0/0 callback=0/0 observer=0/0
REST v2 endpoint smoke failed
```

The exact-measure contract was also added before the private APIs existed and
produced the expected link RED for:

```text
dcc_message_builder_measure_json
dcc_endpoint_measure_interaction_message_json
dcc_endpoint_measure_webhook_execute_json
```

## Allocation-free exact JSON measurement

The private message, legacy-component, Components V2, and embed JSON buffers
now support a count-only mode. Their existing append functions perform the
same trailing-NUL overflow checks and advance the exact encoded length, but
do not reserve, copy, or allocate. Actual builds continue to use the normal
buffer mode.

The structured nested paths reuse their real append encoders:

- embed array build and measurement share the historical-stride-aware array
  traversal and per-embed serializer;
- legacy component array build and measurement share the same recursive
  component encoder;
- Components V2 build and measurement share the same V2 array encoder and
  retain message-context validation;
- poll serialization now appends through one shared message-buffer encoder;
  and
- the top-level message object and braces-free message members are shared by
  build, measure, interaction wrapping, and webhook merging.

For nested encoders that use their own private buffer type, count-only
measurement returns the exact nested length and the message count sink adds
that known length without requiring a value pointer. Raw JSON fields still
use their literal `strlen` and are appended unchanged.

The interaction message wrapper now shares one encoder for
`{"type":N,"data":<message>}` in both build and count modes. Webhook execute
likewise shares one encoder for the merged message members, username,
avatar URL, thread name, and applied-tag IDs. Query-only webhook fields are
not counted in the body.

## Exact multipart primitive and endpoint preflight

The multipart validator now has one private primitive that accepts a field
name and exact field value length without a field value pointer. Normal
pointer-backed measurement and the endpoint primitive share the same helpers
for field headers, file headers, filenames, content types, data lengths,
CRLFs, boundaries, and the trailing buffer NUL constraint.

For requests with files:

- message preflight measures the message JSON, or exactly two bytes for the
  supported null-message `{}` payload;
- initial interaction preflight measures the complete response wrapper with
  its message data; and
- webhook execute preflight measures the complete merged request body.

Each endpoint then passes the exact body length to the multipart primitive.
Any JSON-count or aggregate-size failure is mapped to local
`DCC_ERR_INVALID_ARG` before endpoint, message-JSON, or REST-buffer allocation.

## Regression contracts

`tests/support/rest_v2_endpoint_preflight_contracts.c` now proves exact
measurement against actual output for:

- two historical-stride structured embeds containing JSON escapes;
- a legacy action row and button;
- a structured poll, structured allowed mentions, sticker IDs, and escaped
  message content;
- raw message reference and attachment fields;
- a separate fully raw message covering raw message reference, allowed
  mentions, embeds, legacy components, attachments, and poll fields;
- a structured Components V2 message with raw attachments;
- the complete initial-interaction wrapper; and
- the merged webhook body with escaped webhook metadata and applied tags.

The three measurement calls execute under the message-JSON and REST-buffer
allocation probes and make zero allocation attempts. Their measured lengths
equal the actual built lengths. A direct primitive parity assertion also
proves that pointer-backed `payload_json={}` measurement and known-length
measurement both produce `SIZE_MAX - 1` for the maximum valid boundary
fixture.

The independent overflow fixture uses the same valid metadata with
`data_len == SIZE_MAX - 272`. The minimum `{}` envelope is representable at
`SIZE_MAX - 1`, while each real endpoint JSON payload makes the aggregate
overflow. Message, interaction, and webhook calls all prove:

- `DCC_ERR_INVALID_ARG`;
- `*out_request == NULL`;
- zero endpoint, message-JSON, and REST-buffer allocation attempts; and
- no interceptor admission, callback, or observer delivery.

This is representative three-family probe evidence. It is not a claim that
the allocation probe directly executed every one of the 41 Task 6 wrappers.
The prior generic `SIZE_MAX`, minimum-envelope, later-file, malformed nested
builder, historical-layout, and valid maximum measurement regressions remain
enabled.

## Verification

Native and focused evidence on the production tree:

- `cmake --build build-task6 --parallel 8` — PASS.
- Focused `dcc_rest_v2_endpoint_smoke` — PASS.
- Final endpoint repeat — PASS, 10/10 in 9.22 seconds.
- Fresh final full enabled suite — PASS, 181/181 in 149.19 seconds.
- `git diff --check`, staged diff check, production-commit diff check, and the
  no-public-header diff assertion — PASS.

Compiler and sanitizer evidence:

- All 11 changed production C translation units and the changed endpoint
  support translation unit passed Clang C11 with
  `-Wall -Wextra -Wpedantic -Werror`.
- All 11 changed production C translation units passed warning-as-error
  syntax compilation with both x86_64 and i686 MinGW, 22/22 probes total.
- Final ASan/UBSan endpoint plus builder matrix — PASS, 2/2 in 1.16 seconds
  with `detect_leaks=0` and no diagnostic.
- Final TSan endpoint plus builder matrix, each repeated three times — PASS,
  6/6 in 5.53 seconds with no report.

Manifest, API, documentation, and package gates:

- REST endpoint audit self-test — PASS; 99 malformed fixtures rejected.
- REST endpoint progress audit through Task 6 — PASS; 231 endpoints, Task 6
  count 41, and 190 later-task endpoints explicitly deferred.
- V2 transition surface audit — PASS; 455 headers checked.
- Project layout, source-package, REST-surface, and public-API audits — PASS;
  the public-API audit checked 1789 declarations and 3789 library symbols.
- `python3 -m mkdocs build --strict` — PASS. Material's MkDocs 2.0 notice and
  the existing non-nav pages remained informational only.
- Fresh installation to `/tmp/dcc-task6-round4-install-20260811` — PASS.
- A fresh external package consumer configured in
  `/tmp/dcc-task6-round4-consumer-20260811` against that installation and the
  explicit local LLAM root/library, then built and ran successfully.

## Separate timing observation

The first full suite completed 180/181 in 149.40 seconds. The only failure was
the already documented, unrelated
`dcc_hot_reload_isolated_last_good_smoke`: it received the health request and
both expected slash/async pairs but reached the five-request server cap before
the fallback counters advanced. Four immediate standalone CTest reruns
reproduced the same 5.63–5.83 second timeout; a direct binary invocation also
printed the same diagnostic. No hot-reload production or test file was changed
by this round.

The required fresh final full-suite rerun subsequently passed 181/181,
including the same last-good test in 2.81 seconds. Both the failure sequence
and final green run are reported rather than hiding the known timing behavior.

## Boundaries retained

- Raw JSON remains opaque and unparsed.
- Task 6 endpoint semantics, legacy bridges, public record layouts, and
  ownership rules are unchanged.
- Tasks 7–10 still own the 190 explicitly deferred endpoints.
- The broader pre-existing HTTP event-wait TSan debt documented in earlier
  reports was not expanded; the focused round-4 TSan matrix is clean.
- The optional local Discord documentation mirror was unavailable, so the
  checked design, Task 6 brief, manifest, and existing official references
  remained the contract sources.

The authoritative design, implementation plan, and ignored progress ledger
were not edited. No push was performed.
