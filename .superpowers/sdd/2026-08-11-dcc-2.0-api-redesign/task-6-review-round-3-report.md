# Task 6 review round 3 — nested and multipart preflight

## Outcome

The round-3 fixes are implemented in
`fff99024467562188e2913188b3895cac769d199`
(`fix: preflight nested endpoint payloads`; 21 files, +1025/-33), based on
`295231bc6a8a4c3be94494ad1302986f82673d35`. No Task 7 work was started and
no push was performed.

Invalid structured message trees and impossible multipart bodies are now
rejected before endpoint admission or serializer-buffer allocation. The
generic multipart builder uses the same checked aggregate measurement as the
endpoint preflight. No public header, `DCC_API` declaration, or public record
layout was added or changed.

## Nested message validation

`dcc_message_builder_validate_for_json()` now performs the allocation-free
deep validation shared by the standalone message serializer and Task 6
endpoint preflight:

- structured embeds are capped at ten; the complete historical ABI stride is
  validated for every array element, every element must use the same stride,
  and each embed receives semantic validation before serialization;
- embed field counts are capped at 25 and their pointer/count spans use
  checked multiplication and address arithmetic;
- legacy message components require top-level action rows, at most five rows
  and five children per row, only message-valid child kinds, select isolation,
  and valid string/non-string select option counts;
- every legacy child and select-option pointer/count span is checked before
  traversal, nesting is bounded, and the complete message tree is capped;
- Components V2 arrays and nested arrays use checked spans and the existing
  message-context validator, so modal-only layouts are rejected from message
  payloads; and
- sticker, poll-answer, allowed-user, and allowed-role arrays receive checked
  multiplication/address validation before traversal.

Raw JSON escape hatches were not reinterpreted. Existing poll and allowed
mention semantics were left unchanged; this round only added their span
safety. The legacy validator likewise did not introduce new byte-length or
Unicode policy.

## Checked multipart aggregate

The internal `dcc_rest_multipart_measure()` routine validates the field/file
array spans and metadata, then checked-adds the exact encoded length of every
boundary, header, field name/value, filename, content type, data block, CRLF,
and closing boundary. It reserves space for the trailing buffer NUL and
returns `DCC_ERR_INVALID_ARG` on aggregate overflow without allocating.

`dcc_rest_build_multipart_body()` calls that routine before its first buffer
append and verifies that the built length equals the measured length. On
preflight failure it leaves `out_body == NULL` and `out_body_len == 0`.

Message, initial-interaction-response, and webhook-execute preflight share one
endpoint helper. For requests containing files, it measures the files together
with the mandatory minimum multipart field `payload_json={}`. This closes the
boundary case where the file-only body was representable but the mandatory
payload field made the final envelope overflow. Actual serialized JSON can be
larger than `{}`; allocation failure caused by that later, data-dependent size
remains a legitimate `DCC_ERR_NOMEM` result.

## Regression contracts and allocation evidence

`tests/support/rest_v2_endpoint_preflight_contracts.c` covers malformed legacy
roots and nested spans, malformed structured embeds, `SIZE_MAX` embed counts,
and a modal-only Components V2 tree used in message context. Each case checks
both an endpoint call and the standalone message serializer.

Multipart coverage includes a direct `SIZE_MAX` file, an aggregate overflow
introduced by a later file, and a one-file boundary fixture with
`data_len == SIZE_MAX - 200`: file-only measurement succeeds, while adding the
91-byte minimum `payload_json={}` part makes the complete envelope overflow.
The generic builder and representative create-message, initial-interaction,
and webhook-execute calls exercise that boundary.

The test-only thread-local probes are private internal declarations. They
count attempted growth in the message JSON and REST multipart buffers and use
the existing endpoint allocation probe. The representative three endpoint
families prove `DCC_ERR_INVALID_ARG`, null request output, zero endpoint/JSON/
multipart allocation attempts, no interceptor admission, no callback, and no
error-observer delivery. This is representative probe evidence, not a claim
that the probe directly executed all 41 Task 6 wrappers.

The HTTP builder fixture previously placed a string select directly at the
message root. It now wraps the select in a Discord-valid action row. Its local
test server also shuts down the listening socket before joining when local
submission fails, so a rejected fixture cannot strand the server thread in
`accept()`.

## TDD evidence

The nested-validator RED published an invalid request before the shared deep
validation existed:

```text
message rejects legacy component outside an action row rejection detail status=ok request=0x... allocations=0 capture=0/0 callback=0/0 observer=0/0
REST v2 endpoint smoke failed
```

The first multipart RED reached buffer growth and returned the wrong status:

```text
generic multipart overflow detail status=out of memory body=0x0 length=0
REST v2 endpoint smoke failed
```

After adding the minimum-envelope boundary fixture but before changing endpoint
preflight, the JSON allocation probe produced the round-3 RED:

```text
message rejects mandatory multipart envelope before JSON serialization rejection detail status=invalid argument request=0x0 endpoint=0 json=1 multipart=0 capture=0/0 callback=0/0 observer=0/0
REST v2 endpoint smoke failed
```

After the shared validators, aggregate measure, and endpoint helper were in
place, the same endpoint fixture passed with all three allocation counters at
zero.

## Verification

Native regression evidence on the production commit:

- `cmake --build build-task6 -j8` — PASS.
- Final full enabled suite — PASS, 181/181 in 118.05 seconds.
- Endpoint, builder ABI, HTTP, component-session, Components V2, component
  presets, interaction-flow, and interaction-flow-layout matrix — PASS, 8/8
  in 2.73 seconds.
- `dcc_rest_v2_endpoint_smoke` repeated ten times — PASS, 10/10 in 9.00
  seconds.
- `git diff --check`, `git diff --cached --check`, and project layout audit —
  PASS; the endpoint main fixture is 1960 lines after moving the new contracts
  to a 440-line support translation unit.

Compiler and sanitizer evidence:

- All 11 changed production C translation units passed Clang C11 with
  `-Wall -Wextra -Wpedantic -Werror`.
- The same 11 translation units passed warning-as-error syntax compilation
  with both x86_64 and i686 MinGW.
- ASan/UBSan endpoint plus builder matrix — PASS, 2/2 in 1.06 seconds with
  `detect_leaks=0`; this macOS ASan runtime aborts when leak detection is
  requested because that mode is unsupported on the platform.
- TSan endpoint plus builder matrix, each repeated three times — PASS, 6/6 in
  4.23 seconds with no report.

Manifest, API, documentation, and package gates:

- REST endpoint audit self-test — PASS; 99 malformed fixtures rejected.
- REST endpoint progress audit through Task 6 — PASS; 231 endpoints, Task 6
  count 41, and 190 later-task endpoints explicitly deferred.
- V2 transition surface audit — PASS; 455 headers checked.
- Project layout, source-package, REST-surface, and public-API audits — PASS;
  the public-API audit checked 1789 declarations and 3775 library symbols.
- `python3 -m mkdocs build --strict` — PASS.
- Fresh installation to `/tmp/dcc-task6-round3-install.oOQqQz` — PASS.
- The external package consumer configured against that install using the
  explicit local LLAM root/library, then built and ran successfully.

## Separate timing observation

The first full suite completed 180/181 in 119.50 seconds. The unrelated
`dcc_hot_reload_isolated_last_good_smoke` timed out after receiving five
expected HTTP requests but before its fallback counters advanced. Three
immediate isolated reruns produced FAIL in 5.74 seconds, FAIL in 5.71 seconds,
and PASS in 2.62 seconds. No hot-reload runtime code was changed. The required
fresh final full-suite rerun subsequently passed 181/181, including that test
in 2.64 seconds.

## Known baseline debts

- Tasks 7–10 still own 190 enumerated transition endpoints.
- The broader HTTP event-wait path retains the pre-existing Task 3/4 TSan race
  documented in earlier reports; the focused Task 6 TSan matrix is clean.
- The optional local Discord documentation mirror was unavailable. The checked
  design, Task 6 brief, manifest, and existing official links remained the
  contract sources.
- The hot-reload last-good timing behavior above remains an unrelated flaky
  baseline observation.

The authoritative design, implementation plan, and ignored progress ledger
were not edited. No push was performed.
