# Task 8 implementer report

## Result

Task 8 is implemented on `codex/v2-api-impl` by commit
`1931ed899427da8894c59a1ecdf548cb5de836c9` with the required subject:

`feat: migrate guild REST endpoints to requests`

The change migrates all 47 retained guild, guild-member, and role operations
to the request-handle API, removes four stale canonical operations and the 32
reviewed legacy entry points, and advances the registered transition gate
through Task 8. The manifest is now 224 active endpoints with the exact task
split 41/35/47/57/44. Tasks 6–8 account for 123 strict endpoints and Tasks
9–10 retain 101 reviewed transition endpoints, so the migration is 123/224
(54.9 percent) complete.

No push, tag, release, or merge was performed.

## RED evidence

The reviewed RED contract is commit `2ca19e5` (`test: define guild REST
migration contract`). It established all of the following before production
changes:

- Task 7's registered progress audit remained green at 224 active endpoints.
- `--progress-through 8` rejected the old implementations, including missing
  typed inputs, auth/audit contracts, and the unapplied removal task.
- The out-of-band canonical probe contained the exact 47 function signatures
  and failed because the new record types/signatures were unavailable.
- The companion legacy probe compiled with the reviewed legacy declarations,
  proving that the removal work had not already happened.
- The manifest fixture moved Task 8 to 47 retained operations while keeping
  the old implementation deliberately inconsistent with it.

Focused TDD during implementation also exposed three concrete defects before
their fixes:

- Optional-only null bodies initially returned `invalid argument` instead of
  serializing `{}`.
- Add Guild Member's forced-admission-failure test initially reported
  `wipes=1/1 bytes=29/29 expected=49`, proving that the temporary OAuth JSON
  allocation was freed without the additional secure wipe.
- The calendar test initially admitted `2026-02-29T00:00:00Z` in a non-leap
  year. The shared parser now validates month lengths and leap years, and the
  guild-name tests reject both ASCII and Unicode edge whitespace.

The temporary RED-only wrappers and legacy compile probe were removed after
their contracts became green; the canonical signature assertions remain in
`tests/support/rest_v2_endpoint_task8_contracts.c`.

## Retained operation inventory

All rows use `const dcc_rest_call_options_t *options` and
`dcc_rest_request_t **out_request`; all 47 are `multipart: false`.

Guild operations (28):

1. `dcc_rest_begin_guild_prune`
2. `dcc_rest_create_guild_ban`
3. `dcc_rest_create_guild_soundboard_sound`
4. `dcc_rest_delete_guild_ban`
5. `dcc_rest_delete_guild_integration`
6. `dcc_rest_delete_guild_soundboard_sound`
7. `dcc_rest_get_current_user_guilds`
8. `dcc_rest_get_guild`
9. `dcc_rest_get_guild_audit_log`
10. `dcc_rest_get_guild_ban`
11. `dcc_rest_get_guild_bans`
12. `dcc_rest_get_guild_integrations`
13. `dcc_rest_get_guild_preview`
14. `dcc_rest_get_guild_prune_count`
15. `dcc_rest_get_guild_soundboard_sound`
16. `dcc_rest_get_guild_soundboard_sounds`
17. `dcc_rest_get_guild_vanity_url`
18. `dcc_rest_get_guild_voice_regions`
19. `dcc_rest_get_guild_welcome_screen`
20. `dcc_rest_get_guild_widget`
21. `dcc_rest_get_guild_widget_json`
22. `dcc_rest_get_guild_widget_png`
23. `dcc_rest_leave_guild`
24. `dcc_rest_modify_guild`
25. `dcc_rest_modify_guild_incident_actions`
26. `dcc_rest_modify_guild_soundboard_sound`
27. `dcc_rest_modify_guild_welcome_screen`
28. `dcc_rest_modify_guild_widget`

Guild-member operations (12):

1. `dcc_rest_add_guild_member`
2. `dcc_rest_add_guild_member_role`
3. `dcc_rest_bulk_ban_guild_members`
4. `dcc_rest_get_current_user_guild_member`
5. `dcc_rest_get_guild_member`
6. `dcc_rest_list_guild_members`
7. `dcc_rest_modify_current_guild_member`
8. `dcc_rest_modify_guild_member`
9. `dcc_rest_remove_guild_member`
10. `dcc_rest_remove_guild_member_role`
11. `dcc_rest_search_guild_members`
12. `dcc_rest_set_current_guild_member_nickname` (deprecated in favor of
    Modify Current Guild Member, but retained because Discord documents it)

Role operations (7):

1. `dcc_rest_create_guild_role`
2. `dcc_rest_delete_guild_role`
3. `dcc_rest_get_guild_role`
4. `dcc_rest_get_guild_role_member_counts`
5. `dcc_rest_get_guild_roles`
6. `dcc_rest_modify_guild_role`
7. `dcc_rest_modify_guild_role_positions`

## Removed surface

The four stale canonical operations are absent from public declarations,
definitions, consumers, route bindings, and active manifest rows:

- `dcc_rest_create_guild`
- `dcc_rest_delete_guild`
- `dcc_rest_modify_guild_integration`
- `dcc_rest_sync_guild_integration`

The 32 reviewed legacy names now occur only in the manifest removal ledger,
its audit fixture, and the historical Task 8 brief:

1. `dcc_rest_add_guild_member_params`
2. `dcc_rest_add_guild_member_role_params`
3. `dcc_rest_begin_guild_prune_options`
4. `dcc_rest_begin_guild_prune_params`
5. `dcc_rest_build_bulk_ban_body`
6. `dcc_rest_bulk_ban_guild_members_params`
7. `dcc_rest_create_guild_params`
8. `dcc_rest_create_guild_ban_params`
9. `dcc_rest_create_guild_ban_seconds`
10. `dcc_rest_create_guild_role_params`
11. `dcc_rest_build_guild_soundboard_sound_create_body`
12. `dcc_rest_create_guild_soundboard_sound_params`
13. `dcc_rest_get_guild_audit_log_page`
14. `dcc_rest_get_guild_bans_page`
15. `dcc_rest_get_guild_prune_count_options`
16. `dcc_rest_list_guild_members_page`
17. `dcc_rest_modify_current_guild_member_params`
18. `dcc_rest_modify_guild_params`
19. `dcc_rest_build_guild_incident_actions_body`
20. `dcc_rest_modify_guild_incident_actions_params`
21. `dcc_rest_clear_guild_member_timeout`
22. `dcc_rest_modify_guild_member_params`
23. `dcc_rest_move_guild_member`
24. `dcc_rest_set_guild_member_timeout`
25. `dcc_rest_modify_guild_role_params`
26. `dcc_rest_modify_guild_role_positions_params`
27. `dcc_rest_build_guild_soundboard_sound_modify_body`
28. `dcc_rest_modify_guild_soundboard_sound_params`
29. `dcc_rest_modify_guild_welcome_screen_params`
30. `dcc_rest_modify_guild_widget_params`
31. `dcc_rest_search_guild_members_page`
32. `dcc_rest_set_current_guild_member_nickname_params`

## Versioned record contract

Every new record starts with `size`, `version`, and `present`; body records
that support explicit JSON null also carry `nulls`. Version 1 accepts a fully
covered historical prefix and a larger same-version record, rejects partial
field coverage, bad versions, unknown presence/null bits, null-without-present,
and a present field outside the declared byte prefix. Construction starts only
after nested records, spans, alignment, and checked address arithmetic pass.

Minimum accepted prefixes are expressed with the final mandatory field so
they remain correct across ABIs:

| Record family | Minimum prefix ends at |
| --- | --- |
| Guild/current-guild/audit/bans/members/prune/widget-image queries | `present` |
| Member-search query | mandatory `query` |
| Add Guild Member | mandatory `access_token` |
| Bulk Ban | mandatory `user_id_count` |
| Role colors | mandatory `primary_color` |
| Role position element | mandatory `role_id` |
| Welcome channel element | mandatory `description` |
| Sound create | mandatory `sound` |
| Other optional-only Task 8 bodies | `present` |

Query presence masks, in serialization order:

- Guild: `WITH_COUNTS`.
- Current-user guilds: `BEFORE`, `AFTER`, `LIMIT`, `WITH_COUNTS`.
- Audit log: `USER_ID`, `ACTION_TYPE`, `BEFORE`, `AFTER`, `LIMIT`.
- Bans: `BEFORE`, `AFTER`, `LIMIT` (wire order remains limit/before/after).
- Members: `AFTER`, `LIMIT` (wire order limit/after).
- Search: mandatory `query`, optional `LIMIT`.
- Prune: `DAYS`, `INCLUDE_ROLES`.
- Widget image: `STYLE`.

Member/ban/prune body masks:

- Add Member: mandatory `access_token`; `NICK`, `ROLES`, `MUTE`, `DEAF`; no
  nullable bits; null input invalid.
- Modify Member: `NICK`, `ROLES`, `MUTE`, `DEAF`, `CHANNEL_ID`,
  `COMMUNICATION_DISABLED_UNTIL`, `FLAGS`; all seven nullable; null input `{}`.
- Modify Current Member: `NICK`, `BANNER`, `AVATAR`, `BIO`; all nullable; null
  input `{}`.
- Current Nickname: `NICK`; nullable; null input `{}`.
- Create Ban: `DELETE_MESSAGE_SECONDS`; nonnullable; null input `{}`.
- Bulk Ban: mandatory `user_ids/user_id_count`, optional nonnullable
  `DELETE_MESSAGE_SECONDS`; null input invalid.
- Prune: `DAYS`, `COMPUTE_PRUNE_COUNT`, `INCLUDE_ROLES`; nonnullable; null
  input `{}`.

Role masks:

- Role colors: mandatory `primary_color`; `SECONDARY_COLOR` and
  `TERTIARY_COLOR`, both nullable.
- Role create/update share `NAME`, `PERMISSIONS`, `COLOR`, `COLORS`, `HOIST`,
  `ICON`, `UNICODE_EMOJI`, `MENTIONABLE`. Create permits null only for icon and
  Unicode emoji; update permits null for all eight. Both accept null input as
  `{}`.
- Role positions use mandatory `role_id` and nullable `POSITION`; the outer
  array must be nonempty and uses the first element's validated `size` as its
  checked stride.

Guild and auxiliary masks:

- Guild Update uses the exact 20-bit order `NAME`, `REGION`,
  `VERIFICATION_LEVEL`, `DEFAULT_MESSAGE_NOTIFICATIONS`,
  `EXPLICIT_CONTENT_FILTER`, `AFK_CHANNEL_ID`, `AFK_TIMEOUT`, `ICON`, `SPLASH`,
  `DISCOVERY_SPLASH`, `BANNER`, `SYSTEM_CHANNEL_ID`, `SYSTEM_CHANNEL_FLAGS`,
  `RULES_CHANNEL_ID`, `PUBLIC_UPDATES_CHANNEL_ID`, `PREFERRED_LOCALE`,
  `FEATURES`, `DESCRIPTION`, `PREMIUM_PROGRESS_BAR_ENABLED`,
  `SAFETY_ALERTS_CHANNEL_ID`. The nullable set is region, the three enum
  fields, AFK/channel IDs except flags, the image strings, preferred locale,
  description, and safety-alerts channel. Name, AFK timeout, system-channel
  flags, features, and premium-progress boolean are nonnullable.
- Incident Actions: `INVITES_DISABLED_UNTIL`, `DMS_DISABLED_UNTIL`, both
  nullable.
- Widget Update: `ENABLED`, `CHANNEL_ID`; channel is nullable.
- Welcome Channel: mandatory channel/description; `EMOJI_ID`, `EMOJI_NAME`,
  both independently nullable but mutually exclusive when non-null.
- Welcome Screen: `ENABLED`, `WELCOME_CHANNELS`, `DESCRIPTION`, all nullable.
- Sound Create: mandatory name/sound; `VOLUME`, `EMOJI_ID`, `EMOJI_NAME`, all
  nullable. Sound Update: `NAME`, `VOLUME`, `EMOJI_ID`, `EMOJI_NAME`; all but
  name nullable. Create rejects null input; update accepts `{}`.

The shared syntax path now validates real ISO-8601 calendar dates, including
leap days, while Discord remains authoritative for the dynamic 28-day member
timeout and 24-hour incident horizons. Guild names validate 2..100 Unicode
scalars and the complete Unicode White_Space set at both edges. Sound data is
JSON (not multipart), accepts structurally valid MP3/Ogg base64 data URIs, and
preflights the 512 KiB decoded limit before construction allocation.

## Authentication and audit matrix

- `dcc_rest_get_guild_widget_json` and `dcc_rest_get_guild_widget_png` are
  public and reject Bot/Bearer overrides.
- `dcc_rest_get_current_user_guild_member` is Bearer-only.
- `dcc_rest_get_current_user_guilds` accepts the reviewed user modes (default
  Bot or an explicit Bearer token).
- The other 43 retained operations use Bot authentication.

Exactly 21 operations permit an audit reason: add/remove member role, begin
prune, bulk ban, create/delete ban, create/delete/modify role, create/delete/
modify soundboard sound, delete integration, modify current member, modify
guild, modify member, modify role positions, modify welcome screen, modify
widget, remove member, and set current member nickname. The other 26 reject a
supplied audit reason before admission. The common Task 7 pipeline performs
UTF-8 scalar validation, percent encoding, the 512-scalar boundary, owned
metadata copying, and header cleanup.

## Ownership, allocation, and sensitive data

Task 8 validates and serializes every query/body synchronously, then the common
request submission layer owns its path, query, JSON, authorization, and audit
metadata before returning. Caller arrays, nested records, and strings are not
retained. Checked span/alignment tests cover member roles, prune roles, guild
features, role positions, and welcome-channel strides; malformed nested role
colors are proven to fail with zero construction allocations.

Add Guild Member unconditionally adds
`DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY`; caller flags cannot clear it. The
async copy uses the common secure-free lifecycle, and Task 8 now also wipes the
temporary serialized JSON, including its terminating byte, on both successful
submission and pre-submit failure. The focused forced-admission test proves
one additional 49-byte secure wipe for the temporary OAuth JSON allocation.
ASan/UBSan and TSan cover the request/endpoint lifecycle after these changes.

Local validation clears `*out_request`, performs no endpoint construction
allocation for the checked rejection matrix, and leaves transport, callback,
and observer counters unchanged. Native endpoint tests also exercise explicit
false/zero/null/empty-object encodings and the retained request-handle wait and
terminal-result contract.

## Verification commands and results

Manifest and surface gates:

- `python3 tools/audit_rest_v2_endpoints.py --self-test` — PASS; 123 malformed
  manifest/state fixtures rejected.
- `python3 tools/audit_rest_v2_endpoints.py --source . --progress-through 8` —
  PASS; 224 endpoints, task counts 41/35/47/57/44, 101 deferred violations.
- Direct manifest count — PASS; Task 8 is 47 endpoints, 21 audit-enabled, zero
  multipart.
- `DCC_DISCORD_API_DOCS_ROOT=.codex-build/discord-api-docs python3
  tools/audit_discord_api_docs_surface.py` — PASS against Discord docs commit
  `b45077c6f6ab64992879cb238c18c861c8a5488c`; 231 official route entries, 231
  documented method/path routes, 227 canonical routes, and 226 DCC public
  route bindings.
- `python3 tools/audit_v2_surface.py --source . --compiler /usr/bin/clang
  --cxx-compiler /usr/bin/clang++ --llam-include
  /Users/feral/Desktop/Programming/LLAM/include --transition` — PASS; all 455
  public headers compiled as strict C11 and C++17.
- REST surface audit — PASS; 197 DPP methods mapped to 231 canonical wrappers.
- REST request assertion audit — PASS; 217 DPP wrappers, 210 parsed request
  assertions, and 18 bespoke assertions.
- REST field smoke audit — PASS; 43 REST parameter structs and the registered
  wire/path/multipart gap matrix.
- Official surface audit — PASS; 78 new, 3 existing, 9 object symbols, and 78
  Sugar macros.
- Public API audit — PASS; 1,733 declarations and 3,765 library symbols.
- Exact stale/legacy symbol searches outside the removal ledger and brief —
  PASS.

Native, docs, and packaging:

- `cmake --build .codex-build/task8 -j8` — PASS.
- `ctest --test-dir .codex-build/task8 --output-on-failure -j8` — PASS,
  181/181 enabled tests in 35.99 seconds.
- `python3 -m mkdocs build --strict` — PASS in 1.12 seconds.
- `cmake --install .codex-build/task8 --prefix
  .codex-build/task8-install-final` — PASS.
- Standalone configure/build/run of `tests/package_consumer` against that
  install, using `/Users/feral/Desktop/Programming/LLAM` and
  `libllam_runtime.a` — PASS, including the installed module target.
- `git diff --check` and `git diff --cached --check` — PASS before the
  implementation commit.

Sanitizers on the final implementation source:

- ASan/UBSan request, endpoint, complete HTTP, official-surface, and strict
  header smoke selection — PASS, 5/5 in 5.12 seconds with
  `ASAN_OPTIONS=detect_leaks=0:halt_on_error=1` and
  `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`.
- TSan request, endpoint, and official-surface selection — PASS, 3/3 in 3.45
  seconds with `TSAN_OPTIONS=halt_on_error=1:exitcode=66:history_size=7`.

Cross-platform evidence:

- Both `x86_64-w64-mingw32` and `i686-w64-mingw32` compiled all 19 changed
  public headers under warning-as-error C11 and C++17.
- Both MinGW targets compiled the final shared Task 8 core, member/role body,
  and guild body translation units under `-std=c11 -Wall -Wextra -Werror
  -pedantic`.

## Known pre-existing boundary

A diagnostic TSan run that included the complete `dcc_http_smoke` exposed an
unrelated existing race between `event_waiter_match.c:89` and
`event_waiter_runtime.c:74` in the public event-waiter test path. Task 8 does
not touch that subsystem. The same complete HTTP smoke passes natively and
under ASan/UBSan; the Task 8 request, endpoint, and official-surface TSan
selection is clean.

The user's pre-existing modification to
`tests/support/hot_reload_isolated_last_good_retry.c` remains unstaged and was
not altered by the Task 8 commit. `.codex-build/` remains untracked.

Task 8 is ready for independent review. No known Task 8 production or test
failure remains.
