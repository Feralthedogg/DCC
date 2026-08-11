# Task 6 brief — endpoint manifest and first request-based domains

Implement only Task 6 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The REST section
of the design, the refined Task 6 plan, and this brief are authoritative. Work
test-first, do not push, and do not edit the ignored progress ledger. Do not
start Task 7 domains. Keep intermediate commits buildable.

## Outcome

Create one checked manifest for every Discord REST endpoint currently covered
by DCC, then fully migrate the message, interaction, and webhook domains to the
Task 4 request handle. Each migrated endpoint has exactly one public typed
submission function. It returns after validation, serialization, ownership,
and queue admission; final HTTP or transport status is observed through the
optional request handle, callback in call options, and error observer.

Remove the converted domains' public raw JSON, callback/user-data, `_builder`,
`_multipart`, `_params`, `_options`, `_page`, `_thread`, and
`_from_interaction` overloads. Do not add `_v2` names or endpoint-specific
future/sync/async adapters. Generic `dcc_rest_submit()` remains the raw escape
hatch. Unconverted Task 7–10 endpoint declarations remain transition debt and
must be enumerated by the progress audit, not silently ignored.

## Canonical common signatures and ownership

Every canonical endpoint ends with:

```c
const dcc_rest_call_options_t *options,
dcc_rest_request_t **out_request
```

Null options select `DCC_REST_CALL_OPTIONS_INIT`; null `out_request` means
auto-release. On local rejection, set `*out_request = NULL`, do not run a
callback or observer, and leave no queued request. On `DCC_OK`, exactly one
terminal result is guaranteed. The endpoint must serialize or copy all path,
query, body, file, string, and array input needed after return. Only the
callback and its `user_data` retain the Task 4 borrowed lifetime.

Simple required IDs stay as scalar arguments. Optional query groups and
payloads use caller-owned version-1 input records beginning with `size` and
`version`, with an initializer function and complete constant initializer.
Use presence bits wherever zero/false differs from omission. Accept larger
records and covered historical prefixes; reject unsupported versions, unknown
bits, uncovered present fields, invalid pointer/count pairs, and arithmetic
overflow before the first allocation or output mutation.

Use one private typed-endpoint submission helper so every wrapper applies null
call options, initializes `out_request`, selects method/content type, and calls
the Task 4 admission path identically. Do not wait on a request. Reuse the
existing route encoders and multipart encoder. A completed JSON or multipart
body may be copied by `dcc_rest_submit()`; it must never be reserialized on a
worker or borrow caller file bytes.

## Shared message payload

Add `dcc_rest_message_payload_t` for every message-bearing endpoint:

```c
typedef struct dcc_rest_message_payload {
    size_t size;
    uint32_t version;
    const dcc_message_builder_t *message;
    const dcc_rest_multipart_file_t *files;
    size_t file_count;
} dcc_rest_message_payload_t;
```

Publish `DCC_REST_MESSAGE_PAYLOAD_VERSION`, a complete initializer macro, and a
null-safe initializer accepting the message pointer. The record borrows its
inputs only for the duration of the endpoint call. A nonzero file count
requires a non-null array; every fixed file record requires a field name,
filename, valid optional content type, and a valid data pointer/length pair.
The entire multipart body, including exact file bytes and embedded NULs, is
built before queue admission. Zero files use the JSON request path. A null
message is allowed only when at least one valid file makes the operation
meaningful; encode an empty payload object in that case. A null message with no
files is invalid.

Use this payload for channel-message create/edit, original interaction response
edit, followup create/edit, and webhook-message edit. This is the one mechanism
that replaces every JSON/builder/multipart overload. Bulk message deletion
instead takes a fixed snowflake array and count and serializes it before return.

## Typed pagination

Replace raw query strings and `_page` overloads with versioned typed query
records. Prefer a small reusable `dcc_rest_id_page_t` (`before`, `after`,
`limit` presence bits) and a message-list query that additionally supports
`around`; enforce endpoint-specific allowed bits, mutual exclusion, and Discord
limit ranges. Channel pins use an ISO-8601 `before` string plus limit. Query
builders must percent-encode values and compare against literal expected paths
in tests. Null query means endpoint defaults.

## Tagged interaction response input

Add one versioned `dcc_rest_interaction_response_t` with response `type`, an
explicit `with_response` presence/value, and a named union of borrowed pointers
to `dcc_message_builder_t`, `dcc_modal_builder_t`, or
`dcc_autocomplete_builder_t`. Provide constructors or setters for message,
modal, autocomplete, pong, defer-message, defer-update, premium-required, and
launch-activity forms without allocation.

`dcc_rest_interaction_response_create()` is the only initial-callback endpoint:

```c
dcc_status_t dcc_rest_interaction_response_create(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const dcc_rest_interaction_response_t *response,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);
```

The tag determines the active union member. Reject cross-tag data, missing
required builders, unsupported types, malformed nested builders, and invalid
boolean values before admission. Remove all named response and
`_from_interaction` public helpers; App/interaction-flow code extracts IDs and
tokens and calls this one endpoint. Original-response and followup endpoints
take application ID/token explicitly and use `dcc_rest_message_payload_t` for
writes.

## Webhook inputs

Use one versioned webhook execute input containing the typed message builder,
optional files, and presence-gated webhook-only fields/query options: username,
avatar URL, thread name, applied tag IDs, wait, thread ID, and
`with_components`. Merge typed webhook-only JSON members with the message
object without accepting endpoint-specific raw JSON. Build one multipart body
when files are present.

Use one versioned webhook create/update builder for name, avatar, and channel
ID; create requires a non-empty name, while update remains PATCH-capable. Get,
modify, and delete accept an optional token in one canonical operation instead
of `_with_token` overloads. Webhook message get/edit/delete use one optional
thread-ID field rather than `_thread` suffixes. Preserve current route
authentication behavior and validate tokens before formatting paths.

## Complete endpoint manifest

Create `tools/rest_v2_endpoints.json` with a top-level schema version and a
stable, sorted entry for every Discord endpoint exposed anywhere under
`include/dcc/rest/`. Each entry records at least:

- canonical symbol and owning public header;
- domain and migration task (6, 7, 8, 9, or 10);
- HTTP method and canonical route-formatter identity;
- typed input type or `null` for scalar-only operations;
- implementation source;
- multipart capability;
- every current legacy public symbol collapsed into that endpoint.

Exclude only a small immutable list of generic REST infrastructure operations
(request/result/firewall/configuration helpers) that are not Discord
endpoints. A public endpoint symbol may belong to exactly one manifest entry.
Reject duplicate canonical symbols, duplicate legacy symbols, missing files,
unknown fields/domains/tasks/methods, unsorted entries, and method/route
collisions unless explicitly justified by distinct Discord semantics. The
manifest is the source of truth, not a generated list of whatever headers
happen to contain.

Create `tools/audit_rest_v2_endpoints.py`. Strict mode is the default and
requires every manifest entry to have exactly one canonical public declaration
and external definition, with matching symbol/header/source, typed input, call
options, and request output. It rejects endpoint declarations or definitions
not classified by the manifest; `dcc_rest_cb`, `user_data`, raw endpoint JSON,
future/sync/async variants, and terminal overload suffixes; public App REST
mirrors are checked in Task 10.

An explicit `--progress-through 6` mode permits legacy violations only for the
exact Task 7–10 domains recorded in the manifest. Task 6 domains are always
strict. The command must fail if an allowed later-task set contains no
remaining violation, so stale progress allowances cannot survive a later
migration. With tests enabled, Python and all manifest/header/source inputs are
required; the gate must not disappear silently.

## Required RED tests

Before implementation, register a focused `dcc_rest_v2_endpoint_smoke` target
and capture RED for at least:

- strict/progress manifest audit detecting an existing create-message overload;
- delayed create-message submission returning in under 100 ms while the server
  is delayed at least 250 ms;
- caller mutation/free of message strings and multiple binary file buffers
  immediately after return, with the server receiving the original bytes;
- callback plus retained handle seeing one identical terminal result;
- local validation/admission rejection publishing no handle/callback/observer;
- literal paths and queries for message pagination, reactions, polls, and pins;
- every interaction response union tag and cross-tag rejection;
- original response and followup create/get/edit/delete;
- webhook execute JSON and multipart, zero/multiple files, optional query
  combinations, webhook token/no-token management, and message thread query;
- cancellation of a delayed multipart request and handle destruction;
- malformed file metadata, overflowed counts, invalid historical input prefix,
  unsupported version, and unknown presence bits.

Use the local HTTP server/interceptor; no live Discord calls. Preserve embedded
NUL file bytes. Snapshot input/output/callback counters on rejected calls.

## Migration and audit rules

Migrate all in-tree users of converted functions in the same implementation
commit: App context and transition App mirrors, interaction flow/server,
managed messages, examples, generators, tests, and package consumers. Adapters
may be private, but no removed converted declaration remains installed. Do not
block a runtime worker, wait for an endpoint request, or reintroduce
`dcc_rest_future_t`.

Audit route formatting and endpoint coverage against existing implementation
units and HTTP smoke fixtures before deleting overloads. Preserve all currently
covered Discord operations, including deprecated legacy pin routes; mark a
genuinely deprecated Discord endpoint in manifest metadata instead of treating
it as a DCC 1 alias.

## Verification and handoff

Run at minimum:

- progress endpoint audit plus its malformed-manifest self-tests;
- new endpoint smoke and the complete HTTP smoke suite;
- REST request/result, interaction-flow/server, App, managed-message, command
  registry, package-consumer, and transition surface tests;
- standalone changed public headers as strict C11 and C++17;
- i686 and x86_64 MinGW header plus changed production-source probes;
- focused ASan/UBSan and TSan for delayed submit/cancel/callback/ownership;
- full enabled CTest, MkDocs strict, install/package consumer, source/release
  audits, and `git diff --check`.

Commit implementation, manifest, audit, tests, and migrations with exact
message `feat: migrate message REST endpoints to requests`. Then write
`task-6-implementer-report.md` beside this brief with RED evidence, manifest
counts by task/domain, canonical endpoint list, removed symbols, ownership
matrix, exact commands/results, platform evidence, known baseline debts, and
the implementation hash. Commit the report separately and stop for review.
