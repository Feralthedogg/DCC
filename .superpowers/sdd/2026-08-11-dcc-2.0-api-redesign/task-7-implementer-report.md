# Task 7 implementer report — request-based channels, threads, and invites

## Outcome

Implemented in `39e59bf910a5449f7d9db1f1b551b5aabf653146`
(`feat: migrate channel REST endpoints to requests`; 160 files,
+6203/-2739). No push was performed.

All 35 Task 7 operations now expose one typed, non-blocking submission
function ending in `const dcc_rest_call_options_t *options` and
`dcc_rest_request_t **out_request`. The split is 12 channel, 13 thread, and 10
invite operations. The 18 reviewed legacy REST overloads and aliases are gone
from installed headers and external definitions. In-tree App adapters, sugar,
HTTP fixtures, examples, and package consumers use the canonical operations.

The migration also closes the typed message-reference and attachment metadata
holes, extends the historical call-options prefix with audit/auth/sensitivity
policy, separates nonsecret operation identity from owned wire paths, and
propagates sensitive request/result ownership through submit, retry,
cancellation, observers, clones, and teardown.

## TDD / RED evidence

The Task 7 contract was introduced before implementation in `c024123` and
tightened in `7d6ab4d`. The locked Tasks 7–15 design is recorded by `89f59e2`.

- The endpoint manifest and declaration audit rejected every old callback,
  raw-query, raw-JSON, `_params`, `_page`, and thread-alias shape.
- Dedicated Task 7 endpoint and builder fixtures initially failed to compile
  against the old records and legacy signatures.
- Runtime RED cases covered local rejection silence, historical/partial record
  boundaries, App adapter routing, JSON/multipart ownership, exact embedded
  NUL spans, route/query literals, and caller mutation immediately after
  return.
- A final forum regression proved that a noncanonical multipart field such as
  `upload` was accepted. Production validation now requires the exact
  `files[n]` sequence before allocation/admission, and the test is green.
- The standalone installed-package consumer exposed stale thread-member,
  channel, invite, DM, and official-sugar call shapes. They were migrated to
  the final typed/request contract and the installed consumer now builds and
  runs.

## Manifest inventory

The checked active manifest contains exactly 224 endpoints.

| Migration task | Count |
| --- | ---: |
| 6 | 41 |
| 7 | 35 |
| 8 | 47 |
| 9 | 57 |
| 10 | 44 |

Task 7 contains exactly three multipart operations and 12 audit-reason
operations. Progress through Task 7 reports 76 migrated and 148 explicitly
deferred Task 8–10 endpoints. The seven unavailable candidates remain only in
the removal ledger.

## Task 7 canonical endpoints

Channels (12):

- `dcc_rest_create_dm_channel`
- `dcc_rest_create_guild_channel`
- `dcc_rest_delete_channel`
- `dcc_rest_delete_channel_permission`
- `dcc_rest_follow_news_channel`
- `dcc_rest_get_channel`
- `dcc_rest_get_guild_channels`
- `dcc_rest_modify_channel`
- `dcc_rest_modify_channel_permission`
- `dcc_rest_modify_guild_channel_positions`
- `dcc_rest_set_channel_voice_status`
- `dcc_rest_trigger_channel_typing`

Threads (13):

- `dcc_rest_add_thread_member`
- `dcc_rest_create_forum_thread`
- `dcc_rest_create_thread`
- `dcc_rest_create_thread_from_message`
- `dcc_rest_get_active_threads`
- `dcc_rest_get_joined_private_archived_threads`
- `dcc_rest_get_private_archived_threads`
- `dcc_rest_get_public_archived_threads`
- `dcc_rest_get_thread_member`
- `dcc_rest_get_thread_members`
- `dcc_rest_join_thread`
- `dcc_rest_leave_thread`
- `dcc_rest_remove_thread_member`

Invites (10):

- `dcc_rest_create_channel_invite`
- `dcc_rest_create_lobby_channel_invite_for_self`
- `dcc_rest_create_lobby_channel_invite_for_user`
- `dcc_rest_delete_invite`
- `dcc_rest_get_channel_invites`
- `dcc_rest_get_guild_invites`
- `dcc_rest_get_invite`
- `dcc_rest_get_invite_target_users`
- `dcc_rest_get_invite_target_users_job_status`
- `dcc_rest_put_invite_target_users`

## Removed public REST symbols

The following 18 reviewed symbols no longer compile or link as public REST
operations:

- `dcc_rest_add_thread_member_params`
- `dcc_rest_create_channel_invite_params`
- `dcc_rest_create_dm_channel_params`
- `dcc_rest_create_forum_thread_params`
- `dcc_rest_create_guild_channel_params`
- `dcc_rest_create_thread_params`
- `dcc_rest_create_thread_from_message_params`
- `dcc_rest_get_thread`
- `dcc_rest_get_invite_full`
- `dcc_rest_get_joined_private_archived_threads_page`
- `dcc_rest_get_private_archived_threads_page`
- `dcc_rest_get_public_archived_threads_page`
- `dcc_rest_modify_channel_params`
- `dcc_rest_modify_thread`
- `dcc_rest_modify_thread_params`
- `dcc_rest_modify_channel_permission_params`
- `dcc_rest_modify_guild_channel_positions_params`
- `dcc_rest_set_channel_voice_status_params`

## Input, validation, and ownership matrix

| Area | Public input and validation | Ownership after submission returns |
| --- | --- | --- |
| Channels | Versioned tagged guild/group-DM/thread payloads; presence/null masks; versioned overwrite, forum-tag, position, voice-status, follow, and DM records; type-specific ranges and field legality | Path scalars and the complete serialized JSON body are endpoint-owned |
| Threads | Versioned create/archive/member query records; operation-specific legal fields; timestamp vs snowflake pagination; forum message restrictions | Query/path/body storage is owned; forum payload and exact file metadata/bytes are copied |
| Invites | Presence-gated body/query/upload records; target relationships; role arrays; optional/required CSV contracts | Invite code, query, JSON/multipart metadata, filename, and exact CSV bytes are owned |
| Message records | Versioned typed reference and attachment records; unique IDs, UTF-8, base64 waveform, voice pairing, forward restrictions, extended-array stride | Builder records are borrowed only until JSON/body serialization; endpoint output owns the serialized form |
| Call options | Historical prefix plus size-gated audit reason, auth mode/token, and sensitivity flags; every partially covered field is rejected | Callback/user data remain borrowed through callback; audit/auth/header state is normalized and copied |
| Request/result | Canonical operation is public identity; wire path is private; sensitive request/result policy survives retry, clone, cancellation, and teardown | Owned sensitive buffers are securely wiped before release; observers receive redacted sensitive results |
| Local failure | Unsupported versions/bits, partial fields, invalid strides/counts/ranges, allocation/runtime/admission failure | `*out_request` is null, nothing is queued, and no callback or observer runs |

Larger layouts and covered historical prefixes are accepted. Versioned arrays
use their validated declared stride. Nullable channel fields distinguish
omission, explicit null, and actual zero/false. All structured validation and
multipart aggregate measurement completes before the first body allocation.

## Authentication, audit, and sensitive-data policy

- General Task 7 operations are Bot-only; Get Invite defaults to no auth and
  permits None/Bot; self-lobby invite is Bearer-only; user-lobby invite is
  Bot-only.
- Exactly 12 declared Task 7 operations accept a canonical UTF-8 audit reason
  and percent-encode it once. Every other Task 7 operation rejects it locally.
- Absolute raw URLs receive no DCC credential and reject Discord-specific
  auth/audit overrides. Relative raw submissions retain explicit
  Default/None/Bot/Bearer behavior.
- Typed terminal/error identity is the canonical operation name. Raw submits
  use `dcc_rest_submit`; observer-visible data never derives from a secret
  wire path.
- The exact 17 token-bearing interaction/webhook paths and 20 forced-sensitive
  REST results are audited. Route keys use the required domain-separated
  SHA-256 fingerprints and do not retain plaintext tokens or raw paths.

## App and consumer migration

The transition App layer constructs call options from its callback/user data
and submits through `dcc_app_client(app)` to canonical request endpoints.
Channel create/modify, permission, positions, invite, DM, thread create,
thread modify/archive, and composed send-with-thread paths no longer call the
legacy future family or wait. The send-with-thread bridge copies only the
fields legal for Start Thread from Message.

Sugar and package-consumer declarations use path IDs as scalar arguments and
version/presence-gated records for bodies and queries. The installed consumer
also verifies the canonical thread-member function pointer, invite upload
record, request-based official-surface macros, `dcc::dcc`, and the installed
hot-reload module helper.

## Verification commands and results

Native build, tests, docs, and packaging:

- `cmake --build build-v2-api -j 8` — PASS, all enabled targets built.
- `ctest --test-dir build-v2-api --output-on-failure` — PASS, 181/181 enabled
  tests in 116.64 seconds; the 24 bundled LLAM tests in this configuration are
  disabled.
- `python3 -m mkdocs build --strict` — PASS.
- `cmake --install build-v2-api --prefix build-task7-install-final` — PASS.
- Standalone configure/build/run of `tests/package_consumer` against that
  install with local LLAM root/library — PASS, including the module target.
- `git diff --check` and `git diff --cached --check` — PASS.

Manifest and surface gates:

- `python3 tools/audit_rest_v2_endpoints.py --self-test` — PASS, 123 malformed
  manifest/state fixtures rejected.
- `python3 tools/audit_rest_v2_endpoints.py --progress-through 7` — PASS, 224
  endpoints, task counts 41/35/47/57/44, 148 deferred.
- REST field, request assertion, REST surface, and official-surface audits —
  PASS.
- Public API audit — PASS, 1,764 declarations and 3,766 library symbols.
- Strict public-header transition audit — PASS, 455 headers as C11/C++17.
- Project layout/source package/deploy/release/SBOM/workflow-pin audits — PASS
  through the complete CTest run.

Sanitizers on the final source:

- ASan/UBSan request, endpoint, and builder matrix with
  `--repeat until-fail:3` — PASS, all nine executions, no diagnostic.
- TSan request and endpoint matrix with `--repeat until-fail:3` — PASS, all
  six executions, no report.

Cross-platform compile evidence:

- Strict standalone C11/C++17 probes passed for all 18 changed aggregate
  headers.
- Both i686 and x86_64 MinGW header probes passed for those 18 headers.
- Both i686 and x86_64 MinGW production translation-unit probes passed for all
  64 changed `.c` units with the generated DCC and LLAM include trees.

## Known transition debt and boundaries

- The progress audit intentionally permits only the 148 enumerated Task 8–10
  endpoint migrations. App/Bot removal and complete public documentation remain
  later design tasks; Task 7 introduces no new allowance for its domains.
- The 24 LLAM tests shown after CTest entry 181 are disabled by the selected
  bundled-LLAM configuration; every enabled DCC test passed.
- MkDocs strict emits informational Material/MkDocs notices and lists the
  intentionally unlisted superpowers design documents; neither is a build
  error.
- The original checkout was not modified, the pre-existing
  `tests/support/hot_reload_isolated_last_good_retry.c` working-tree change was
  not staged, and no push, tag, or release was performed.

No Task 7 production or test failure remains.
