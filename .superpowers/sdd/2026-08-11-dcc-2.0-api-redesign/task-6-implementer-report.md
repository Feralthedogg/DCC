# Task 6 implementer report — endpoint manifest and request-based messages

## Outcome

Implemented in `772b4bfa149518ebc8c029f1c814f0c1c84688ce`
(`feat: migrate message REST endpoints to requests`; 123 files,
+5821/-5372), on top of the contract commit
`c6dcbc3da9a8d0e926ac7192a17eafce0347cd0d`. No push was performed.

All 41 Task 6 endpoints now expose exactly one typed, non-blocking submission
function ending in `const dcc_rest_call_options_t *options` and
`dcc_rest_request_t **out_request`. Messages (21), interactions (8), and
webhooks (12) use the Task 4 request/result lifecycle. Their 66 public legacy
overloads were removed, and every in-tree App, flow, managed-message, example,
test, and package consumer was migrated in the same production commit.

The common implementation path validates the complete versioned input before
admission, formats the route/query, finishes JSON or multipart serialization,
and then calls the Task 4 submit path. It never waits. The eight multipart
operations copy metadata and exact file bytes, including embedded NULs, before
returning. The hot-reload worker now starts and settles its client around
module dispatch, and hot-reload modules resolve the host's DCC ABI instead of
creating a second static runtime.

## TDD / RED evidence

The checked contract and RED fixture were introduced by `c6dcbc3` before the
implementation:

- `tools/audit_rest_v2_endpoints.py --progress-through 6` rejected the old
  create-message declaration/overload group and the remaining Task 6 legacy
  declarations.
- `dcc_rest_v2_endpoint_smoke` could not compile against the old raw JSON,
  callback/user-data, builder, multipart, page, options, thread, and
  from-interaction signatures. Its delayed-submit and ownership assertions
  therefore remained RED until the typed request-returning declarations and
  definitions existed.
- Once the API compiled, the hot-reload canonical endpoint probe returned
  `DCC_ERR_RUNTIME`/`EINVAL`; this exposed a duplicate static DCC runtime in
  the module. Exporting the worker/host ABI and resolving it from modules made
  the native and isolated probes green.
- The broad builder/autocomplete regression initially exposed a fixture with
  no running client lifecycle. Starting and stopping the fixture client fixed
  the test setup without weakening endpoint admission semantics.

The final endpoint fixture covers delayed return, caller mutation/free after
return, callback plus retained-handle identity, local rejection silence,
cancellation, literal route/query output, version/presence/size boundaries,
all interaction tags, all eight multipart operations, file-only `{}` payloads,
multiple binary files, CR/LF metadata rejection, and webhook authentication/
query combinations.

## Manifest inventory

The manifest contains exactly 231 canonical endpoints.

| Migration task | Count |
| --- | ---: |
| 6 | 41 |
| 7 | 35 |
| 8 | 51 |
| 9 | 59 |
| 10 | 45 |

Domain counts are:

| Task | Domain | Count |
| ---: | --- | ---: |
| 6 | messages | 21 |
| 6 | interactions | 8 |
| 6 | webhooks | 12 |
| 7 | channels | 12 |
| 7 | threads | 13 |
| 7 | invites | 10 |
| 8 | guilds | 32 |
| 8 | guild_members | 12 |
| 8 | roles | 7 |
| 9 | application_commands | 21 |
| 9 | auto_moderation | 5 |
| 9 | emojis_stickers | 13 |
| 9 | message_search | 1 |
| 9 | onboarding | 2 |
| 9 | scheduled_events | 6 |
| 9 | stage_instances | 4 |
| 9 | templates | 7 |
| 10 | applications | 3 |
| 10 | direct_messages | 1 |
| 10 | entitlements | 6 |
| 10 | gateway | 2 |
| 10 | group_dms | 2 |
| 10 | lobbies | 13 |
| 10 | role_connections | 5 |
| 10 | skus | 2 |
| 10 | soundboard | 2 |
| 10 | users | 4 |
| 10 | voice_states | 5 |

The audit also classifies the reviewed baseline exactly once: 60 immutable
generic operations, one Task 10 transition helper, 405 current endpoint
symbols, seven transition composites, and two planned canonical endpoints.
The Task 7–10 progress allowance reports 190 deferred endpoints and fails if a
later `(task, domain)` allowance becomes stale.

## Task 6 canonical endpoints

Messages:

- `dcc_rest_add_message_reaction` — `PUT message_reaction_self`
- `dcc_rest_bulk_delete_messages` — `POST channel_message_bulk_delete`
- `dcc_rest_create_message` — `POST channel_messages`
- `dcc_rest_crosspost_message` — `POST channel_message_crosspost`
- `dcc_rest_delete_all_message_reactions` — `DELETE channel_message_reactions`
- `dcc_rest_delete_all_message_reactions_for_emoji` — `DELETE channel_message_reactions_emoji`
- `dcc_rest_delete_message` — `DELETE channel_message`
- `dcc_rest_delete_own_message_reaction` — `DELETE message_reaction_self`
- `dcc_rest_delete_user_message_reaction` — `DELETE message_reaction_user`
- `dcc_rest_edit_message` — `PATCH channel_message`
- `dcc_rest_end_poll` — `POST channel_message_poll_expire`
- `dcc_rest_get_channel_messages` — `GET channel_messages`
- `dcc_rest_get_channel_pins` — `GET channel_message_pins`
- `dcc_rest_get_legacy_channel_pins` — `GET legacy_channel_pins`
- `dcc_rest_get_message` — `GET channel_message`
- `dcc_rest_get_message_reactions` — `GET channel_message_reactions_emoji`
- `dcc_rest_get_poll_answer_voters` — `GET channel_message_poll_answer`
- `dcc_rest_legacy_pin_message` — `PUT legacy_channel_pin`
- `dcc_rest_legacy_unpin_message` — `DELETE legacy_channel_pin`
- `dcc_rest_pin_message` — `PUT channel_message_pin`
- `dcc_rest_unpin_message` — `DELETE channel_message_pin`

Interactions:

- `dcc_rest_interaction_followup_create` — `POST interaction_followups`
- `dcc_rest_interaction_followup_delete` — `DELETE interaction_followup_message`
- `dcc_rest_interaction_followup_edit` — `PATCH interaction_followup_message`
- `dcc_rest_interaction_followup_get` — `GET interaction_followup_message`
- `dcc_rest_interaction_original_response_delete` — `DELETE interaction_original_response`
- `dcc_rest_interaction_original_response_edit` — `PATCH interaction_original_response`
- `dcc_rest_interaction_original_response_get` — `GET interaction_original_response`
- `dcc_rest_interaction_response_create` — `POST interaction_callback`

Webhooks:

- `dcc_rest_create_webhook` — `POST channel_webhooks`
- `dcc_rest_delete_webhook` — `DELETE webhook_optional_token`
- `dcc_rest_delete_webhook_message` — `DELETE webhook_message`
- `dcc_rest_execute_webhook` — `POST webhook_execute`
- `dcc_rest_execute_webhook_github` — `POST webhook_execute_github`
- `dcc_rest_execute_webhook_slack` — `POST webhook_execute_slack`
- `dcc_rest_get_channel_webhooks` — `GET channel_webhooks`
- `dcc_rest_get_guild_webhooks` — `GET guild_webhooks`
- `dcc_rest_get_webhook` — `GET webhook_optional_token`
- `dcc_rest_get_webhook_message` — `GET webhook_message`
- `dcc_rest_modify_webhook` — `PATCH webhook_optional_token`
- `dcc_rest_modify_webhook_message` — `PATCH webhook_message`

The exact multipart set is create/edit message, initial interaction response,
original response edit, followup create/edit, webhook execute, and webhook
message edit.

## Removed public endpoint symbols

The following 66 Task 6 legacy symbols were collapsed into the canonical
operations above and removed from installed headers and external definitions:

- `dcc_rest_add_message_reaction_params`
- `dcc_rest_create_message_builder`
- `dcc_rest_create_message_builder_multipart`
- `dcc_rest_create_message_multipart`
- `dcc_rest_create_webhook_params`
- `dcc_rest_delete_webhook_message_thread`
- `dcc_rest_delete_webhook_with_token`
- `dcc_rest_edit_message_builder`
- `dcc_rest_edit_message_builder_multipart`
- `dcc_rest_edit_message_flags`
- `dcc_rest_edit_message_flags_params`
- `dcc_rest_edit_message_multipart`
- `dcc_rest_execute_webhook_multipart`
- `dcc_rest_execute_webhook_multipart_options`
- `dcc_rest_execute_webhook_options`
- `dcc_rest_get_channel_messages_page`
- `dcc_rest_get_channel_pins_page`
- `dcc_rest_get_message_reactions_page`
- `dcc_rest_get_poll_answer_voters_page`
- `dcc_rest_get_webhook_message_thread`
- `dcc_rest_get_webhook_with_token`
- `dcc_rest_interaction_followup_create_builder`
- `dcc_rest_interaction_followup_create_from_interaction`
- `dcc_rest_interaction_followup_create_from_interaction_builder`
- `dcc_rest_interaction_followup_create_multipart`
- `dcc_rest_interaction_followup_create_multipart_from_interaction`
- `dcc_rest_interaction_followup_delete_from_interaction`
- `dcc_rest_interaction_followup_edit_builder`
- `dcc_rest_interaction_followup_edit_from_interaction`
- `dcc_rest_interaction_followup_edit_from_interaction_builder`
- `dcc_rest_interaction_followup_edit_multipart`
- `dcc_rest_interaction_followup_edit_multipart_from_interaction`
- `dcc_rest_interaction_followup_get_from_interaction`
- `dcc_rest_interaction_original_response_delete_from_interaction`
- `dcc_rest_interaction_original_response_edit_builder`
- `dcc_rest_interaction_original_response_edit_from_interaction`
- `dcc_rest_interaction_original_response_edit_from_interaction_builder`
- `dcc_rest_interaction_original_response_edit_multipart`
- `dcc_rest_interaction_original_response_edit_multipart_from_interaction`
- `dcc_rest_interaction_original_response_get_from_interaction`
- `dcc_rest_interaction_response_create_autocomplete`
- `dcc_rest_interaction_response_create_autocomplete_from_interaction`
- `dcc_rest_interaction_response_create_deferred_message`
- `dcc_rest_interaction_response_create_deferred_message_from_interaction`
- `dcc_rest_interaction_response_create_deferred_update`
- `dcc_rest_interaction_response_create_deferred_update_from_interaction`
- `dcc_rest_interaction_response_create_from_interaction`
- `dcc_rest_interaction_response_create_from_interaction_message_builder`
- `dcc_rest_interaction_response_create_message_builder`
- `dcc_rest_interaction_response_create_modal`
- `dcc_rest_interaction_response_create_modal_from_interaction`
- `dcc_rest_interaction_response_create_options`
- `dcc_rest_interaction_response_create_pong`
- `dcc_rest_interaction_response_create_pong_from_interaction`
- `dcc_rest_interaction_response_create_premium_required`
- `dcc_rest_interaction_response_create_premium_required_from_interaction`
- `dcc_rest_interaction_response_create_type`
- `dcc_rest_interaction_response_create_type_from_interaction`
- `dcc_rest_modify_webhook_message_builder`
- `dcc_rest_modify_webhook_message_builder_thread`
- `dcc_rest_modify_webhook_message_multipart`
- `dcc_rest_modify_webhook_message_multipart_thread`
- `dcc_rest_modify_webhook_message_thread`
- `dcc_rest_modify_webhook_params`
- `dcc_rest_modify_webhook_with_token`
- `dcc_rest_modify_webhook_with_token_params`

The obsolete source-only overload translation units and the private webhook
body compatibility helper were deleted with their CMake entries.

## Ownership and terminal behavior

| Value | Caller obligation | DCC behavior after return |
| --- | --- | --- |
| Scalar IDs, tokens, optional query strings | Valid for the endpoint call | Path/query bytes are validated and copied/formatted before return |
| Versioned query/payload/response/webhook records | Record and covered fields valid for the call | Size/version/presence and endpoint-specific fields are validated before admission; no record pointer is retained |
| Message/modal/autocomplete builders and nested arrays/strings | Borrowed for the call | Complete JSON is serialized before admission; worker code never reserializes caller objects |
| Multipart file array, metadata, and byte buffers | Borrowed for the call; pointer/count pairs valid | Metadata is injection-checked and the complete multipart body copies exact bytes before admission |
| `dcc_rest_call_options_t` | Options record valid for the call | Null selects `DCC_REST_CALL_OPTIONS_INIT`; callback identity and `user_data` remain borrowed until the terminal callback |
| `dcc_rest_request_t **out_request` | If non-null, caller eventually destroys the returned handle | `DCC_OK` publishes one retained handle; null requests auto-release after the terminal result |
| Terminal result | Borrowed while observed through callback/request APIs | Callback and retained handle observe the same single terminal result; clone explicitly to retain result data |
| Local validation/admission rejection | No cleanup beyond caller-owned inputs | `*out_request` is null, no callback or observer runs, and no request is queued |
| App/flow compatibility bridge state | No public ownership change | Private heap bridge survives until terminal callback and is freed there, or is freed immediately on local rejection |

Null messages are accepted only with at least one valid file and serialize an
empty payload object. All payload/query prefixes accept covered historical
sizes and larger records, while unsupported versions, uncovered present bits,
invalid pointer/count pairs, invalid ranges, and arithmetic overflow fail
before output publication or queue admission.

## Verification commands and results

Manifest and surface gates:

- `python3 tools/audit_rest_v2_endpoints.py --self-test` — PASS, 99 malformed
  manifest fixtures rejected.
- `python3 tools/audit_rest_v2_endpoints.py --progress-through 6` — PASS, 231
  endpoints; task counts 41/35/51/59/45; 190 later-task endpoints deferred.
- `python3 tools/audit_v2_surface.py --source . --compiler /usr/bin/clang --cxx-compiler /usr/bin/clang++ --llam-include /Users/feral/Desktop/Programming/LLAM/include --transition`
  — PASS, 455 public headers compile as strict C11 and C++17.
- `python3 -m mkdocs build --strict` — PASS.
- `git diff --check` and `git diff --cached --check` — PASS.

Native build and tests:

- `cmake --build build-task6 -j2` — PASS, all enabled targets built.
- `ctest --test-dir build-task6 --output-on-failure` — PASS, 181/181 in
  84.12 seconds on the final production tree.
- Focused endpoint/HTTP/hot-reload/manifest/request/surface/public API matrix —
  PASS, 8/8 in 24.03 seconds.
- `cmake --install build-task6 --prefix build-task6/install-task6` — PASS.
- `cmake -S tests/package_consumer -B build-task6-package-consumer -DCMAKE_PREFIX_PATH=/Users/feral/Desktop/Programming/DCC/.worktrees/v2-api-impl/build-task6/install-task6 -DDCC_ALLOW_LOCAL_LLAM_FALLBACK=ON`
  followed by `cmake --build build-task6-package-consumer -j2` and
  `./build-task6-package-consumer/dcc_package_consumer` — configure/build/run
  PASS, including the installed `dcc::hot_reload_worker` module contract.

Sanitizers on the final endpoint source:

- `cmake -E env ASAN_OPTIONS=detect_leaks=0:abort_on_error=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 ctest --test-dir build-v2-api-asan --output-on-failure -R '^dcc_rest_v2_endpoint_smoke$'`
  — PASS, 1/1 in 0.95 seconds, no ASan/UBSan diagnostic.
- `cmake -E env TSAN_OPTIONS=halt_on_error=1:exitcode=66:history_size=7 ctest --test-dir build-v2-api-tsan --output-on-failure --repeat until-fail:3 -R '^dcc_rest_v2_endpoint_smoke$'`
  — PASS, 3 consecutive executions in 3.34 seconds, no TSan report. An
  earlier five-repeat request+endpoint matrix also passed all 10 executions.
- Request, endpoint, builder, HTTP, hot-reload last-good, and dispatch-focused
  ASan/UBSan runs passed; a concurrently launched HTTP sanitizer process once
  remained settling after the other five completed, then passed stand-alone
  1/1 in 2.01 seconds without a sanitizer diagnostic.

Windows evidence:

- `x86_64-w64-mingw32-gcc -std=c11 -Wall -Wextra -Werror -Iinclude -Ibuild-task6/generated/include -Isrc -I/Users/feral/Desktop/Programming/LLAM/include -c src/rest/rest_endpoint_v2.c -o /tmp/dcc-task6-rest-endpoint-v2-x64.o`
  — PASS.
- The identical command with `i686-w64-mingw32-gcc` and
  `/tmp/dcc-task6-rest-endpoint-v2-i686.o` — PASS.
- Strict standalone probes for `dcc/rest/official_surface.h`,
  `dcc/sugar/official_surface.h`, and `dcc/app/legacy.h`, plus the six changed
  production units for endpoint submission and hot-reload worker lifecycle,
  passed with both x86_64 and i686 MinGW.
- A complete x86_64 cross build produced `libdcc.a` with bundled LLAM. PE
  `objdump`/`nm` then confirmed that `dcc_hot_reload_worker.exe` exports
  `dcc_rest_create_message` and `dcc_rest_interaction_response_create`, its
  import library contains the corresponding `__imp_*` records, and
  `dcc_hot_reload_isolated_module.dll` imports both from the worker executable.

## Known baseline debts

- Task 7–10 endpoint declarations intentionally remain transition debt; the
  progress audit enumerates all 190 and will reject stale allowances as later
  tasks migrate them. Transition surface mode also reports its previously
  recorded App/Bot and documentation/removal debt; Tasks 10/14 own those
  removals, and no Task 6 declaration is allowed by that debt.
- Task 3/4 documented an out-of-scope TSan event-wait race reached by the full
  HTTP sanitizer suite. Task 6's delayed submit/cancel/callback/ownership TSan
  matrix is clean.

## Validation boundaries

- The optional local Discord documentation mirror `/tmp/discord-api-docs` was
  unavailable, so the conditional external-docs cross-check was not enabled.
  The checked manifest route catalog, literal local-server assertions, and
  source-token audits all passed.
- The cross environment initially selected Homebrew's host macOS OpenSSL
  archives for a Windows worker link. The real DCC+LLAM Windows archive built;
  link-only OpenSSL symbol stubs confined to the ignored cross-build directory
  were used solely to inspect the PE host export/import/module ABI. No stub or
  cross-build artifact is present in the production commit.
- MkDocs strict emits only informational notices about the Material version and
  intentionally unlisted superpowers documents.

No Task 6 failure or uncommitted production change remains.
