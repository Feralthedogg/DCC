# Task 9 implementer report

## Result

Task 9 is implemented on `codex/v2-api-impl` by commit
`210f4db69b66c8fb4e3d744c3e0e00444b65dd3d` with subject
`feat: migrate command REST endpoints to requests`.

The registered transition gate now passes through Task 9 at 180/224 endpoints
(80.4%). All 57 application-command and adjacent resource operations use
request handles, typed/versioned inputs, endpoint capability admission, and
owned serialized state. The reviewed 39-symbol removal ledger is absent from
the public/definition surface. No push, tag, release, or merge was performed.

## RED and GREEN evidence

RED was committed first as `40bc9a2` (`test: define command REST migration
contract`). It failed against the old callback/raw/composite surface and fixed
the expected Task 9 inventory at 57 active operations, 30 active legacy names,
two stale canonicals with two attached aliases, five composite symbols, one
multipart endpoint, and 15 audit-capable endpoints.

GREEN evidence after `210f4db`:

- `python3 tools/audit_rest_v2_endpoints.py --source . --progress-through 9`
  passed: `224 endpoints; task 6=41, task 7=35, task 8=47, task 9=57,
  task 10=44; deferred violations=44`.
- The focused HTTP, request-assertion, field-smoke, REST-surface, and public-API
  audits passed together (5/5).
- `cmake --build .codex-build/task8 -j8` passed with strict warning policy.
- Full enabled CTest passed: 183/183. The 24 imported LLAM standalone tests
  numbered 184-207 were disabled by this DCC build configuration.
- `git diff --check` passed before the implementation commit.

The existing build also compiles the installed/public header probes and the C
and C++ consumers registered by CMake. Dedicated MinGW, sanitizer, and TSan
jobs were not rerun locally for this intermediate task; they remain mandatory
release gates in Task 15 rather than being represented here as local evidence.

## Exact 57 canonical operations

1. `dcc_rest_bulk_overwrite_global_commands`
2. `dcc_rest_bulk_overwrite_guild_commands`
3. `dcc_rest_create_application_emoji`
4. `dcc_rest_create_auto_moderation_rule`
5. `dcc_rest_create_global_command`
6. `dcc_rest_create_guild_command`
7. `dcc_rest_create_guild_emoji`
8. `dcc_rest_create_guild_scheduled_event`
9. `dcc_rest_create_guild_sticker`
10. `dcc_rest_create_guild_template`
11. `dcc_rest_create_stage_instance`
12. `dcc_rest_delete_application_emoji`
13. `dcc_rest_delete_auto_moderation_rule`
14. `dcc_rest_delete_global_command`
15. `dcc_rest_delete_guild_command`
16. `dcc_rest_delete_guild_emoji`
17. `dcc_rest_delete_guild_scheduled_event`
18. `dcc_rest_delete_guild_sticker`
19. `dcc_rest_delete_guild_template`
20. `dcc_rest_delete_stage_instance`
21. `dcc_rest_edit_global_command`
22. `dcc_rest_edit_guild_command`
23. `dcc_rest_edit_guild_command_permissions`
24. `dcc_rest_get_all_guild_command_permissions`
25. `dcc_rest_get_application_emoji`
26. `dcc_rest_get_application_emojis`
27. `dcc_rest_get_auto_moderation_rule`
28. `dcc_rest_get_auto_moderation_rules`
29. `dcc_rest_get_global_command`
30. `dcc_rest_get_global_commands`
31. `dcc_rest_get_guild_command`
32. `dcc_rest_get_guild_command_permissions`
33. `dcc_rest_get_guild_commands`
34. `dcc_rest_get_guild_emoji`
35. `dcc_rest_get_guild_emojis`
36. `dcc_rest_get_guild_onboarding`
37. `dcc_rest_get_guild_scheduled_event`
38. `dcc_rest_get_guild_scheduled_event_users`
39. `dcc_rest_get_guild_scheduled_events`
40. `dcc_rest_get_guild_sticker`
41. `dcc_rest_get_guild_stickers`
42. `dcc_rest_get_guild_templates`
43. `dcc_rest_get_stage_instance`
44. `dcc_rest_get_sticker`
45. `dcc_rest_get_sticker_pack`
46. `dcc_rest_get_sticker_packs`
47. `dcc_rest_get_template`
48. `dcc_rest_guild_message_search`
49. `dcc_rest_modify_application_emoji`
50. `dcc_rest_modify_auto_moderation_rule`
51. `dcc_rest_modify_guild_emoji`
52. `dcc_rest_modify_guild_onboarding`
53. `dcc_rest_modify_guild_scheduled_event`
54. `dcc_rest_modify_guild_sticker`
55. `dcc_rest_modify_guild_template`
56. `dcc_rest_modify_stage_instance`
57. `dcc_rest_sync_guild_template`

## Exact 39-symbol removal ledger

1. `dcc_rest_bulk_edit_guild_command_permissions`
2. `dcc_rest_bulk_edit_guild_command_permissions_params`
3. `dcc_rest_bulk_overwrite_application_commands`
4. `dcc_rest_bulk_overwrite_application_commands_builder`
5. `dcc_rest_bulk_overwrite_global_commands_builder`
6. `dcc_rest_bulk_overwrite_guild_commands_builder`
7. `dcc_rest_create_application_command`
8. `dcc_rest_create_application_command_builder`
9. `dcc_rest_create_application_emoji_params`
10. `dcc_rest_create_auto_moderation_rule_params`
11. `dcc_rest_create_global_command_builder`
12. `dcc_rest_create_guild_command_builder`
13. `dcc_rest_create_guild_emoji_params`
14. `dcc_rest_create_guild_from_template`
15. `dcc_rest_create_guild_from_template_params`
16. `dcc_rest_create_guild_scheduled_event_params`
17. `dcc_rest_create_guild_sticker_multipart`
18. `dcc_rest_create_guild_sticker_params`
19. `dcc_rest_create_guild_template_params`
20. `dcc_rest_create_stage_instance_params`
21. `dcc_rest_delete_all_application_commands`
22. `dcc_rest_delete_all_global_commands`
23. `dcc_rest_delete_all_guild_commands`
24. `dcc_rest_edit_global_command_builder`
25. `dcc_rest_edit_guild_command_builder`
26. `dcc_rest_edit_guild_command_permissions_params`
27. `dcc_rest_get_global_commands_with_localizations`
28. `dcc_rest_get_guild_commands_with_localizations`
29. `dcc_rest_get_guild_scheduled_event_users_page`
30. `dcc_rest_get_guild_scheduled_event_with_user_count`
31. `dcc_rest_get_guild_scheduled_events_with_user_count`
32. `dcc_rest_guild_message_search_params`
33. `dcc_rest_modify_application_emoji_params`
34. `dcc_rest_modify_auto_moderation_rule_params`
35. `dcc_rest_modify_guild_emoji_params`
36. `dcc_rest_modify_guild_onboarding_params`
37. `dcc_rest_modify_guild_scheduled_event_params`
38. `dcc_rest_modify_guild_template_params`
39. `dcc_rest_modify_stage_instance_params`

## Inventory and capability matrix

- Domains: application commands 20, auto moderation 5, emoji/sticker 13,
  message search 1, onboarding 2, scheduled events 6, stage instances 4,
  templates 6.
- Methods: GET 25, POST 9, PUT 5, PATCH 9, DELETE 9.
- Auth: BOT 42, BOT_OR_BEARER 14, BEARER-only 1 (guild-command permission
  edit). Bearer values are copied on admission and authorization diagnostics
  remain redacted.
- Audit reason: allowed 15, denied 42. Reasons use the Task 7 raw-scalar
  ownership and single percent-encoding contract.
- Multipart: one (`dcc_rest_create_guild_sticker`), non-multipart 56. File
  bytes and metadata are copied before successful submission returns.

All endpoint submitters null the output handle first, validate before queue
admission, and retain no caller-owned route/body/query storage. After `DCC_OK`,
the worker owns the serialized request and delivers exactly one terminal
result. Null output auto-releases; retained outputs remain caller-owned.

The command registry apply operation owns its aggregate state, runs legs in
deterministic order with at most one in flight, records the first terminal
error, supports dry-run/all-NOOP completion, and preserves retained-handle and
callback lifetime rules across cancellation and teardown.

## Exact deferred Task 10 list (44)

1. `dcc_rest_add_group_dm_recipient`
2. `dcc_rest_add_lobby_member`
3. `dcc_rest_bulk_update_lobby_members`
4. `dcc_rest_consume_entitlement`
5. `dcc_rest_create_lobby`
6. `dcc_rest_create_or_join_lobby`
7. `dcc_rest_create_test_entitlement`
8. `dcc_rest_delete_current_user_application_role_connection`
9. `dcc_rest_delete_lobby`
10. `dcc_rest_delete_test_entitlement`
11. `dcc_rest_get_activity_instance`
12. `dcc_rest_get_application_role_connection_metadata`
13. `dcc_rest_get_current_application`
14. `dcc_rest_get_current_user`
15. `dcc_rest_get_current_user_application_role_connection`
16. `dcc_rest_get_current_user_connections`
17. `dcc_rest_get_current_user_voice_state`
18. `dcc_rest_get_default_soundboard_sounds`
19. `dcc_rest_get_entitlement`
20. `dcc_rest_get_entitlements`
21. `dcc_rest_get_gateway`
22. `dcc_rest_get_gateway_bot`
23. `dcc_rest_get_lobby`
24. `dcc_rest_get_lobby_messages`
25. `dcc_rest_get_sku_subscription`
26. `dcc_rest_get_sku_subscriptions`
27. `dcc_rest_get_skus`
28. `dcc_rest_get_user`
29. `dcc_rest_get_user_voice_state`
30. `dcc_rest_get_voice_regions`
31. `dcc_rest_leave_lobby`
32. `dcc_rest_link_lobby_channel`
33. `dcc_rest_modify_current_application`
34. `dcc_rest_modify_current_user`
35. `dcc_rest_modify_current_user_voice_state`
36. `dcc_rest_modify_lobby`
37. `dcc_rest_modify_user_voice_state`
38. `dcc_rest_remove_group_dm_recipient`
39. `dcc_rest_remove_lobby_member`
40. `dcc_rest_send_lobby_message`
41. `dcc_rest_send_soundboard_sound`
42. `dcc_rest_update_application_role_connection_metadata`
43. `dcc_rest_update_current_user_application_role_connection`
44. `dcc_rest_update_lobby_message_moderation_metadata`

Strict mode therefore fails only on the registered Task 10 cutover debt. Task
10 must eliminate those 44 deferred violations; Task 14 will remove the
temporary test-only compatibility shim together with all DCC 1 consumers.
