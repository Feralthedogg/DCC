# Task 6 review-fixes report — endpoint contracts

## Outcome

All four Task 6 review findings are fixed in
`94c0f8e49cf0ae5022b773950974874caccecbee`
(`fix: enforce request endpoint contracts`; 50 files, +2509/-300). No Task 7
work was started and no push was performed.

The fixes keep the 41 migrated message, interaction, and webhook endpoints on
the Task 4 request/result lifecycle; make all 11 Task 6 versioned input record
families obey the documented historical-prefix and larger-record ABI; enforce
semantic rejection before endpoint-owned allocation or output mutation; and
publish one referenced Doxygen contract for the complete Task 6 public
surface.

## Review finding 1 — transition callers bypassed the Task 4 request path

The remaining App raw-message/raw-webhook shortcuts and managed-message
delete/create sequence no longer call the legacy async request machinery
directly. They use the private `dcc_endpoint_submit_legacy_raw()` adapter,
which creates Task 4 call options and submits through `dcc_rest_submit()`.

The adapter validates method, client, path, content-type/body pairing, and
query input locally; creates the callback bridge only after local input
validation; and releases it immediately if admission fails. The Task 4
descriptor owns/copies the path, content type, query, and exact body bytes
before the compatibility call returns. Accepted calls auto-release their
request handle and deliver the legacy callback exactly once from request
completion. Local rejection queues nothing and invokes no callback.

`tests/support/rest_v2_endpoint_transition_contracts.c` exercises all three
call paths with a blocked interceptor. It proves that:

- submission returns before the worker is released;
- caller-owned body/builder input can be mutated immediately after return;
- the worker receives the original copied method, path, query, and body;
- managed delete completes before create and both use the request worker;
- the callback runs exactly once and not on the submitting thread; and
- stopped-client, null-client, invalid-method, and invalid body/content-type
  admission failures remain callback-silent.

A targeted source audit also finds no `dcc_rest_request_*()` call in
`src/app/app_messages.c`, `src/app/app_rest_shortcuts_webhooks.c`, or
`src/managed_message/managed_message.c`.

## Review finding 2 — versioned inputs required the current full layout

One shared endpoint record reader now validates only the mandatory ABI prefix,
copies each field only when its byte range is covered, and treats uncovered
optional suffix fields as absent. It still rejects unsupported versions,
unknown presence bits, any present field whose bytes are not covered, partial
mandatory fields, invalid paired fields, and arithmetic overflow. A
version-1 record larger than the current layout is accepted and its unknown
tail is ignored.

The rule is applied consistently to all 11 Task 6 record families:

| Record family | Mandatory covered prefix | Optional suffix treatment |
| --- | --- | --- |
| `dcc_rest_message_payload_t` | header plus `message` | files absent unless both file fields are covered |
| `dcc_rest_message_list_query_t` | header through `present` | uncovered page fields absent |
| `dcc_rest_id_page_t` | header through `present` | uncovered page fields absent |
| `dcc_rest_pin_page_t` | header through `present` | uncovered query fields absent |
| `dcc_rest_reaction_query_t` | header through `present` | uncovered query fields absent |
| `dcc_rest_interaction_response_t` | header, presence mask, and response type | union data/files absent unless covered and allowed by the tag |
| `dcc_rest_webhook_builder_t` | header through `present` | uncovered builder fields absent |
| `dcc_rest_webhook_execute_t` | header, presence mask, and message member | files require a covered pair; other uncovered fields are absent |
| `dcc_rest_webhook_message_query_t` | header through `present` | uncovered query fields absent |
| `dcc_rest_webhook_message_edit_t` | header, presence mask, and payload | uncovered query/options fields absent |
| `dcc_rest_webhook_compat_payload_t` | size/version header | body is absent unless pointer and length are both covered |

The endpoint resource fixture covers every family with a covered historical
prefix, an oversized version-1 record, a present-but-uncovered field, and a
short mandatory header. Existing unknown-bit and future-version cases remain
covered. Both 64-bit and 32-bit MinGW compilation validates that the checks
derive coverage from each target ABI rather than assuming host offsets.

## Review finding 3 — semantic validation could occur after allocation

All 41 Task 6 wrappers were audited at their public boundary. Path scalars,
tokens, options, typed inputs, nested builders, pointer/count pairs, ranges,
and mutually exclusive fields are now rejected before body, query, or path
construction begins.

| Wrapper class | Count | Pre-allocation audit result |
| --- | ---: | --- |
| Scalar-only | 22 | client/options and every path/token scalar validated before formatting |
| Versioned-record | 18 | complete record, presence, endpoint semantics, nested builders, and file metadata validated before serialization or route/query allocation |
| Bulk array | 1 | count, pointer, every snowflake, and range checked before the JSON buffer |
| **Total** | **41** | **all Task 6 wrappers covered** |

The scalar-only set is add/crosspost/delete reaction operations; message
delete/fetch/end-poll and legacy/current pin writes; webhook get/delete and
channel/guild listing; and interaction followup/original-response get/delete.
The record set is create/edit message, typed message/reaction/pin/poll reads,
initial/original/followup interaction writes, webhook create/modify/execute,
Slack/GitHub compatibility execute, and webhook-message get/delete/edit. Bulk
message delete is the sole adjacent-array operation.

Interaction response preflight now validates the response tag, allowed union
member, files, and nested message/modal/autocomplete builder semantics without
serializing. Private allocation-free modal and autocomplete validators are
shared with their serializers, so malformed nested records cannot reach a
buffer allocation. Followup edit/get/delete and webhook-message fetch also
validate required tokens and IDs before route construction.

The internal test-only allocation failpoint is armed at the endpoint boundary
for representative path and nested-input regressions. Invalid followup and
webhook paths, PONG-with-files, malformed modal input, and malformed
autocomplete input all return `DCC_ERR_INVALID_ARG` with zero endpoint
allocation-probe calls, a null request output, no queue admission, and no
callback. Successful paths continue through normal allocation and submission.

## Review finding 4 — the public endpoint contract was implicit

`include/dcc/rest/request.h` now owns the Doxygen
`dcc_rest_endpoint_call_contract` page. It specifies borrowed input lifetime,
copy/serialization before return, null options, automatic release, retained
request ownership, exactly-one terminal result, callback lifetime, local
rejection silence, and the historical-prefix/larger-record rules.

Every one of the 41 canonical Task 6 endpoint declarations references that
page. The public definitions for all 11 versioned record families state their
mandatory prefix, presence/coverage behavior, larger-record behavior, and
borrowed lifetime. A declaration audit reports 41/41 endpoint references and
11/11 record-family contracts. Strict MkDocs generation remains clean.

## Verification

Native and focused regression evidence on the production commit:

- `cmake --build build-task6 -j8` — PASS.
- Full enabled suite — PASS, 181/181 in 52.07 seconds.
- `dcc_rest_v2_endpoint_smoke` repeated ten times — PASS, 10/10 in 9.03
  seconds.
- Endpoint, builder ABI, HTTP, App, and interaction-flow focused matrix —
  PASS, 5/5 in 2.77 seconds.
- `git diff --check` and pre-commit `git diff --cached --check` — PASS.

Manifest, API, and documentation gates:

- `python3 tools/audit_rest_v2_endpoints.py --self-test` — PASS; all 99
  malformed fixtures rejected.
- `python3 tools/audit_rest_v2_endpoints.py --progress-through 6` — PASS; 231
  endpoints with task counts 41/35/51/59/45 and 190 later-task endpoints
  explicitly deferred.
- `python3 tools/audit_v2_surface.py --source . --compiler /usr/bin/clang --cxx-compiler /usr/bin/clang++ --llam-include /Users/feral/Desktop/Programming/LLAM/include --transition`
  — PASS; 455 public headers compile as strict C11 and C++17.
- `python3 -m mkdocs build --strict` — PASS.
- The targeted legacy-bypass audit and the 41/41 declaration plus 11/11
  record-documentation audit — PASS.

Compiler, sanitizer, and package evidence:

- Every changed production C translation unit (20) passed Clang C11 with
  `-Wall -Wextra -Wpedantic -Werror`.
- Every changed production C translation unit (20) and changed public header
  (23) passed warning-as-error compilation with both x86_64 and i686 MinGW.
- Final ASan/UBSan endpoint and builder matrix — PASS, 2/2 in 1.06 seconds,
  with no diagnostic.
- Final TSan endpoint and builder matrix, each repeated three times — PASS,
  6/6 in 4.27 seconds, with no report.
- Installation to `/tmp/dcc-task6-review-install-93d9721-final` — PASS.
- The external package consumer configured against that install with the
  explicit local LLAM root/library, then built and ran successfully.

## Known baseline debts

- Tasks 7–10 still own 190 enumerated transition endpoints. The progress audit
  keeps each allowance explicit and rejects stale debt; this review did not
  migrate or alter those domains.
- The full HTTP event-wait path retains the Task 3/4 pre-existing TSan race
  documented in the earlier reports. The focused Task 6 endpoint/builder TSan
  matrix is clean, and this change does not widen that baseline.
- The optional local Discord documentation mirror was unavailable during the
  review. The Task 6 contract and manifest remained grounded in the checked
  design/brief and their official Discord documentation links.

The authoritative design, implementation plan, and ignored progress ledger
were not edited. No push was performed.
