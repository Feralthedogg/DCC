# Task 10 implementer report

## Status

Implementation completed on `codex/v2-api-impl`. The Task 10 base is
`63f7cdd09a16cda9aba66164b1538d2c0a47acbe` and the implementation commit is
`1f02076748a947f380841584cfb2714e3d1db455`.

## RED evidence

- `tests/rest_v2_task10_red_audit_contract.py` failed before migration because
  the final endpoint batch was not canonical.
- `tests/rest_v2_task10_red_compile_contract.py` failed before migration on the
  44 request signatures and 32 required removals.
- RED contract commit already present: `63f7cdd test: define final REST migration contract`.

## Final REST inventory

- Active endpoint arithmetic: 224 total; Task 6/7/8/9/10 = 41/35/47/57/44.
- Task 10 domains: applications 3, entitlements 6, gateway 2, group DMs 2,
  lobbies 13, role connections 5, SKUs 2, soundboard 2, users 4, voice states 5.
- Task 10 auth policies: BOT 29, BEARER 11, BOT_OR_BEARER 3, NONE 1.
- Audit-log reason support is denied for all 44 Task 10 endpoints.
- Removed: 28 Task 10 legacy names, the official-body free helper, two public
  DM composites, stale current-user DMs, the direct-message public header/two
  sources/private header, and obsolete Task 10 body-builder units.
- Strict endpoint audit is unconditional. `--progress-through` is rejected by
  argparse and all progress/debt branches and synthetic progress fixtures are
  gone.

## App compatibility freeze and composites

- Frozen mirror owners remain 51/72/45/28 = 196 exports.
- Sorted-name SHA-256 is
  `4b3246e7c89911bba7bf3256b1021294845aae343e8a6f32580fcd116e6a660f`.
- The nine context helpers now belong to `app_context_shortcuts.c` and submit
  canonical REST through `dcc_app_client`.
- No non-mirror source calls a frozen App REST mirror; strict audit checks the
  exact baseline, declarations, definitions, owners, and consumers.
- Send-with-thread is a nonblocking CREATE_MESSAGE -> PARSE_MESSAGE_ID ->
  CREATE_THREAD -> PARSE_THREAD_ID state machine.
- Managed-message publish exposes the aggregate result/stage model and follows
  LOAD -> optional DELETE -> CREATE -> SAVE -> DONE.
- Command-registry apply already deep-copies the executable plan before
  returning; READY sync releases source snapshot and plan immediately after
  successful admission.

## Verification evidence

- `python3 tools/audit_rest_v2_endpoints.py --source .`: PASS, strict 224/224.
- Endpoint audit self-test: PASS, 119 malformed fixtures rejected.
- Expected `--progress-through 10` invocation: PASS by failing as an
  unrecognized argument.
- Complete configured build: PASS.
- Focused matrix: 13/13 PASS (Task 9/10 audit and compile contracts, schema,
  request/result, endpoint smoke, App error lifetime, official audit/header
  smoke, HTTP smoke, and official-surface HTTP smoke).
- `git diff --check`: PASS.

Sanitizer, TSan, MinGW, installed-package, and full CTest execution remain
release-gate work; they must not be reported as passing until run.
