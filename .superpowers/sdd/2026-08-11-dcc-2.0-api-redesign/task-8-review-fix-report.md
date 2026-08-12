# Task 8 review fix report

## Finding

The Add Guild Member request correctly marked its final JSON body sensitive
and securely freed both the temporary completed serializer buffer and the
async request copy. A serializer-capacity failure after the OAuth token had
already been copied, however, used the generic buffer deinitializer and could
free the partial token-bearing allocation without securely zeroing it.

## Fix

The REST buffer test seam can now deterministically fail a selected capacity
growth. The Add Guild Member builder uses the optimization-resistant sensitive
free primitive for its complete current allocation capacity on every partial
serialization or closing-brace failure.

The focused regression uses a 1,536-byte token, permits the initial 1,024-byte
allocation, fails the next growth, and proves the call returns
`DCC_ERR_NOMEM`, publishes no request, and records an additional secure wipe
covering the entire 1,024-byte partial allocation.

## Verification

- RED link failure proved the new failure seam did not exist.
- RED runtime evidence before the fix was
  `submit=out of memory ... wipes=1/1 bytes=1536/1536`, proving no additional
  wipe occurred for the partial serializer buffer.
- Focused `dcc_rest_v2_endpoint_smoke` — PASS.
- Full native CTest — PASS, 181/181 in 39.47 seconds.
- `python3 tools/audit_rest_v2_endpoints.py --source . --progress-through 8`
  — PASS, 224 endpoints with task counts 41/35/47/57/44 and 101 deferred.
- `git diff --check` — PASS.

The pre-existing user modification to
`tests/support/hot_reload_isolated_last_good_retry.c` remains unstaged.
