# DCC Architecture

DCC is a C library. The repository layout is part of the API contract because C has no package-private namespace.

## Layout

```text
include/dcc/      Public headers installed for applications.
src/              Library implementation files.
src/internal/     Private headers shared only by DCC implementation units.
tests/            Smoke, package, sanitizer, and audit tests.
examples/         Buildable examples that use public headers.
docs/             Maintainer and user documentation.
tools/            Source audits and release tooling.
third_party/      Vendored dependency inventory. Empty unless code is vendored.
fuzz/             Fuzz harnesses.
build*/           Local build trees, ignored by source packaging.
```

## Header Boundary

Public headers live only under `include/dcc/` and are included as `<dcc/name.h>`.
They must be self-contained and must not include anything from `src/internal/`.

Internal headers live only under `src/internal/` and are included as `"internal/name.h"` or
`"internal/module/name.h"`. They are available to the `dcc` target and to tests that explicitly
opt into private headers.

`src/internal/dcc_internal.h` is a compatibility umbrella only. Core helpers,
runtime state, event state, REST client state, cache tables, voice client state,
Gateway lifecycle hooks, and the concrete client object live in domain-specific
headers under `src/internal/` instead of accumulating in the umbrella.
The layout audit rejects new direct includes of this umbrella outside the
umbrella itself.

## Module Size

Directory structure is not enough for DCC. Implementation files are expected to
map to one module responsibility instead of becoming catch-all files.

The layout audit enforces a soft source budget of 2500 lines for new `src/*.c`
files and a public-header budget of 1800 lines. Temporary oversized files must
be kept on explicit capped budgets so they cannot grow silently:

- Public object model declarations keep `include/dcc/objects.h` as the
  compatibility aggregate, while base resources, gateway update payloads,
  interactions, generic gateway data, and clone/accessor APIs live under
  `include/dcc/objects/`. Resource object declarations keep
  `include/dcc/objects/resources.h` as the compatibility aggregate, while guild,
  channel, role, member, scheduled-event, commerce, invite, stage, soundboard,
  emoji, and sticker object groups live under `include/dcc/objects/resources/`.
  Generic gateway data declarations keep `include/dcc/objects/gateway.h` as the
  compatibility aggregate, while voice state, activity, and normalized event-data
  object groups live under `include/dcc/objects/gateway/`.
  Gateway update payload declarations keep `include/dcc/objects/gateway_updates.h`
  as the compatibility aggregate, while message and poll updates, voice updates,
  moderation updates, thread updates, guild updates, channel and presence
  updates, and application command permission updates live under
  `include/dcc/objects/gateway_updates/`.
  Clone/free and interaction accessor declarations keep
  `include/dcc/objects/clone.h` as the compatibility aggregate, while base
  object, resource, gateway update, interaction, and generic gateway clone
  groups live under `include/dcc/objects/clone/`.
- Cluster declarations keep `include/dcc/cluster.h` as the compatibility
  aggregate, while base declarations, shared types, lifecycle controls, shard
  operations, gateway helpers, rolling operations, health/recovery/remediation
  APIs, fixed-buffer recovery/remediation-loop JSON, listener fanout, and supervisor policy
  controls live under `include/dcc/cluster/`.
- Hot-reload module authoring keeps `include/dcc/hot_reload/module.h` as the
  compatibility aggregate, while module types, context helpers,
  descriptor-based init/deinit helpers, and export macros live under
  `include/dcc/hot_reload/module/`.
- Interaction webhook server declarations keep
  `include/dcc/interaction_server.h` as the compatibility aggregate, while base
  declarations, route registration and built-in health route helpers,
  lifecycle/drain controls, health/stat snapshots, request accessors/responses,
  and signature verification live under `include/dcc/interaction_server/`.
- Event API declarations keep `include/dcc/events.h` as the compatibility
  aggregate, while base declarations, event enums, payload views, accessors,
  listener helpers, and wait APIs live under `include/dcc/events/`. Event wait
  declarations keep `include/dcc/events/wait.h` as the compatibility aggregate,
  while wait types, generic event waits, and owned object waits live under
  `include/dcc/events/wait/`. Owned object waits keep
  `include/dcc/events/wait/owned_objects.h` as the compatibility aggregate,
  while single-type waits and any-of waits live under
  `include/dcc/events/wait/owned_objects/`.
  Composed Gateway helpers cover READY/RESUMED-or-close,
  gateway/interaction admission, interaction-or-close, and voice descriptor
  waits without exposing internal waiter state.
- Event internals keep `dcc_event_state_internal.h` and
  `dcc_events_internal.h` as compatibility aggregates. Listener state, waiter
  state, event bus state/API, concrete event storage, interaction listener
  state, bus runtime helpers, waiter runtime helpers, owned waiter clone/free,
  waiter setup helpers, and owned wait APIs live in focused
  `src/internal/events/dcc_event_*_internal.h` headers. Concrete event objects
  keep `dcc_event_object_internal.h` as the struct aggregate, while the event
  payload union lives in `dcc_event_object_data_internal.h`.
- REST parameter and enum declarations keep `include/dcc/rest/types.h` as the
  compatibility aggregate, while domain-specific type groups live under
  `include/dcc/rest/types/`.
- Low-level REST core declarations keep `include/dcc/rest/core.h` as the
  compatibility aggregate, while method helpers, direct request helpers, async
  queue controls, and future-based request helpers live under
  `include/dcc/rest/core/`.
- Channel REST declarations keep `include/dcc/rest/channels.h` as the
  compatibility aggregate, while channel lifecycle, guild channel positions,
  permission overwrites, channel actions, and channel invite API groups live
  under `include/dcc/rest/channels/`.
- Guild REST declarations keep `include/dcc/rest/guilds.h` as the compatibility
  aggregate, while core guild operations, current member updates, audit logs,
  bans, integrations, widgets, prune, onboarding, welcome screen, auto
  moderation, and scheduled event API groups live under
  `include/dcc/rest/guilds/`.
- Guild member REST declarations keep `include/dcc/rest/guild_members.h` as the
  compatibility aggregate, while member query/search, lifecycle and
  voice/timeout updates, and member role API groups live under
  `include/dcc/rest/guild_members/`.
- Emoji and sticker REST declarations keep
  `include/dcc/rest/emojis_stickers.h` as the compatibility aggregate, while
  guild emoji, guild sticker, global sticker lookup, and guild message search
  API groups live under `include/dcc/rest/emojis_stickers/`.
- Secondary REST resource declarations keep `include/dcc/rest/resources.h` as
  the compatibility aggregate, while stage instances, templates, users and DM
  channel operations, direct-message helpers, group DMs, voice states,
  entitlements, gateway discovery, and invite API groups live under
  `include/dcc/rest/resources/`.
- Message REST declarations keep `include/dcc/rest/messages.h` as the
  compatibility aggregate, while message fetch, create, edit/delete, reaction,
  pin, and poll API groups live under `include/dcc/rest/messages/`.
- Thread REST declarations keep `include/dcc/rest/threads.h` as the
  compatibility aggregate, while lifecycle, creation, membership, and
  archive/list API groups live under `include/dcc/rest/threads/`.
- Interaction REST declarations keep `include/dcc/rest/interactions.h` as the
  compatibility aggregate, while initial callback responses, original response
  operations, and followup message operations live under
  `include/dcc/rest/interactions/`. Followup declarations keep
  `include/dcc/rest/interactions/followups.h` as the compatibility aggregate,
  while token-based helpers and `dcc_interaction_t` convenience helpers live
  under `include/dcc/rest/interactions/followups/`.
- Application command REST declarations keep
  `include/dcc/rest/application_commands.h` as the compatibility aggregate,
  while application emoji, default-global registration helpers, global command,
  guild command, and guild command permission API groups live under
  `include/dcc/rest/application_commands/`.
  Global and guild command declarations keep their compatibility aggregates,
  while fetch, write, and bulk API groups live under
  `include/dcc/rest/application_commands/global_commands/` and
  `include/dcc/rest/application_commands/guild_commands/`.
- Webhook REST declarations keep `include/dcc/rest/webhooks.h` as the
  compatibility aggregate, while webhook management, execution, and message API
  groups live under `include/dcc/rest/webhooks/`. Webhook management
  declarations keep `include/dcc/rest/webhooks/management.h` as the
  compatibility aggregate, while create, fetch, modify, and delete API groups
  live under `include/dcc/rest/webhooks/management/`. Webhook message
  declarations keep `include/dcc/rest/webhooks/messages.h` as the compatibility
  aggregate, while fetch, edit, and delete API groups live under
  `include/dcc/rest/webhooks/messages/`.
- Hot reload declarations keep `include/dcc/hot_reload.h` as the compatibility
  aggregate, while base declarations, module authoring helpers, run options,
  lifecycle controls, health snapshots, and state accessors live under
  `include/dcc/hot_reload/`.
- Voice API declarations keep `include/dcc/voice.h` as the compatibility
  aggregate, while base declarations, shared types, lifecycle, DAVE, gateway,
  media, transport, notification, and supervisor APIs live under
  `include/dcc/voice/`. Shared voice types keep `include/dcc/voice/types.h` as
  the compatibility aggregate, while enums, session/health snapshots, codec
  callbacks, and DAVE MLS frame types live under `include/dcc/voice/types/`.
  Voice internals keep
  `dcc_voice_state_internal.h` as the compatibility aggregate, while limits,
  session/descriptor fields, DAVE fields, UDP transport state, receive/reorder
  state, websocket loop state, codec state, send queue state, and owner registry
  declarations live in focused `src/internal/voice/dcc_voice_*_internal.h`
  headers. Shared voice implementation declarations keep
  `dcc_voice_internal.h` as a compatibility aggregate, while protocol constants,
  session helpers, utility helpers, Gateway JSON helpers, crypto helpers, send
  queue helpers, receive/reorder helpers, and JSON emission helpers live in
  focused voice internal headers.
- Low-level HTTP transport is split by responsibility. `dcc_http.c` owns the
  LLAM connection lifecycle and send/receive loop, `dcc_http_url.c` owns URL
  parsing, `dcc_http_request.c` owns request serialization and cancellation
  state, `dcc_http_response.c` owns response assembly/deinit,
  `dcc_http_response_headers.c` owns response header scanning,
  `dcc_http_response_chunked.c` owns chunked body decoding, and
  `dcc_http_buffer.c` owns the shared growable buffer. HTTP internal
  declarations keep `dcc_http.h` as the compatibility aggregate, while buffer,
  URL, request, response, and perform/lifecycle declarations live in focused
  `src/internal/http/dcc_http_*_internal.h` headers.
- Low-level socket transport keeps connection open/close orchestration in
  `dcc_conn.c`, read/write loops in `dcc_conn_io.c`, TCP dialing in
  `dcc_conn_tcp.c`, and TLS setup/poll retry handling in `dcc_conn_tls.c`.
- REST implementation is split by responsibility. `rest.c` owns the core
  typed/raw request wrappers, `rest_request_path.c` owns owned-path and query
  path helpers, `rest_request_multipart.c` owns public multipart request
  wrappers, and lower-level HTTP execution and rate-limit buckets live in the
  request/async implementation files. `rest_async.c` keeps public async
  enqueue APIs and wait signaling, `rest_async_request.c` owns request
  allocation/cancel primitives, `rest_async_queue.c` owns pending and active
  queue links, `rest_async_routes.c` owns route serialization and wake
  scheduling, `rest_async_drain.c` owns dispatch draining,
  `rest_async_worker.c` owns HTTP worker execution,
  `rest_async_worker_lifecycle.c` owns active request completion,
  `rest_async_retry.c` owns retry wake/requeue handling,
  `rest_async_status.c` owns async status snapshots,
  `rest_async_cancel.c` owns pending/active cancellation APIs, and
  `rest_async_wait.c` owns async wait APIs. REST async internals keep
  `dcc_rest_async_internal.h` as the compatibility aggregate, while request,
  queue, signal, drain, retry, worker, status, wait, and cancel declarations live in focused
  `src/internal/rest/dcc_rest_async_*_internal.h` headers. Future lifecycle,
  request wrappers, and wait/destroy APIs live in `rest_future_lifecycle.c`,
  `rest_future_request.c`, and `rest_future_wait.c`. Platform
  wait/time helpers, common buffer, JSON,
  query, method,
  path, multipart, interaction, application-command, emoji, sticker, invite,
  channel, message, thread, guild, guild-member, message-search, role,
  application-role-connection, stage-instance, template, user/DM, group-DM,
  voice-state, entitlement, gateway, and webhook endpoint helpers live in
  `rest_buffer.c`, `rest_capture.c`, `rest_json.c`,
  `rest_json_arrays.c`, `rest_json_array_members.c`,
  `rest_headers.c`, `rest_lifecycle.c`, `rest_rate_limit.c`,
  `rest_request.c`, request header construction lives in
  `rest_request_headers.c`, request rate-limit response handling lives in
  `rest_request_rate_limit_response.c`, URL construction lives in `rest_request_url.c`,
  `rest_routes.c`, `rest_platform.c`,
  `rest_query_append.c`, `rest_query_collections.c`, `rest_methods.c`,
  `rest_paths.c`, `rest_interaction_response_body.c`,
  `rest_interaction_response_create.c`, `rest_interaction_response_builders.c`,
  `rest_interaction_response_wrappers.c`,
  `rest_application_commands.c`, `rest_application_command_permission_builders.c`,
  `rest_emoji_builders.c`,
  `rest_guild_emojis.c`, `rest_application_emojis.c`, guild sticker APIs live in
  `rest_guild_stickers.c`, global sticker APIs live in `rest_stickers.c`,
  `rest_invites.c`, `rest_channels.c`, direct-message creation lives in
  `rest_direct_messages.c`, direct-message response JSON parsing lives in
  `rest_direct_messages_json.c`, `rest_messages.c`,
  `rest_multipart.c`, multipart body orchestration lives in
  `rest_multipart_build.c`, multipart part serialization lives in
  `rest_multipart_parts.c`, multipart validation lives in
  `rest_multipart_validate.c`, guild create/modify body
  helpers live in `rest_guild_body.c`, guild audit-log endpoints live in
  `rest_guilds_audit_log.c`, guild core endpoints live in `rest_guilds_core.c`,
  `rest_guilds_current_member.c`, `rest_guilds_integrations.c`,
  ban fetch endpoints live in `rest_guild_bans_fetch.c`, ban create/delete
  endpoints live in `rest_guild_bans_modify.c`, prune endpoints live in
  `rest_guilds_prune.c`, prune request body building lives in
  `rest_guild_prune_body.c`,
  `rest_guilds_welcome.c`, `rest_guild_members_query.c`,
  `rest_guild_members_lifecycle.c`, guild member timeout/move convenience APIs
  live in `rest_guild_member_controls.c`, `rest_guild_member_roles.c`,
  `rest_guild_member_builders.c`, message search endpoints live in
  `rest_message_search.c`, message search query building lives in
  `rest_message_search_query.c`, role body builders
  live in `rest_role_body.c`, role fetch/create/modify/position/delete APIs live
  in `rest_roles_fetch.c`, `rest_roles_create.c`, `rest_roles_modify.c`,
  `rest_roles_positions.c`, and `rest_roles_delete.c`,
  `rest_role_connections.c`, role connection key validation lives in
  `rest_role_connections_key.c`, role connection metadata body building lives in
  `rest_role_connections_metadata_builder.c`, user role connection endpoints live in
  `rest_role_connections_user.c`, and user role connection body building lives in
  `rest_role_connections_user_builder.c`,
  `rest_stage_instances.c`, template body/path helpers live in
  `rest_template_body.c` and `rest_template_paths.c`, template fetch/create/
  modify/sync-delete APIs live in `rest_templates_fetch.c`,
  `rest_templates_create.c`, `rest_templates_modify.c`, and
  `rest_templates_sync_delete.c`, `rest_threads.c`,
  `rest_thread_body.c`, `rest_thread_create.c`, `rest_thread_members.c`,
  `rest_thread_archives.c`, base user resource wrappers live in
  `rest_users.c`, current-user profile wrappers live in `rest_users_current.c`,
  user DM channel wrappers live in `rest_users_dms.c`, `rest_group_dms.c`,
  voice state body building lives in `rest_voice_state_body.c`, current-user voice state APIs live in
  `rest_voice_states_current_user.c`, user voice state APIs live in
  `rest_voice_states_user.c`, voice region APIs live in `rest_voice_regions.c`, SKU endpoints
  live in `rest_skus.c`, entitlement listing lives in `rest_entitlements.c`,
  test entitlement actions live in `rest_entitlements_test.c`,
  `rest_gateway.c`, `rest_onboarding.c`,
  `rest_onboarding_builders.c`, `rest_onboarding_option_builders.c`,
  `rest_onboarding_prompt_builders.c`,
  `rest_auto_moderation.c`, `rest_auto_moderation_builders.c`,
  `rest_auto_moderation_actions.c`, `rest_auto_moderation_metadata.c`,
  scheduled-event body orchestration, JSON member serialization, and validation/defaulting live in
  `rest_scheduled_event_builders.c`, `rest_scheduled_event_body.c`, and
  `rest_scheduled_event_validate.c`; scheduled-event endpoints live in
  `rest_scheduled_events.c` and `rest_scheduled_event_users.c`,
  `rest_webhook_body.c`, `rest_webhooks_create.c`,
  `rest_webhooks_fetch.c`, `rest_webhooks_modify.c`,
  `rest_webhooks_delete.c`, `rest_webhook_helpers.c`, `rest_webhook_execute.c`,
  `rest_webhook_messages_fetch.c`, `rest_webhook_messages_edit.c`,
  and `rest_webhook_messages_delete.c`.
  REST internal declarations keep `dcc_rest_internal.h` as a compatibility
  aggregate, while config macros, shared buffers, capture helpers, JSON helpers,
  builders, path formatting, query serialization, multipart payload helpers,
  rate-limit coordination, rate-limit bucket table helpers, runtime wait/time helpers, async
  queues, futures, thread helpers, and REST client state live in focused headers
  under `src/internal/rest/`. REST builder declarations keep
  `dcc_rest_builders_internal.h` as a compatibility aggregate, while channel,
  auto-moderation, guild-member, scheduled-event, emoji, application-command
  permission, onboarding, and role-connection body builders live in focused
  builder headers. REST JSON helper declarations keep
  `dcc_rest_json_internal.h` as a compatibility aggregate, while scalar/raw
  object member helpers and array member helpers live in focused JSON headers.
  REST query declarations keep `dcc_rest_query_internal.h` as a compatibility
  aggregate, while scalar append helpers, collection/repeated helpers, and
  webhook option query helpers live in focused query headers and implementation
  files.
  Request bridge declarations keep
  `dcc_rest_request_internal.h` as a compatibility aggregate, while core
  path/query wrappers, payload multipart requests, message builder requests,
  application-command builder requests, interaction path helpers, request URL
  helpers, webhook token path helpers, and the raw HTTP implementation hook live in focused
  `dcc_rest_request_*_internal.h` headers.
  Interaction REST is split further: callback response body building lives in
  `rest_interaction_response_body.c`, raw callback creation lives in
  `rest_interaction_response_create.c`, builder-based callback helpers live in
  `rest_interaction_response_builders.c`, interaction-object convenience
  wrappers live in `rest_interaction_response_wrappers.c`, shared interaction
  field/path helpers live in `rest_interaction_helpers.c`, original response token APIs live in
  `rest_interaction_original.c`, original response interaction wrappers live in
  `rest_interaction_original_interaction.c`, token-based followup create APIs live in
  `rest_interaction_followup_create.c`, token-based followup message get/edit/delete APIs live in
  `rest_interaction_followup_message.c`, followup multipart APIs live in
  `rest_interaction_followup_multipart.c`, and interaction convenience followup
  wrappers are split into create, message, and multipart files:
  `rest_interaction_followup_interaction_create.c`,
  `rest_interaction_followup_interaction_message.c`, and
  `rest_interaction_followup_interaction_multipart.c`.
  Webhook REST is split further: webhook body serialization lives in
  `rest_webhook_body.c`; management APIs are split into
  `rest_webhooks_create.c`, `rest_webhooks_fetch.c`,
  `rest_webhooks_modify.c`, and `rest_webhooks_delete.c`; token path and
  option query helpers live in `rest_webhook_helpers.c`; execute APIs live in
  `rest_webhook_execute.c`; webhook message fetch and delete APIs live in
  `rest_webhook_messages_fetch.c` and `rest_webhook_messages_delete.c`; and
  webhook message edit APIs are split into base JSON, builder, and multipart
  flows in `rest_webhook_messages_edit.c`,
  `rest_webhook_messages_edit_builder.c`, and
  `rest_webhook_messages_edit_multipart.c`.
  Application command REST is split further: default-global create, bulk
  overwrite, and delete-all registration with explicit global/guild scope
  helpers lives in `rest_application_commands_registration.c`;
  global command fetch APIs stay in
  `rest_application_commands.c`, global write and bulk APIs live in
  `rest_application_commands_global_write.c` and
  `rest_application_commands_global_bulk.c`, builder request bridges live in
  `rest_application_command_builders.c`, guild command fetch/create/edit/delete
  and bulk APIs live in `rest_application_commands_guild_fetch.c`,
  `rest_application_commands_guild_create.c`,
  `rest_application_commands_guild_edit.c`, and
  `rest_application_commands_guild_delete_bulk.c`, and guild command permission
  APIs live in `rest_application_command_permissions.c`, permission body builders
  live in `rest_application_command_permission_builders.c`, and permission JSON
  members live in `rest_application_command_permission_json.c`.
  Thread REST is split further: base get/modify APIs stay in `rest_threads.c`,
  shared thread request body orchestration lives in `rest_thread_body.c`, thread
  body validation lives in `rest_thread_body_validate.c`, thread body member
  serialization lives in `rest_thread_body_members.c`, creation APIs live in
  `rest_thread_create.c`, membership APIs live in
  `rest_thread_members.c`, and archive/list APIs live in
  `rest_thread_archives.c`.
  Channel REST is split further: basic channel/guild-channel CRUD wrappers stay
  in `rest_channels.c`, channel body orchestration lives in
  `rest_channel_builders.c`, channel builder validation/default extraction lives
  in `rest_channel_builder_context.c`, channel JSON member orchestration lives in
  `rest_channel_builder_json.c`, base channel member serialization lives in
  `rest_channel_builder_base_json.c`, forum/media member serialization lives in
  `rest_channel_builder_forum_json.c`, tail/archive member serialization lives in
  `rest_channel_builder_tail_json.c`, permission overwrite builders live in
  `rest_channel_permission_builders.c`, forum/media tag builders live in
  `rest_channel_forum_builders.c`,
  guild channel position updates live in `rest_channel_positions.c`, permission
  overwrite APIs live in `rest_channel_permissions.c`, typing/follow/voice-status
  APIs live in `rest_channel_actions.c`, and channel invite APIs live in
  `rest_channel_invites.c`.
  Message REST is split further: channel message listing/get APIs stay in
  `rest_messages.c`, builder payload request bridges live in
  `rest_message_builders.c`, message creation variants live in
  `rest_message_create.c`, edit variants live in `rest_message_edit.c`, flag
  payload helpers live in `rest_message_flags.c`, delete/crosspost/bulk-delete
  actions live in `rest_message_actions.c`, reaction add/remove APIs live in
  `rest_message_reactions.c`, reaction path helpers live in
  `rest_message_reaction_paths.c`, reaction fetch/page APIs live in
  `rest_message_reactions_fetch.c`, reaction delete-all APIs live in
  `rest_message_reactions_delete_all.c`,
  pin APIs live in `rest_message_pins.c`, and poll voter/expire APIs live in
  `rest_message_polls.c`.
- JSON internal declarations keep `dcc_json_internal.h` as the compatibility
  umbrella. Gateway key declarations keep `dcc_json_keys.h` as the
  compatibility aggregate, while enum assembly lives in `dcc_json_key_ids.h`,
  lookup declarations live in `dcc_json_key_lookup.h`, and domain-specific enum
  entry groups live in `dcc_json_key_id_*_entries.h`. Resource key enum entries
  keep `dcc_json_key_id_resource_entries.h` as a compatibility aggregate, while
  media, voice, reference, scalar, and text/time resource key groups live in
  focused `dcc_json_key_id_resource_*_entries.h` headers. Collection key enum
  entries keep `dcc_json_key_id_collection_entries.h` as a compatibility
  aggregate, while core, text, time, client-status, resource-list, and activity
  collection key groups live in focused
  `dcc_json_key_id_collection_*_entries.h` headers. Payload storage keeps
  `dcc_json_gateway_payload_state.h` as the compatibility aggregate while
  limits, core top-level fields, resource fields, interaction fields, generic
  gateway fields, and collection backing arrays live in focused
  `dcc_json_gateway_payload_*` headers. Payload interaction storage keeps
  `dcc_json_gateway_payload_interaction_state.h` as the compatibility aggregate,
  while interaction core, option-list, resolved user, resolved member, and
  resolved resource backing fields live in focused
  `dcc_json_gateway_payload_interaction_*_state.h` headers. Payload resource storage keeps
  `dcc_json_gateway_payload_resource_state.h` as the compatibility aggregate,
  while message, primary resource, and update resource fields live in focused
  `dcc_json_gateway_payload_*_resource_state.h` headers. Primary resource
  storage keeps `dcc_json_gateway_payload_primary_resource_state.h` as a
  compatibility aggregate, while guild, base channel/role/member, scheduled
  event, commerce, and event resource fields live in focused
  `dcc_json_gateway_payload_*_resource_state.h` headers. Update resource
  storage keeps `dcc_json_gateway_payload_update_resource_state.h` as a
  compatibility aggregate, while thread/member, channel/voice-server,
  identity, and guild update fields live in focused
  `dcc_json_gateway_payload_*_update_resource_state.h` headers.
  Payload gateway storage keeps `dcc_json_gateway_payload_gateway_state.h` as a
  compatibility aggregate, while gateway core payload, text scratch buffers,
  time scratch buffers, media/name buffers, and ID collection buffers live in
  focused `dcc_json_gateway_payload_gateway_*_state.h` headers.
  Payload collection storage keeps
  `dcc_json_gateway_payload_collection_state.h` as the compatibility aggregate,
  while activity, guild resource, presence, list resource, and thread/member
  backing arrays live in focused
  `dcc_json_gateway_payload_*_collection_state.h` headers.
  JSON public/internal facade declarations keep `dcc_json.h` as a compatibility
  aggregate, while JSON types, DOM API declarations, and gateway parse API
  declarations live in `dcc_json_types.h`, `dcc_json_dom_api.h`, and
  `dcc_json_gateway_api.h`. Gateway payload helper declarations keep
  `dcc_json_gateway_payload.h` as a compatibility aggregate, while event-name
  helpers, event ID aliasing, payload normalization, and top-level payload
  parsing/reset declarations live in focused `dcc_json_gateway_*` payload
  headers.
  JSON core declarations keep `dcc_json_core.h` as a compatibility aggregate,
  while stage1 state/functions, arena state, DOM/doc state, and parser helpers
  live in `dcc_json_stage1_core.h`, `dcc_json_arena_core.h`,
  `dcc_json_dom_core.h`, and `dcc_json_parser_core.h`. DOM declarations keep
  `dcc_json_dom_core.h` as a compatibility aggregate, while document, array,
  object, and value storage structs live in `dcc_json_doc_core.h`,
  `dcc_json_array_core.h`, `dcc_json_object_core.h`, and
  `dcc_json_value_core.h`. Parser declarations keep `dcc_json_parser_core.h` as
  a compatibility aggregate, while parser state, parser function declarations,
  and shared capacity growth helpers live in `dcc_json_parser_state.h`,
  `dcc_json_parser_functions.h`, and `dcc_json_capacity.h`.
  Gateway temporary parse buffers keep
  `dcc_json_gateway_buffers.h` as the compatibility aggregate, while activity,
  presence, member/user, list-resource, resolved-resource, scheduled/thread/
  entitlement/stage, and interaction option scratch state live in focused
  `dcc_json_gateway_*_buffers.h` and `dcc_json_gateway_interaction_state.h`
  headers. Activity buffers keep `dcc_json_gateway_activity_buffers.h` as a
  compatibility aggregate while per-object and per-array scratch state live in
  `dcc_json_gateway_activity_object_buffers.h` and
  `dcc_json_gateway_activity_array_buffers.h`; member buffers keep
  `dcc_json_gateway_member_buffers.h` as a compatibility aggregate while user
  and member scratch state live in `dcc_json_gateway_user_buffers.h` and
  `dcc_json_gateway_member_object_buffers.h`. List-resource buffers keep
  `dcc_json_gateway_list_buffers.h` as a
  compatibility aggregate while emoji, sticker, and soundboard buffer structs
  live in focused `dcc_json_gateway_*_buffers.h` headers; resource buffers keep
  `dcc_json_gateway_resource_buffers.h` as a compatibility aggregate while
  scheduled-event, thread, entitlement, and stage-instance buffers live in
  focused buffer headers; presence buffers keep
  `dcc_json_gateway_presence_buffers.h` as a compatibility aggregate while
  status and presence-list buffer structs live in focused presence buffer
  headers; gateway payload normalization/dispatch,
  user/member parsers, list object parsers, resource collection parsers,
  `d` field dispatch, resolved maps, interaction payload details,
  activity/presence parsing, gateway array dispatch, parse helpers, and
  arena/string helpers are split into focused internal headers under
  `src/internal/json/`. Core key ID declarations keep
  `dcc_json_key_id_core_entries.h` as a compatibility aggregate, while gateway,
  message, and user/resource key groups live in focused
  `dcc_json_key_id_core_*_entries.h` headers. Parse helper declarations keep
  `dcc_json_parse_helpers.h` as a compatibility aggregate, while core token/key,
  skip, number/token, ID-array/object, and nullable string helpers live in
  focused `dcc_json_parse_*_helpers.h` headers. ID helper declarations keep
  `dcc_json_parse_id_helpers.h` as a compatibility aggregate, while snowflake
  array parsing, nested user ID parsing, object ID parsing, and object ID array
  parsing live in focused `dcc_json_parse_*` headers. Gateway `d` core field declarations keep
  `dcc_json_gateway_d_core_fields.h` as a compatibility aggregate, while
  identity/session/id fields, message fields, interaction fields, resource ID
  fields, and guild policy/capacity fields live in focused declaration headers.
  Core guild declarations keep `dcc_json_gateway_d_core_guild_fields.h` as a
  compatibility aggregate, while bool, member-count, capacity-limit,
  policy-limit, and limit-dispatch declarations live in focused
  `dcc_json_gateway_d_core_guild_*_fields.h` headers. Core message declarations
  keep `dcc_json_gateway_d_core_message_fields.h` as a compatibility aggregate,
  while message dispatch, text, bool, scheduled-event, flags, and author field
  declarations live in focused `dcc_json_gateway_d_core_message_*_fields.h`
  headers.
  Core identity declarations keep
  `dcc_json_gateway_d_core_identity_fields.h` as a compatibility aggregate,
  while core dispatch, identity dispatch, session/id fields, name/user fields,
  and application/type fields live in focused
  `dcc_json_gateway_d_core_*_fields.h` headers. Core session/ID declarations
  keep `dcc_json_gateway_d_core_session_id_fields.h` as a compatibility
  aggregate, while session, ID, channel ID, guild ID apply/mark, and webhook ID
  declarations live in focused `dcc_json_gateway_d_core_*_fields.h` headers.
  Gateway `d` ID field declarations keep `dcc_json_gateway_d_id_fields.h` as a
  compatibility aggregate, while ID dispatch, primary IDs, entitlement IDs,
  event IDs, and guild/media IDs live in focused
  `dcc_json_gateway_d_*_id_fields.h` headers.
  Gateway `d` collection declarations keep
  `dcc_json_gateway_d_collection_fields.h` as a compatibility aggregate, while
  collection dispatch, object collections, ID collections, member collections,
  and permission/voice collection handling live in focused
  `dcc_json_gateway_d_*_collection_fields.h` headers. `d` ID collection
  declarations keep `dcc_json_gateway_d_id_collection_fields.h` as a
  compatibility aggregate, while bulk, channel, dispatch, role, and subscription
  ID collection parsing live in focused
  `dcc_json_gateway_d_*_id_collection_fields.h` headers.
  Gateway `d` scalar field declarations keep
  `dcc_json_gateway_d_scalar_fields.h` as a compatibility aggregate, while
  numeric, bool, time, text, guild media, and scalar dispatch groups live in
  focused `dcc_json_gateway_d_*_scalar_fields.h`,
  `dcc_json_gateway_d_guild_media_fields.h`, and
  `dcc_json_gateway_d_scalar_dispatch_fields.h` headers. Numeric scalar
  declarations keep `dcc_json_gateway_d_numeric_scalar_fields.h` as a
  compatibility aggregate, while numeric dispatch, member, scheduled-event,
  moderation, invite, channel, and voice/presence numeric handling live in
  focused `dcc_json_gateway_d_*_numeric_scalar*.h` and
  `dcc_json_gateway_d_voice_presence_*_fields.h` headers. Time scalar
  declarations keep `dcc_json_gateway_d_time_scalar_fields.h` as a
  compatibility aggregate, while time dispatch, message timestamps,
  invite/thread, schedule/entitlement, subscription, member, and voice time
  handling live in focused `dcc_json_gateway_d_*_time_scalar*.h` and
  `dcc_json_gateway_d_message_timestamp*.h` headers. Text scalar declarations
  keep `dcc_json_gateway_d_text_scalar_fields.h` as a compatibility aggregate,
  while text dispatch, channel/invite text, status, emoji/voice text, context,
  auto-moderation, and member-context text parsing live in focused
  `dcc_json_gateway_d_*_text_fields.h` headers.
  Gateway user/member declarations keep `dcc_json_gateway_members.h` as a
  compatibility aggregate, while reusable user object parsing, top-level member
  object parsing, and guild member-list parsing live in
  `dcc_json_gateway_user_fields.h`, `dcc_json_gateway_member_fields.h`, and
  `dcc_json_gateway_member_list_fields.h`. User declarations keep
  `dcc_json_gateway_user_fields.h` as a compatibility aggregate, while user
  buffer initialization, object entry, object field iteration, object dispatch,
  ID/text/flag leaf parsing, and top-level author/user payload parsing live in
  focused `dcc_json_gateway_user_*` headers. Top-level member declarations keep
  `dcc_json_gateway_member_fields.h` as a compatibility aggregate, while member
  entry/finalization and user/text/time/scalar leaf parsing live in focused
  `dcc_json_gateway_member_*_fields.h` headers.
  Gateway member-list declarations keep
  `dcc_json_gateway_member_list_fields.h` as a compatibility aggregate, while
  guild member array parsing, member object/buffer parsing, and per-key member
  dispatch parsing live in `dcc_json_gateway_member_list_array_fields.h`,
  `dcc_json_gateway_member_list_object_fields.h`, and
  `dcc_json_gateway_member_list_dispatch_fields.h`. Member list object
  declarations keep `dcc_json_gateway_member_list_object_fields.h` as a
  compatibility aggregate, while buffer initialization, object entry,
  buffer-backed object parsing, object preparation, and object field iteration
  live in `dcc_json_gateway_member_list_buffer_init.h`,
  `dcc_json_gateway_member_list_object_entry.h`,
  `dcc_json_gateway_member_list_object_parse.h`,
  `dcc_json_gateway_member_list_object_prepare.h`, and
  `dcc_json_gateway_member_list_object_loop.h`. Member list dispatch declarations
  keep `dcc_json_gateway_member_list_dispatch_fields.h` as a compatibility
  aggregate, while entry dispatch and user/text/time/scalar leaf parsing live in
  focused `dcc_json_gateway_member_list_*_fields.h` headers.
  Gateway list-resource declarations keep `dcc_json_gateway_lists.h` as a
  compatibility aggregate, while emoji, sticker, and soundboard list parsers
  live in `dcc_json_gateway_emoji_list_fields.h`,
  `dcc_json_gateway_sticker_list_fields.h`, and
  `dcc_json_gateway_soundboard_list_fields.h`. Emoji list declarations keep
  `dcc_json_gateway_emoji_list_fields.h` as a compatibility aggregate, while
  buffer initialization, object entry/loop/dispatch, and ID/text/user/bool leaf
  parsing live in focused `dcc_json_gateway_emoji_list_*` headers. Soundboard
  list declarations keep `dcc_json_gateway_soundboard_list_fields.h` as a
  compatibility aggregate, while object entry/loop/dispatch and ID/user/text/
  scalar leaf parsing live in focused `dcc_json_gateway_soundboard_list_*`
  headers.
  Gateway array declarations keep `dcc_json_gateway_arrays.h` as a
  compatibility aggregate, while emoji, sticker, soundboard, thread, guild
  member, and thread-member array parsing live in
  `dcc_json_gateway_emoji_array_fields.h`,
  `dcc_json_gateway_sticker_array_fields.h`,
  `dcc_json_gateway_soundboard_array_fields.h`,
  `dcc_json_gateway_thread_array_fields.h`,
  `dcc_json_gateway_member_list_array_fields.h`, and
  `dcc_json_gateway_thread_member_array_fields.h`.
  Sticker list declarations keep `dcc_json_gateway_sticker_list_fields.h` as a
  compatibility aggregate, while object/buffer parsing and per-key dispatch live
  in `dcc_json_gateway_sticker_list_object_fields.h` and
  `dcc_json_gateway_sticker_list_dispatch_fields.h`. Sticker list object
  declarations keep `dcc_json_gateway_sticker_list_object_fields.h` as a
  compatibility aggregate, while object entry, buffer initialization, and object
  field iteration live in focused `dcc_json_gateway_sticker_list_*` object
  headers. Sticker list dispatch declarations keep
  `dcc_json_gateway_sticker_list_dispatch_fields.h` as a compatibility
  aggregate, while entry dispatch and ID/user/text/scalar leaf parsing live in
  focused `dcc_json_gateway_sticker_list_*_fields.h` headers.
  Gateway interaction declarations keep `dcc_json_gateway_interactions.h` as a
  compatibility aggregate, while interaction data fields, option arrays, option
  object fields, and option value parsing/finalization live in
  `dcc_json_gateway_interaction_data_fields.h`,
  `dcc_json_gateway_interaction_option_arrays.h`,
  `dcc_json_gateway_interaction_option_fields.h`, and
  `dcc_json_gateway_interaction_option_values.h`. Interaction data declarations
  keep `dcc_json_gateway_interaction_data_fields.h` as a compatibility
  aggregate, while interaction values, data dispatch, ID, scalar, text, object,
  and top-level data parsing live in focused
  `dcc_json_gateway_interaction_data_*` and
  `dcc_json_gateway_interaction_values_fields.h` headers. Interaction option array
  declarations keep `dcc_json_gateway_interaction_option_arrays.h` as a
  compatibility aggregate, while array entry, begin, item, loop, and top-level
  option parsing live in focused `dcc_json_gateway_interaction_option_array_*`
  and `dcc_json_gateway_interaction_options_entry.h` headers. Interaction option field
  declarations keep `dcc_json_gateway_interaction_option_fields.h` as a
  compatibility aggregate, while option object entry/loop, state finalization,
  field dispatch, metadata fields, and child option fields live in focused
  `dcc_json_gateway_interaction_option_*` headers. Interaction option value
  declarations keep `dcc_json_gateway_interaction_option_values.h` as a
  compatibility aggregate, while number text parsing, value utility helpers,
  value-field dispatch, null/bool/number/string parsing, and value finalizers
  live in focused `dcc_json_gateway_interaction_option_value_*` headers.
  Gateway activity declarations keep `dcc_json_gateway_activities.h` as a
  compatibility aggregate, while gateway emoji, activity object, activity array,
  and client status parsers live in `dcc_json_gateway_emoji_fields.h`,
  `dcc_json_gateway_activity_object_fields.h`,
  `dcc_json_gateway_activity_array_fields.h`, and
  `dcc_json_gateway_client_status_fields.h`. Activity object declarations keep
  `dcc_json_gateway_activity_object_fields.h` as a compatibility aggregate,
  while buffer initialization, emoji object parsing, text/scalar leaf parsing,
  object dispatch, object field iteration, and object entry parsing live in
  `dcc_json_gateway_activity_buffer_init.h`,
  `dcc_json_gateway_activity_emoji_object_fields.h`,
  `dcc_json_gateway_activity_text_fields.h`,
  `dcc_json_gateway_activity_scalar_fields.h`,
  `dcc_json_gateway_activity_object_dispatch.h`,
  `dcc_json_gateway_activity_object_loop.h`, and
  `dcc_json_gateway_activity_object_entry.h`. Activity array declarations keep
  `dcc_json_gateway_activity_array_fields.h` as a compatibility aggregate,
  while array entry, item parsing, loop parsing, array finalization, and
  top-level activity parsing live in focused `dcc_json_gateway_activity_*`
  headers.
  Gateway presence declarations keep `dcc_json_gateway_presence.h` as a
  compatibility aggregate, while nested presence client-status parsing,
  presence list parsing, and guild presence array parsing live in
  `dcc_json_gateway_presence_status_fields.h`,
  `dcc_json_gateway_presence_list_fields.h`, and
  `dcc_json_gateway_presence_array_fields.h`. Presence list declarations keep
  `dcc_json_gateway_presence_list_fields.h` as a compatibility aggregate, while
  list object/buffer parsing and per-key dispatch parsing live in
  `dcc_json_gateway_presence_list_object_fields.h` and
  `dcc_json_gateway_presence_list_dispatch_fields.h`. Presence list object
  declarations keep `dcc_json_gateway_presence_list_object_fields.h` as a
  compatibility aggregate, while buffer initialization, public object entry,
  buffer-backed object parse, and object field iteration live in
  `dcc_json_gateway_presence_list_buffer_init.h`,
  `dcc_json_gateway_presence_list_object_entry.h`,
  `dcc_json_gateway_presence_list_object_parse.h`, and
  `dcc_json_gateway_presence_list_object_loop.h`.
  Gateway resolved map declarations keep `dcc_json_gateway_resolved_maps.h` as a
  compatibility aggregate, while shared map iteration, user maps, member maps,
  role/channel/message/attachment maps, and resolved dispatch fields live in
  `dcc_json_gateway_resolved_map_core.h`,
  `dcc_json_gateway_resolved_user_maps.h`,
  `dcc_json_gateway_resolved_member_maps.h`,
  `dcc_json_gateway_resolved_resource_maps.h`, and
  `dcc_json_gateway_resolved_dispatch_fields.h`. Resolved resource maps keep
  `dcc_json_gateway_resolved_resource_maps.h` as a compatibility aggregate,
  while role, channel, message, and attachment resolved maps live in focused
  `dcc_json_gateway_resolved_*_maps.h` headers.
  Gateway resolved object declarations keep
  `dcc_json_gateway_resolved_objects.h` as a compatibility aggregate, while
  resolved message parsing, attachment parsing, and resolved permission storage
  helpers live in `dcc_json_gateway_resolved_message_fields.h`,
  `dcc_json_gateway_resolved_attachment_fields.h`, and
  `dcc_json_gateway_resolved_permission_fields.h`. Resolved message declarations
  keep `dcc_json_gateway_resolved_message_fields.h` as a compatibility
  aggregate, while message object entry/loop/dispatch and ID/author/text/scalar
  leaf parsing live in focused `dcc_json_gateway_resolved_message_*` headers.
  Resolved attachment
  declarations keep `dcc_json_gateway_resolved_attachment_fields.h` as a
  compatibility aggregate, while attachment object entry/loop/dispatch and
  ID/numeric/string/media leaf parsing live in focused
  `dcc_json_gateway_resolved_attachment_*` headers.
  Gateway resolved entitlement declarations keep
  `dcc_json_gateway_resolved_entitlements.h` as a compatibility aggregate, while
  authorizing integration owner maps, interaction entitlement arrays,
  entitlement object entry/loop/dispatch parsing, and entitlement ID/scalar/time
  fields live in focused `dcc_json_gateway_authorizing_integration_*.h` and
  `dcc_json_gateway_entitlement_*_fields.h` headers.
  Gateway resource role declarations keep `dcc_json_gateway_resource_roles.h` as
  a compatibility aggregate, while application command permission parsing, role
  entry parsing, role field/object parsing, guild role arrays, guild channel
  arrays, and guild scheduled-event arrays live in
  `dcc_json_gateway_application_permission_fields.h`,
  `dcc_json_gateway_role_object_entry.h`,
  `dcc_json_gateway_role_fields.h`,
  `dcc_json_gateway_guild_role_array_fields.h`,
  `dcc_json_gateway_guild_channel_array_fields.h`, and
  `dcc_json_gateway_guild_scheduled_event_array_fields.h`. Role field
  declarations keep `dcc_json_gateway_role_fields.h` as a compatibility
  aggregate, while role list object entry/dispatch, payload object dispatch,
  generic role field dispatch, and ID/text/scalar/bool leaf parsing live in
  focused `dcc_json_gateway_role_*` headers.
  Gateway resource thread declarations keep
  `dcc_json_gateway_resource_threads.h` as a compatibility aggregate, while
  thread list parsing, thread member parsing, and entity metadata parsing live in
  `dcc_json_gateway_thread_list_fields.h`,
  `dcc_json_gateway_thread_member_fields.h`, and
  `dcc_json_gateway_entity_metadata_fields.h`. Thread list declarations keep
  `dcc_json_gateway_thread_list_fields.h` as a compatibility aggregate, while
  object entry/loop, field dispatch, ID, scalar, and text parsing live in
  focused `dcc_json_gateway_thread_list_*` headers. Thread member declarations
  keep `dcc_json_gateway_thread_member_fields.h` as a compatibility aggregate,
  while object parsing, entry dispatch, ID/scalar leaf parsing, and timestamp
  parsing live in focused `dcc_json_gateway_thread_member_*_fields.h` headers.
  Gateway resource stage declarations keep `dcc_json_gateway_resource_stage.h` as
  a compatibility aggregate, while auto-moderation action entry parsing and
  stage-instance array, buffer, object, ID, scalar, and text parsing live in
  `dcc_json_gateway_action_object_entry.h`,
  `dcc_json_gateway_stage_instance_array_fields.h`,
  `dcc_json_gateway_stage_instance_buffer_init.h`,
  `dcc_json_gateway_stage_instance_object_entry.h`,
  `dcc_json_gateway_stage_instance_object_loop.h`,
  `dcc_json_gateway_stage_instance_object_dispatch.h`,
  `dcc_json_gateway_stage_instance_id_fields.h`,
  `dcc_json_gateway_stage_instance_scalar_fields.h`, and
  `dcc_json_gateway_stage_instance_text_fields.h`.
  Gateway scheduled-event declarations keep
  `dcc_json_gateway_resource_scheduled_events.h` as a compatibility aggregate,
  while scheduled-event object parsing and per-key field parsing live in
  `dcc_json_gateway_scheduled_event_object_fields.h` and
  `dcc_json_gateway_scheduled_event_fields.h`. Scheduled-event object
  declarations keep `dcc_json_gateway_scheduled_event_object_fields.h` as a
  compatibility aggregate, while buffer initialization, object entry, object
  field iteration, and object dispatch live in focused
  `dcc_json_gateway_scheduled_event_*` object headers. Scheduled-event field
  declarations keep `dcc_json_gateway_scheduled_event_fields.h` as a
  compatibility aggregate, while ID, numeric, text, identity text, time text,
  and creator field parsing live in focused
  `dcc_json_gateway_scheduled_event_*_fields.h` headers.
- JSON parsing is split by responsibility: DOM public parse/free, DOM allocation,
  DOM array parsing, DOM object parsing, DOM value parsing and public DOM
  accessors live in `dcc_json_dom.c`, `dcc_json_dom_alloc.c`,
  `dcc_json_dom_array.c`, `dcc_json_dom_object.c`, `dcc_json_dom_value.c`
  and `dcc_json_accessors.c`; stage1 scanning,
  stage1 mark storage, stage1 SIMD/portable mask classification, stage1 validation,
  arena allocation,
  string/UTF-8 decoding and string parse wrappers,
  gateway key classification, event-name helpers, common direct-parse helpers,
  gateway direct `d` field handler groups,
  gateway payload initialization, gateway payload top-level field dispatch,
  gateway payload top-level parsing, gateway payload normalization,
  interaction payload parsing, gateway guild collection parsers, gateway role
  parsers, gateway application-command permission parsers, gateway automod
  action parsers, gateway scheduled-event parsers, gateway stage-instance parsers,
  gateway interaction data dispatch, gateway interaction option/value parsers,
  gateway interaction resolved dispatcher, gateway interaction resolved
  map parsers, gateway interaction resolved resource parsers, gateway presence/activity
  parsers, gateway user/member object parsers, gateway list object parsers,
  and gateway list/array collection
  parsers live in
  `dcc_json_stage1.c`, `dcc_json_stage1_marks.c`, `dcc_json_stage1_mask.c`,
  `dcc_json_stage1_validate.c`, `dcc_json_arena.c`, `dcc_json_string.c`,
  `dcc_json_string_decode.c`, `dcc_json_string_escape.c`,
  `dcc_json_string_parse.c`, `dcc_json_string_utf8.c`,
  `dcc_json_keys.c`, `dcc_json_gateway_event_names.c`,
  `dcc_json_gateway_event_guild_id_aliases.c`,
  `dcc_json_gateway_event_id_aliases.c`,
  `dcc_json_gateway_events.c`,
  `dcc_json_gateway_d.c`,
  `dcc_json_parse_utils.c`, `dcc_json_parse_core.c`,
  `dcc_json_parse_number_span.c`, `dcc_json_parse_number_direct.c`,
  `dcc_json_parse_number_ascii.c`,
  `dcc_json_parse_nullable_strings.c`, `dcc_json_parse_nested_user_id.c`,
  `dcc_json_parse_object_id.c`,
  `dcc_json_parse_object_ids.c`,
  `dcc_json_parse_snowflake_arrays.c`, `dcc_json_parse_skip.c`,
  `dcc_json_parse_skip_array.c`, `dcc_json_parse_skip_object.c`,
  `dcc_json_parse_tokens.c`,
  `dcc_json_payload.c`, `dcc_json_gateway_payload_fields.c`,
  `dcc_json_gateway_payload_parse.c`, `dcc_json_interaction_payload_parse.c`,
  `dcc_json_gateway_normalize.c`,
  `dcc_json_gateway_normalize_identity.c`,
  `dcc_json_gateway_normalize_resource_commerce.c`,
  `dcc_json_gateway_normalize_resource_invite_stage.c`,
  `dcc_json_gateway_normalize_resource_scheduled.c`,
  `dcc_json_gateway_normalize_resource_soundboard.c`,
  `dcc_json_gateway_normalize_resources.c`,
  `dcc_json_gateway_normalize_message_reactions.c`,
  `dcc_json_gateway_normalize_messages.c`,
  `dcc_json_gateway_normalize_moderation.c`,
  `dcc_json_gateway_normalize_poll_votes.c`,
  `dcc_json_gateway_normalize_threads.c`,
  `dcc_json_gateway_normalize_update_channel.c`,
  `dcc_json_gateway_normalize_update_core.c`,
  `dcc_json_gateway_normalize_update_guild.c`,
  `dcc_json_gateway_normalize_updates.c`,
  `dcc_json_gateway_normalize_collections.c`,
  `dcc_json_gateway_d_core.c`,
  `dcc_json_gateway_d_core_name.c`,
  `dcc_json_gateway_d_core_user.c`,
  `dcc_json_gateway_d_core_guild_bools.c`,
  `dcc_json_gateway_d_core_guild_capacity_limit_keys.c`,
  `dcc_json_gateway_d_core_guild_capacity_limits.c`,
  `dcc_json_gateway_d_core_guild_limit_values.c`,
  `dcc_json_gateway_d_core_guild_limits.c`,
  `dcc_json_gateway_d_core_guild_policy_apply.c`,
  `dcc_json_gateway_d_core_guild_policy_limits.c`,
  `dcc_json_gateway_d_core_member_count.c`,
  `dcc_json_gateway_d_core_application_id.c`,
  `dcc_json_gateway_d_core_application_type.c`,
  `dcc_json_gateway_d_core_type.c`,
  `dcc_json_gateway_d_core_channel_id.c`,
  `dcc_json_gateway_d_core_guild_id.c`,
  `dcc_json_gateway_d_core_guild_id_fields.c`,
  `dcc_json_gateway_d_core_guild_id_flags.c`,
  `dcc_json_gateway_d_core_id.c`,
  `dcc_json_gateway_d_core_identity.c`,
  `dcc_json_gateway_d_core_message.c`,
  `dcc_json_gateway_d_core_message_author.c`,
  `dcc_json_gateway_d_core_message_bool.c`,
  `dcc_json_gateway_d_core_message_flags.c`,
  `dcc_json_gateway_d_core_message_scheduled.c`,
  `dcc_json_gateway_d_core_message_scheduled_object.c`,
  `dcc_json_gateway_d_core_message_scheduled_text.c`,
  `dcc_json_gateway_d_core_message_text.c`,
  `dcc_json_gateway_d_core_session.c`,
  `dcc_json_gateway_d_core_guild_user.c`,
  `dcc_json_gateway_d_core_webhook_id.c`,
  `dcc_json_gateway_d_core_interaction.c`,
  `dcc_json_gateway_d_core_interaction_objects.c`,
  `dcc_json_gateway_d_core_interaction_scalars.c`,
  `dcc_json_gateway_d_core_interaction_text.c`,
  `dcc_json_gateway_d_ids.c`,
  `dcc_json_gateway_d_primary_message_ids.c`,
  `dcc_json_gateway_d_primary_ids.c`,
  `dcc_json_gateway_d_primary_resource_ids.c`,
  `dcc_json_gateway_d_primary_user_id.c`,
  `dcc_json_gateway_d_entitlement_ids.c`,
  `dcc_json_gateway_d_entitlement_nullable_ids.c`,
  `dcc_json_gateway_d_entitlement_values.c`,
  `dcc_json_gateway_d_event_ids.c`,
  `dcc_json_gateway_d_moderation_event_ids.c`,
  `dcc_json_gateway_d_resource_event_ids.c`,
  `dcc_json_gateway_d_scheduled_event_ids.c`,
  `dcc_json_gateway_d_guild_channel_id_apply.c`,
  `dcc_json_gateway_d_guild_channel_ids.c`,
  `dcc_json_gateway_d_media_ids.c`,
  `dcc_json_gateway_d_numeric_scalars.c`,
  `dcc_json_gateway_d_member_numeric_scalars.c`,
  `dcc_json_gateway_d_scheduled_event_numeric_scalars.c`,
  `dcc_json_gateway_d_moderation_numeric_scalars.c`,
  `dcc_json_gateway_d_invite_numeric_scalars.c`,
  `dcc_json_gateway_d_channel_numeric_scalar_apply.c`,
  `dcc_json_gateway_d_channel_numeric_scalars.c`,
  `dcc_json_gateway_d_voice_presence_double.c`,
  `dcc_json_gateway_d_voice_presence_numeric_scalars.c`,
  `dcc_json_gateway_d_voice_presence_time.c`,
  `dcc_json_gateway_d_voice_presence_u32.c`,
  `dcc_json_gateway_d_bool_scalar_fields.c`,
  `dcc_json_gateway_d_bool_scalars.c`,
  `dcc_json_gateway_d_time_scalars.c`,
  `dcc_json_gateway_d_message_timestamp.c`,
  `dcc_json_gateway_d_message_timestamp_text.c`,
  `dcc_json_gateway_d_message_time_scalars.c`,
  `dcc_json_gateway_d_invite_thread_time_scalars.c`,
  `dcc_json_gateway_d_schedule_entitlement_time_scalars.c`,
  `dcc_json_gateway_d_subscription_time_scalars.c`,
  `dcc_json_gateway_d_member_time_scalars.c`,
  `dcc_json_gateway_d_member_voice_time_scalars.c`,
  `dcc_json_gateway_d_voice_time_scalars.c`,
  `dcc_json_gateway_d_text_scalars.c`,
  `dcc_json_gateway_d_guild_avatar.c`,
  `dcc_json_gateway_d_guild_icon_banner.c`,
  `dcc_json_gateway_d_guild_media_scalars.c`,
  `dcc_json_gateway_d_guild_media_text.c`,
  `dcc_json_gateway_entitlement_ids.c`,
  `dcc_json_gateway_entitlement_object_dispatch.c`,
  `dcc_json_gateway_entitlement_object_fields.c`,
  `dcc_json_gateway_entitlement_objects.c`,
  `dcc_json_gateway_entitlement_array_finish.c`,
  `dcc_json_gateway_entitlement_array_items.c`,
  `dcc_json_gateway_entitlement_scalars.c`,
  `dcc_json_gateway_entitlement_times.c`,
  `dcc_json_gateway_entitlements.c`,
  `dcc_json_gateway_entity_metadata.c`,
  `dcc_json_gateway_entity_metadata_fields.c`,
  `dcc_json_gateway_actions.c`, `dcc_json_gateway_activities.c`,
  `dcc_json_gateway_activity_array_finish.c`,
  `dcc_json_gateway_activity_array_items.c`,
  `dcc_json_gateway_activity_array_loop.c`,
  `dcc_json_gateway_activity_arrays.c`,
  `dcc_json_gateway_activity_buffers.c`,
  `dcc_json_gateway_activity_emoji.c`,
  `dcc_json_gateway_activity_emoji_fields.c`,
  `dcc_json_gateway_activity_object_dispatch.c`,
  `dcc_json_gateway_activity_object_fields.c`,
  `dcc_json_gateway_activity_objects.c`,
  `dcc_json_gateway_activity_scalar_fields.c`,
  `dcc_json_gateway_activity_text_fields.c`,
  `dcc_json_gateway_application_permission_fields.c`,
  `dcc_json_gateway_application_permission_object.c`,
  `dcc_json_gateway_application_permission_object_fields.c`,
  `dcc_json_gateway_application_permissions.c`, `dcc_json_gateway_client_status.c`,
  `dcc_json_gateway_client_status_entries.c`,
  `dcc_json_gateway_client_status_fields.c`,
  `dcc_json_gateway_d_scalars.c`, `dcc_json_gateway_d_collections.c`,
  `dcc_json_gateway_d_object_collections.c`,
  `dcc_json_gateway_d_media_collections.c`,
  `dcc_json_gateway_d_guild_collections.c`,
  `dcc_json_gateway_d_bulk_id_collections.c`,
  `dcc_json_gateway_d_channel_id_collections.c`,
  `dcc_json_gateway_d_id_collections.c`,
  `dcc_json_gateway_d_role_id_collections.c`,
  `dcc_json_gateway_d_subscription_id_collections.c`,
  `dcc_json_gateway_d_auto_moderation_text.c`,
  `dcc_json_gateway_d_member_array_collections.c`,
  `dcc_json_gateway_d_member_collections.c`,
  `dcc_json_gateway_d_member_id_collections.c`,
  `dcc_json_gateway_d_permission_collections.c`,
  `dcc_json_gateway_d_channel_invite_text.c`,
  `dcc_json_gateway_d_context_text.c`,
  `dcc_json_gateway_d_member_context_text.c`,
  `dcc_json_gateway_d_emoji_voice_text.c`,
  `dcc_json_gateway_d_status_text.c`,
  `dcc_json_gateway_d_voice_flags.c`,
  `dcc_json_gateway_guild_channel_array_finish.c`,
  `dcc_json_gateway_guild_channel_array_items.c`,
  `dcc_json_gateway_guilds.c`, `dcc_json_gateway_role_arrays.c`,
  `dcc_json_gateway_role_bool_fields.c`, `dcc_json_gateway_role_fields.c`,
  `dcc_json_gateway_role_id_fields.c`,
  `dcc_json_gateway_role_object_fields.c`,
  `dcc_json_gateway_role_object_parse_fields.c`,
  `dcc_json_gateway_role_objects.c`,
  `dcc_json_gateway_role_scalar_fields.c`,
  `dcc_json_gateway_role_text_fields.c`, `dcc_json_gateway_roles.c`,
  `dcc_json_gateway_interaction_data_fields.c`,
  `dcc_json_gateway_interaction_data_id_fields.c`,
  `dcc_json_gateway_interaction_data_object_fields.c`,
  `dcc_json_gateway_interaction_data_scalar_fields.c`,
  `dcc_json_gateway_interaction_data_text_fields.c`,
  `dcc_json_gateway_interaction_owner_entries.c`,
  `dcc_json_gateway_interaction_owners.c`,
  `dcc_json_gateway_interactions.c`, `dcc_json_gateway_interaction_option_values.c`,
  `dcc_json_gateway_interaction_option_array_loop.c`,
  `dcc_json_gateway_interaction_option_arrays.c`,
  `dcc_json_gateway_interaction_option_array_items.c`,
  `dcc_json_gateway_interaction_option_array_lists.c`,
  `dcc_json_gateway_interaction_option_child_fields.c`,
  `dcc_json_gateway_interaction_option_fields.c`,
  `dcc_json_gateway_interaction_option_finalize.c`,
  `dcc_json_gateway_interaction_option_meta_fields.c`,
  `dcc_json_gateway_interaction_option_number_text.c`,
  `dcc_json_gateway_interaction_option_object_fields.c`,
  `dcc_json_gateway_interaction_option_objects.c`,
  `dcc_json_gateway_interaction_option_value_bool.c`,
  `dcc_json_gateway_interaction_option_value_fields.c`,
  `dcc_json_gateway_interaction_option_value_null.c`,
  `dcc_json_gateway_interaction_option_value_number.c`,
  `dcc_json_gateway_interaction_option_value_parse_bool.c`,
  `dcc_json_gateway_interaction_option_value_parse_number.c`,
  `dcc_json_gateway_interaction_option_value_parse_string.c`,
  `dcc_json_gateway_interaction_option_value_string.c`,
  `dcc_json_gateway_interaction_option_value_utils.c`,
  `dcc_json_gateway_interaction_options.c`,
  `dcc_json_gateway_interaction_values.c`,
  `dcc_json_gateway_emoji_apply.c`,
  `dcc_json_gateway_emoji.c`,
  `dcc_json_gateway_emoji_fields.c`,
  `dcc_json_gateway_emoji_object_fields.c`,
  `dcc_json_gateway_list_emoji_arrays.c`,
  `dcc_json_gateway_list_emoji_bools.c`,
  `dcc_json_gateway_list_emoji_buffers.c`,
  `dcc_json_gateway_list_emoji_ids.c`,
  `dcc_json_gateway_list_emoji_array_items.c`,
  `dcc_json_gateway_list_emoji_object_dispatch.c`,
  `dcc_json_gateway_list_emoji_object_fields.c`,
  `dcc_json_gateway_list_emoji_objects.c`,
  `dcc_json_gateway_list_emoji_text_fields.c`,
  `dcc_json_gateway_list_emoji_user_fields.c`,
  `dcc_json_gateway_list_objects.c`,
  `dcc_json_gateway_list_soundboard_array_items.c`,
  `dcc_json_gateway_list_soundboard_arrays.c`,
  `dcc_json_gateway_list_sticker_array_items.c`,
  `dcc_json_gateway_list_sticker_arrays.c`,
  `dcc_json_gateway_list_sticker_buffers.c`,
  `dcc_json_gateway_list_sticker_fields.c`,
  `dcc_json_gateway_list_sticker_id_fields.c`,
  `dcc_json_gateway_list_sticker_object_fields.c`,
  `dcc_json_gateway_list_sticker_objects.c`,
  `dcc_json_gateway_list_sticker_scalar_fields.c`,
  `dcc_json_gateway_list_sticker_text_fields.c`,
  `dcc_json_gateway_list_sticker_user_fields.c`,
  `dcc_json_gateway_list_stickers.c`,
  `dcc_json_gateway_list_soundboard.c`,
  `dcc_json_gateway_list_soundboard_fields.c`,
  `dcc_json_gateway_list_soundboard_id_fields.c`,
  `dcc_json_gateway_list_soundboard_object_fields.c`,
  `dcc_json_gateway_list_soundboard_objects.c`,
  `dcc_json_gateway_list_soundboard_scalar_fields.c`,
  `dcc_json_gateway_list_soundboard_text_fields.c`,
  `dcc_json_gateway_list_soundboard_user_fields.c`,
  `dcc_json_gateway_list_thread_array_finish.c`,
  `dcc_json_gateway_list_thread_ids.c`,
  `dcc_json_gateway_list_threads.c`,
  `dcc_json_gateway_list_thread_member_arrays.c`,
  `dcc_json_gateway_list_thread_member_fields.c`,
  `dcc_json_gateway_list_thread_member_ids.c`,
  `dcc_json_gateway_list_thread_member_objects.c`,
  `dcc_json_gateway_list_thread_member_scalars.c`,
  `dcc_json_gateway_list_thread_member_times.c`,
  `dcc_json_gateway_list_thread_fields.c`,
  `dcc_json_gateway_list_thread_object_fields.c`,
  `dcc_json_gateway_list_thread_objects.c`,
  `dcc_json_gateway_list_thread_scalars.c`,
  `dcc_json_gateway_list_thread_text_fields.c`,
  `dcc_json_gateway_list_thread_members.c`,
  `dcc_json_gateway_nullable_string_fields.c`,
  `dcc_json_gateway_presence.c`, `dcc_json_gateway_presence_client_status.c`,
  `dcc_json_gateway_presence_client_status_object_fields.c`,
  `dcc_json_gateway_presence_client_status_fields.c`,
  `dcc_json_gateway_user_buffers.c`,
  `dcc_json_gateway_user_flag_fields.c`,
  `dcc_json_gateway_user_id_fields.c`,
  `dcc_json_gateway_user_object_dispatch.c`,
  `dcc_json_gateway_user_object_fields.c`,
  `dcc_json_gateway_user_objects.c`,
  `dcc_json_gateway_user_text_fields.c`,
  `dcc_json_gateway_users.c`,
  `dcc_json_gateway_member_fields.c`,
  `dcc_json_gateway_member_finish.c`,
  `dcc_json_gateway_member_scalar_fields.c`,
  `dcc_json_gateway_member_text_fields.c`,
  `dcc_json_gateway_member_time_fields.c`,
  `dcc_json_gateway_member_user_fields.c`,
  `dcc_json_gateway_members.c`,
  `dcc_json_gateway_member_list_arrays.c`,
  `dcc_json_gateway_member_list_buffers.c`,
  `dcc_json_gateway_member_list_entries.c`,
  `dcc_json_gateway_member_list_fields.c`,
  `dcc_json_gateway_member_list_object_fields.c`,
  `dcc_json_gateway_member_list_object_parse.c`,
  `dcc_json_gateway_member_list_object_prepare.c`,
  `dcc_json_gateway_member_list_scalar_fields.c`,
  `dcc_json_gateway_member_list_objects.c`,
  `dcc_json_gateway_member_list_text_fields.c`,
  `dcc_json_gateway_member_list_time_fields.c`,
  `dcc_json_gateway_member_list_user_fields.c`,
  `dcc_json_gateway_member_lists.c`,
  `dcc_json_gateway_resolved.c`, `dcc_json_gateway_resolved_fields.c`,
  `dcc_json_gateway_resolved_users.c`,
  `dcc_json_gateway_resolved_user_entries.c`,
  `dcc_json_gateway_resolved_user_arrays.c`,
  `dcc_json_gateway_resolved_members.c`,
  `dcc_json_gateway_resolved_member_entries.c`,
  `dcc_json_gateway_resolved_member_map_items.c`,
  `dcc_json_gateway_resolved_member_arrays.c`,
  `dcc_json_gateway_resolved_channel_arrays.c`,
  `dcc_json_gateway_resolved_channel_entries.c`,
  `dcc_json_gateway_resolved_channel_maps.c`,
  `dcc_json_gateway_resolved_role_arrays.c`,
  `dcc_json_gateway_resolved_role_entries.c`,
  `dcc_json_gateway_role_array_finish.c`,
  `dcc_json_gateway_role_array_items.c`,
  `dcc_json_gateway_resolved_role_maps.c`,
  `dcc_json_gateway_resolved_object_map_items.c`,
  `dcc_json_gateway_resolved_object_maps.c`,
  `dcc_json_gateway_resolved_messages.c`,
  `dcc_json_gateway_resolved_message_arrays.c`,
  `dcc_json_gateway_resolved_message_author_fields.c`,
  `dcc_json_gateway_resolved_message_entries.c`,
  `dcc_json_gateway_resolved_message_fields.c`,
  `dcc_json_gateway_resolved_message_id_fields.c`,
  `dcc_json_gateway_resolved_message_object_fields.c`,
  `dcc_json_gateway_resolved_message_objects.c`,
  `dcc_json_gateway_resolved_message_scalar_fields.c`,
  `dcc_json_gateway_resolved_message_text_fields.c`,
  `dcc_json_gateway_resolved_attachment_arrays.c`,
  `dcc_json_gateway_resolved_attachment_entries.c`,
  `dcc_json_gateway_resolved_attachment_ids.c`,
  `dcc_json_gateway_resolved_attachment_fields.c`,
  `dcc_json_gateway_resolved_attachment_media_fields.c`,
  `dcc_json_gateway_resolved_attachment_maps.c`,
  `dcc_json_gateway_resolved_attachment_numeric_fields.c`,
  `dcc_json_gateway_resolved_attachment_object_fields.c`,
  `dcc_json_gateway_resolved_attachment_string_fields.c`,
  `dcc_json_gateway_resolved_attachments.c`, `dcc_json_gateway_resolved_permissions.c`,
  `dcc_json_gateway_scheduled_event_array_finish.c`,
  `dcc_json_gateway_scheduled_event_array_items.c`,
  `dcc_json_gateway_scheduled_event_arrays.c`,
  `dcc_json_gateway_scheduled_event_buffers.c`,
  `dcc_json_gateway_scheduled_event_creator_fields.c`,
  `dcc_json_gateway_scheduled_event_ids.c`,
  `dcc_json_gateway_scheduled_event_identity_text_fields.c`,
  `dcc_json_gateway_scheduled_event_numeric_fields.c`,
  `dcc_json_gateway_scheduled_event_object_dispatch.c`,
  `dcc_json_gateway_scheduled_event_object_fields.c`,
  `dcc_json_gateway_scheduled_event_text_fields.c`,
  `dcc_json_gateway_scheduled_event_time_text_fields.c`,
  `dcc_json_gateway_scheduled_events.c`,
  `dcc_json_gateway_stage_instance_array_finish.c`,
  `dcc_json_gateway_stage_instance_array_items.c`,
  `dcc_json_gateway_stage_instance_buffers.c`,
  `dcc_json_gateway_stage_instance_id_fields.c`,
  `dcc_json_gateway_stage_instance_object_dispatch.c`,
  `dcc_json_gateway_stage_instance_object_fields.c`,
  `dcc_json_gateway_stage_instance_objects.c`,
  `dcc_json_gateway_stage_instance_scalar_fields.c`,
  `dcc_json_gateway_stage_instance_text_fields.c`,
  `dcc_json_gateway_stage_instances.c`,
  `dcc_json_gateway_presence.c`, `dcc_json_gateway_presence_list.c`,
  `dcc_json_gateway_presence_list_activity_fields.c`,
  `dcc_json_gateway_presence_list_arrays.c`,
  `dcc_json_gateway_presence_list_buffers.c`,
  `dcc_json_gateway_presence_list_entries.c`,
  `dcc_json_gateway_presence_list_fields.c`,
  `dcc_json_gateway_presence_list_identity_fields.c`,
  `dcc_json_gateway_presence_list_object_fields.c`,
  `dcc_json_gateway_presence_list_object_parse.c`,
  `dcc_json_gateway_presence_list_objects.c`, and
  `dcc_json_gateway_presence_list_status_fields.c`.
  Gateway JSON key classification keeps the public dispatcher in
  `dcc_json_keys.c`; range dispatch lives in `dcc_json_keys_short.c`,
  `dcc_json_keys_medium.c`, `dcc_json_keys_medium_tail.c`,
  `dcc_json_keys_long.c`, `dcc_json_keys_extended.c`, and
  `dcc_json_keys_extended_tail.c`; exact
  1- and 2-byte root keys stay in `dcc_json_keys_short.c`; 3-through-5
  byte classifiers live in `dcc_json_keys_len3.c`,
  `dcc_json_keys_len4.c`, and `dcc_json_keys_len5.c`; 6-through-15 byte
  classifiers live in `dcc_json_keys_len6.c` through
  `dcc_json_keys_len15.c`; 16-through-18 byte classifiers live in
  `dcc_json_keys_len16.c` through `dcc_json_keys_len18.c`.
- Interaction server create/destroy and public accessors live in
  `interaction_server.c`; error string handling lives in
  `interaction_server_error.c`; state snapshot filling lives in
  `interaction_server_state.c`; public start/stop/drain/wait lifecycle lives in
  `interaction_server_lifecycle.c`; route table storage management lives in
  `interaction_server_routes.c`, route lookup lives in
  `interaction_server_route_lookup.c`; signed interaction dispatch lives in
  `interaction_server_dispatch.c`; response copies/accounting/free helpers live in
  `interaction_server_response.c`; listener bind lives in
  `interaction_server_bind.c`, accept-loop spawning lives in
  `interaction_server_accept.c`, and per-client request tasks live in
  `interaction_server_client_task.c`; shared text comparison/copy helpers live
  in `interaction_server_text.c`, request read loops live in
  `interaction_server_http_request.c`, request buffer growth lives in
  `interaction_server_http_request_buffer.c`, request header scan/content-length
  parsing lives in `interaction_server_http_header_scan.c`, request-line and
  header storage parsing lives in `interaction_server_http_request_parser.c`, response serialization lives in
  `interaction_server_http_response.c`; Ed25519 signature verification lives in
  `interaction_server_crypto.c`; public server stats/state APIs live in
  `interaction_server_stats.c`; health snapshots live in
  `interaction_server_health.c`, health JSON/string helpers live in
  `interaction_server_health_json.c`, health route registration lives in
  `interaction_server_health_route.c`, health JSON string escaping lives in
  `interaction_server_json_escape.c`; request accessors and text/JSON replies
  live in `interaction_server_request_api.c`; data-less named callback replies
  live in `interaction_server_request_replies.c`; message/text callback replies
  live in `interaction_server_request_message_replies.c`; modal/autocomplete
  callback replies live in `interaction_server_request_builder_replies.c`. Internal interaction server contracts are
  split under `src/internal/interactions/` by limits, buffer, route table,
  server state, request state, HTTP IO, dispatch, runtime, crypto, and text/core
  helper concerns; `dcc_interaction_server_internal.h` remains only the
  compatibility aggregate.
- Gateway implementation keeps public lifecycle entry points in `gateway.c`;
  websocket control state lives in `gateway_ws_control.c`, resume state policy
  lives in `gateway_resume.c`, close-code reconnect policy lives in
  `gateway_close_policy.c`, gateway voice-state update sending lives in
  `gateway_voice_state_update.c`, session run orchestration lives in
  `gateway_session.c`, session receive loop lives in
  `gateway_session_receive.c`, HELLO parsing lives in
  `gateway_session_hello.c`, opcode handling lives in
  `gateway_session_opcode.c`, heartbeat work lives in
  `gateway_heartbeat.c`, identify-rate-limit handling lives in
  `gateway_identify.c`, reconnect orchestration lives in `gateway_runtime.c`,
  time/jitter helpers live in `gateway_timing.c`; `/gateway/bot`
  discovery request orchestration lives in `gateway_bot.c`, gateway URL
  construction lives in `gateway_bot_url.c`, and session-start-limit response
  application lives in `gateway_bot_response.c`,
  identify/resume payload builders live in `gateway_payload.c`, shared payload
  string-buffer helpers live in `gateway_payload_buffer.c`, heartbeat payload
  sending lives in `gateway_payload_heartbeat.c`,
  dispatch routing orchestration lives in `gateway_dispatch.c`, while session,
  object, resource, runtime, and miscellaneous dispatch groups live in
  `gateway_dispatch_session.c`, `gateway_dispatch_objects.c`,
  `gateway_dispatch_resources.c`, `gateway_dispatch_runtime.c`, and
  `gateway_dispatch_misc.c`; event construction and
  bus emission live in `gateway_emit_core.c`, `gateway_emit_session_events.c`,
  `gateway_emit_message_object.c`, `gateway_emit_guild_object.c`,
  `gateway_emit_channel_object.c`, `gateway_emit_role_object.c`,
  `gateway_emit_member_object.c`, `gateway_emit_user_object.c`,
  `gateway_emit_presence_events.c`, `gateway_emit_guild_resources.c`,
  `gateway_emit_misc_resources.c`, `gateway_emit_scheduled_resources.c`,
  `gateway_emit_commerce_resources.c`, `gateway_emit_invite_resources.c`,
  `gateway_emit_stage_resources.c`, `gateway_emit_soundboard_resources.c`,
  `gateway_emit_integration_resources.c`, `gateway_emit_message_runtime.c`,
  `gateway_emit_moderation_runtime.c`, `gateway_emit_thread_runtime.c`,
  `gateway_emit_voice_runtime.c`, and `gateway_emit_interactions.c`, cache mutation
  dispatch lives in `gateway_cache.c`; message, guild, channel/thread,
  member/role, and user/voice cache event handling lives in
  `gateway_cache_messages.c`, `gateway_cache_guilds.c`,
  `gateway_cache_channels.c`, `gateway_cache_members.c`, and
  `gateway_cache_user_voice.c`; gateway cache helper mutations live in
  `gateway_cache_helpers.c`, while runtime Discord gateway event-name
  lookup lives in `gateway_events.c` and its event-name table lives in
  `gateway_event_names.c`; JSON gateway event prefix/name comparison
  lives in `dcc_json_gateway_event_names.c`.
  Gateway internal declarations keep `dcc_gateway_internal.h` as a thin
  compatibility aggregate, while session state, timing, websocket control,
  resume policy, close-code policy, identify handling, heartbeat, session
  receive helpers, runtime orchestration, dispatch, cache helpers, lifecycle
  hooks, `/gateway/bot` helpers, gateway voice-state updates, and event emit groups live in focused headers under
  `src/internal/gateway/`. Basic gateway event emit declarations keep
  `dcc_gateway_emit_basic_internal.h` as a compatibility aggregate, while
  session and presence/typing emit declarations live in focused gateway emit
  headers. Object event emit declarations keep
  `dcc_gateway_emit_object_events_internal.h` as a compatibility aggregate,
  while message, guild, channel, role, member, and user object emit
  declarations live in focused object emit headers. Resource object emit declarations keep
  `dcc_gateway_emit_resource_objects_internal.h` as a compatibility aggregate,
  while scheduled-event, commerce, invite, stage-instance, soundboard, and
  integration emit declarations live in focused resource emit headers.
- WebSocket implementation keeps connect orchestration in `dcc_ws_connect.c`
  and retain/close/destroy lifecycle APIs in `dcc_ws.c`; URL parsing lives in
  `dcc_ws_url.c`; client handshake validation lives in
  `dcc_ws_handshake.c`; handshake buffer, key, and header helpers live in
  `dcc_ws_handshake_buffer.c`, `dcc_ws_handshake_crypto.c`, and
  `dcc_ws_handshake_headers.c`; exact-read and endian helpers live in `dcc_ws_io.c`;
  frame sending lives in `dcc_ws_send.c`; and frame receive/control handling
  lives in `dcc_ws_recv.c`. WebSocket internal declarations keep `dcc_ws.h` as
  the compatibility aggregate, while shared types, concrete connection state,
  URL parsing, IO/endian helpers, handshake helpers, frame send/receive APIs,
  and lifecycle APIs live in focused headers under `src/internal/ws/`.
- Cache implementation keeps lifecycle, counting, clearing, and cache policy
  APIs in `cache.c`; object put/get/remove APIs are split by object family
  across `cache_objects_guilds.c`, `cache_objects_channels.c`,
  `cache_objects_users.c`, `cache_objects_messages.c`, and
  `cache_objects_voice.c`; object merge APIs are split by object family across
  `cache_merge_objects_guilds.c`, `cache_merge_objects_channels.c`,
  `cache_merge_objects_users.c`, `cache_merge_objects_messages.c`, and
  `cache_merge_objects_voice.c`,
  hash-table put/get/remove primitives live in `cache_table.c`, resize/hash
  helpers live in `cache_table_resize.c`, lifecycle and trim helpers live in
  `cache_table_lifecycle.c`, sequence reseeding lives in
  `cache_table_sequence.c`, and partial Gateway
  update merge views are split by object family across `cache_merge_users.c`,
  `cache_merge_messages.c`, `cache_merge_channels.c`, `cache_merge_guilds.c`,
  and `cache_merge_voice.c`; guild view merge details are further split into
  identity, string, collection, limit, and flag helpers. Cache internal declarations keep
  `dcc_cache_internal.h` as a compatibility aggregate, while cache state,
  locking, value cleanup, enabled-state guards, table primitives, and merge-view
  declarations live in focused headers under `src/internal/cache/`.
- Hot reload implementation keeps create/destroy lifecycle in
  `hot_reload.c`; attach orchestration lives in `hot_reload_attach.c`; host-facing simple run orchestration lives in
  `hot_reload_run.c`; file signatures live in `hot_reload_file_sig.c`,
  temp-copy/path helpers live in `hot_reload_loader_file.c`, dynamic-library
  open/symbol/close lives in `hot_reload_loader.c`, module context helpers live in
  `hot_reload_module_context.c`, module init/deinit loading lives in
  `hot_reload_module_loader.c`, trampoline dispatch lives in
  `hot_reload_module_trampoline.c`, isolated worker child execution lives in
  `hot_reload_worker_child.c`, worker REST capture and result serialization
  live in `hot_reload_worker_child_capture.c` and
  `hot_reload_worker_child_result.c`, parent-side candidate promotion lives in
  `hot_reload_worker_parent.c`, parent-side event routing lives in
  `hot_reload_worker_dispatch.c`, including last-good retry and temporary
  parent-side interaction error fallback, active/last-good cleanup lives in
  `hot_reload_worker_lifecycle.c`, POSIX fork/exec plus socketpair launch lives
  in `hot_reload_worker_process_posix.c`, Windows `_spawnvp` plus `_pipe`
  launch lives in `hot_reload_worker_process_windows.c`, process stop/drain
  helpers split between `hot_reload_worker_process_lifecycle_posix.c` and
  `hot_reload_worker_process_lifecycle_windows.c`, worker readiness probing
  lives in `hot_reload_worker_process_ready.c`, worker event/result IPC lives
  in `hot_reload_worker_ipc.c`, parent-side bounded worker result decoding
  lives in `hot_reload_worker_result_read.c`, worker process supervision and
  idle IPC health probes live in `hot_reload_worker_supervisor.c`, worker protocol framing lives in
  `hot_reload_worker_protocol.c`, platform read/write protocol helpers live in
  `hot_reload_worker_protocol_posix.c` and
  `hot_reload_worker_protocol_windows.c`, condition-variable/runtime helpers live in
  `hot_reload_runtime.c`, persistent module state allocation lives in
  `hot_reload_state.c`, module replacement lives in `hot_reload_reload.c`,
  file-change settling lives in `hot_reload_poll.c`, watcher thread lifecycle
  lives in `hot_reload_watch.c`, generation waits live in
  `hot_reload_wait_generation.c`, health snapshots live in
  `hot_reload_health.c`, health JSON serialization lives in
  `hot_reload_health_json.c`, hot-reload health sidecar route binding lives in
  `hot_reload_sidecar_route.c`, health JSON buffer writing lives in
  `hot_reload_health_json_buffer.c`, and small state/accessor APIs live in
  `hot_reload_accessors.c`. Internal hot-reload declarations keep
  `dcc_hot_reload_internal.h` as the compatibility aggregate, while file
  signatures, dynamic-library loading, module state, module loading,
  worker protocol/process APIs, reload/poll/watch/wait APIs, health JSON buffer
  helpers, runtime/error helpers, persistent state allocation, and synchronization live in focused headers under
  `src/internal/hot_reload/`.
- Task group implementation keeps lifecycle in `task_group.c`; option/deadline
  mapping lives in `task_group_options.c`, state synchronization and snapshots
  live in `task_group_state.c`, entry bookkeeping and the LLAM trampoline live
  in `task_group_entries.c`, spawning lives in `task_group_spawn.c`,
  cancellation lives in `task_group_cancel.c`, basic join waiting lives in
  `task_group_wait.c`, wait-any completion tracking lives in
  `task_group_wait_any.c`, result snapshot waiting lives in
  `task_group_wait_result.c`, and cancel-then-wait composition lives in
  `task_group_wait_cancel.c`. Task group internal declarations keep
  `dcc_task_group_internal.h` as the compatibility aggregate, while concrete
  state, option/status helpers, synchronization/wait snapshots, entry
  bookkeeping, and cancellation helper declarations live in focused runtime
  headers.
- Cluster implementation keeps create/destroy and shard accessors in
  `cluster.c`; option normalization and identify delay planning live in
  `cluster_config.c`; listener fan-out and cluster health event capture live in
  `cluster_events.c`; gateway discovery and admission APIs live in
  `cluster_gateway.c`; cluster startup orchestration and initial identify
  budget checks live in `cluster_start.c`; stop/wait lifecycle lives in
  `cluster_lifecycle.c`; per-shard drain/resume/reconnect controls live in
  `cluster_shards.c`; operation status/reset APIs live in
  `cluster_operations.c`, operation-status JSON output lives in
  `cluster_operations_json.c`, sidecar status route binding lives in
  `cluster_sidecar_route.c`, while rolling reconnect planning and execution are
  split into `cluster_operations_plan.c` and `cluster_operations_run.c`;
  restart policy normalization and APIs live in `cluster_restart_policy.c`,
  supervisor policy/callback APIs, decision normalization, and decision status
  snapshots live in `cluster_supervisor_policy.c`, decision-event JSON output
  lives in `cluster_supervisor_decision_json.c`, and the shard supervisor task
  lives in `cluster_supervisor.c`; shard health snapshots and summaries live in
  `cluster_health.c`; health state classification lives in
  `cluster_health_state.c`; recovery-plan health wait supervision lives in
  `cluster_wait_health.c`, and READY admission waits live in
  `cluster_wait_ready.c`; recovery plan assembly lives in
  `cluster_recovery.c`, while shard recovery classifiers and recovery action
  helpers live in `cluster_recovery_action.c`; conservative recovery execution
  hooks live in `cluster_remediation.c`, and bounded automated remediation
  loops are split across `cluster_remediation_loop.c`,
  `cluster_remediation_loop_options.c`, `cluster_remediation_loop_delay.c`, and
  `cluster_remediation_loop_result.c`; health/recovery/remediation-loop JSON output lives in
  `cluster_health_json_summary.c`, `cluster_health_json_plan.c`,
  `cluster_health_json_result.c`, `cluster_health_json_loop_result.c`,
  `cluster_health_json_status.c`, and
  `cluster_json_buffer.c`; counter
  reset/rearm helpers live in `cluster_health_control.c`; and cluster status
  strings live in `cluster_strings.c`. Internal cluster declarations keep
  `dcc_cluster_internal.h` as the compatibility aggregate, while cluster state,
  health synchronization/helpers, identify-delay config, health event hooks,
  gateway admission, operation planning helpers, and supervisor hooks live in
  focused headers under `src/internal/client/`. Cluster health helper
  declarations keep `dcc_cluster_health_helpers_internal.h` as a compatibility
  aggregate, while text-copy, shard counter reset, and health-summary
  aggregation helpers live in focused cluster helper headers.
- Client implementation keeps create/destroy ownership in `client.c` and
  runtime start/stop/wait lifecycle in `client_lifecycle.c`; event emission
  lives in `client_events.c`, gateway control and
  gateway state snapshots live in `client_gateway_control.c`, DM channel cache
  helpers live in `client_dm_cache.c`, and shared status/error/string helpers
  live in `client_utils.c`. The concrete client object keeps
  `dcc_client_state_internal.h` as a compact compatibility aggregate, while
  gateway, config, REST, DM cache, voice registry, runtime, and lifecycle field
  groups live in focused `src/internal/client/dcc_client_*_state_fields_internal.h`
  headers.
- Voice implementation is split by responsibility: client lifecycle and small
  public accessors remain in `voice.c`, session connect/leave control lives in
  `voice_session.c`, Gateway voice session event application lives in
  `voice_session_events.c`, session state helpers and strings live in
  `voice_session_state.c`, session snapshots and stats live in
  `voice_session_stats.c`, connect-and-wait supervision lives in
  `voice_connect_wait.c`, reconnect policy lives in `voice_reconnect.c`, DAVE
  state accessors live in `voice_dave_state.c`, health strings live in
  `voice_health_strings.c`, health snapshots live in `voice_health_snapshot.c`,
  health JSON lives in `voice_health_json.c`,
  gateway text frame dispatch lives in `voice_gateway.c`, READY/session setup
  handling lives in `voice_gateway_ready.c`, voice client events live in
  `voice_gateway_client_events.c`, DAVE gateway state transitions live in
  `voice_gateway_dave_events.c`, gateway JSON helper parsing lives in
  `voice_gateway_helpers.c`, binary DAVE gateway frames live in
  `voice_gateway_binary.c`, websocket single-connection
  orchestration lives in `voice_ws.c`, initial identify/resume payload sending
  lives in `voice_ws_initial_payload.c`, websocket receive dispatch lives in
  `voice_ws_receive_loop.c`, websocket current-connection state lives in
  `voice_ws_state.c`, current websocket send helpers live in
  `voice_ws_send_current.c`, reconnect request handling lives in
  `voice_ws_reconnect.c`, stop-aware websocket sleeps live in
  `voice_ws_timing.c`, heartbeat handling lives in `voice_ws_heartbeat.c`,
  hello/select-protocol handshake logic lives in `voice_ws_handshake.c`,
  reconnect loop wrappers live in `voice_ws_loop.c`, managed websocket loop task
  execution lives in `voice_ws_loop_task.c`, managed websocket task
  start/stop/status APIs live in `voice_ws_task.c`, general voice utilities live
  in `voice_utils.c`, JSON emission helpers live in `voice_json.c`, DAVE MLS frame
  helpers live in `voice_dave.c`, DAVE client payload/frame builders live in
  `voice_dave_client_builders.c`, DAVE client handler/send helpers live in
  `voice_dave_client.c`, voice identify/resume/select-protocol payload builders
  live in `voice_payloads.c`, heartbeat/speaking payload builders live in
  `voice_payloads_control.c`, receive-sequence accessors live in
  `voice_payloads_sequence.c`,
  websocket URL assembly lives in `voice_url.c`, RTP AEAD wrapping lives in
  `voice_crypto.c`, HChaCha20 key derivation lives in `voice_hchacha20.c`,
  UDP socket setup lives in `voice_udp.c`, IP discovery
  packet build/parse helpers live in `voice_ip_discovery_packet.c`, IP discovery
  UDP round-trip handling lives in `voice_ip_discovery.c`,
  RTP/encryption state accessors live in `voice_rtp_state.c`, send queue
  primitives live in `voice_send_queue_core.c`, queued media enqueue helpers live
  in `voice_send_queue_media.c`, marker/inspection APIs live in
  `voice_send_queue_markers.c`, queued RTP packet drain lives in `voice_transport.c`,
  receive PCM/codec setters live in `voice_receive.c`, receive RTP entry points live in
  `voice_receive_rtp.c`,
  RTP decrypt/decode lives in `voice_receive_decode.c`, receive reorder and
  jitter drain lives in `voice_receive_reorder.c`, reorder slot helpers live in
  `voice_receive_reorder_slots.c`, sequence/jitter utilities live in
  `voice_receive_reorder_utils.c`, mixed PCM accumulation lives
  in `voice_receive_mix.c`, receive jitter timer lifecycle lives in
  `voice_receive_timer.c`, receive stat accessors live in
  `voice_receive_stats.c`, SSRC/user mapping lives in `voice_ssrc.c`, optional
  Opus public enable/disable APIs live in `voice_opus.c`, Opus encode/decode
  callbacks live in `voice_opus_codec.c`, Opus backend teardown lives in
  `voice_opus_lifecycle.c`, buffer/ready notification wrappers live in
  `voice_notify.c`, media notification wrappers live in `voice_notify_media.c`,
  client-state notification wrappers live in `voice_notify_client.c`, and owner
  registry code lives in `voice_registry.c`.
- Object clone/free ownership code is split by family: string/snowflake/user
  helpers live in `objects_common.c`, attachment field ownership lives in
  `objects_attachment_fields.c`, message field ownership lives in
  `objects_message_fields.c`, attachment/message public clone wrappers live in
  `objects_message.c`, channel ownership lives in `objects_channel_fields.c`
  and `objects_channel_clone.c`, role ownership lives in `objects_role_fields.c`
  and `objects_role_clone.c`, member ownership lives in
  `objects_member_fields.c` and `objects_member_clone.c`, thread-member field
  ownership lives in `objects_thread_member_fields.c`, interaction option lookup
  accessors live in `interaction_accessors.c`, typed interaction option value
  accessors live in `interaction_option_values.c`, interaction resolved user/member
  accessors live in `interaction_resolved_accessors.c`, resolved resource
  accessors live in `interaction_resolved_accessors_resources.c`, permission,
  integration-owner, and entitlement accessors live in
  `interaction_resolved_accessors_misc.c`, presence, emoji, and sticker field ownership
  live in `objects_presence_fields.c`, `objects_emoji_fields.c`, and
  `objects_sticker_fields.c`, guild aggregate entry points live in
  `objects_guild.c`, with string and collection ownership split into
  `objects_guild_strings.c`, `objects_guild_collections.c`, and
  `objects_guild_collections_free.c`,
  supplemental resource objects are split along the public resource headers:
  scheduled events in `objects_scheduled_events.c`, commerce objects in
  `objects_commerce.c`, invites in `objects_invites.c`, stage instances in
  `objects_stage_instances.c`, soundboard sounds in `objects_soundboard.c`,
  emoji clone wrappers in `objects_emojis.c`, and sticker clone wrappers in
  `objects_stickers.c`; interaction ownership entry points live in
  `objects_interactions.c`, interaction string/value/option/collection clone
  helpers live in `objects_interaction_clone_strings.c`,
  `objects_interaction_clone_values.c`,
  `objects_interaction_clone_options.c`, and
  `objects_interaction_clone_collections.c`, while option tree ownership lives in
  `objects_interaction_options.c` and resolved object clone/free ownership lives
  in `objects_interaction_resolved_clone.c`,
  `objects_interaction_resolved_clone_people.c`,
  `objects_interaction_resolved_clone_resources.c`,
  `objects_interaction_resolved_clone_files.c`, and
  `objects_interaction_resolved_free.c`; gateway raw event payload entry points live
  in `objects_gateway.c`, gateway string clone ownership lives in
  `objects_gateway_strings.c`, string reference clearing lives in
  `objects_gateway_strings_clear.c`, string freeing lives in
  `objects_gateway_strings_free.c`,
  collection clone/free orchestration lives in `objects_gateway_collections_clone.c`
  and `objects_gateway_collections_free.c`, and collection helpers live in
  `objects_gateway_collection_snowflakes.c`,
  `objects_gateway_collection_members.c`, and
  `objects_gateway_collection_activities.c`; typed Gateway
  event clone/free helpers are split into `objects_events_message_reaction.c`,
  `objects_events_message_delete.c`, `objects_events_poll_vote.c`,
  `objects_events_channel_updates.c`,
  `objects_events_moderation.c`, `objects_events_application_command_permissions.c`,
  `objects_events_guild_bans.c`, `objects_events_guild_members_chunk.c`,
  `objects_events_threads.c`, `objects_events_thread_list_sync.c`,
  voice channel effect ownership in `objects_events_voice.c`, voice channel
  update ownership in `objects_events_voice_channel.c`, voice server update
  ownership in `objects_events_voice_server.c`, voice state ownership in
  `objects_events_voice_state.c`, `objects_events_guild_updates.c`,
  `objects_events_guild_id_lists.c`, `objects_events_integrations.c`, and
  `objects_events_presence.c`, with
  `objects.c` and `objects_events.c` kept as module boundaries. Internal object
  declarations keep `dcc_objects_internal.h` as the compatibility aggregate,
  while clone helpers, core field ownership, resource field ownership,
  interaction ownership, and Gateway event data ownership live in focused
  headers under `src/internal/objects/`.
- Component builder public init/free helpers live in `component_builder_core.c`,
  public common/button setters live in `component_builder_setters.c`, select
  setters live in `component_builder_setters_select.c`, text-input setters live
  in `component_builder_setters_text_input.c`, state/children setters live in
  `component_builder_setters_state.c`, and build entry points live in
  `component_builder_build.c`; JSON buffer helpers, JSON string escaping,
  JSON member writers, component serialization, and validation are split into
  `component_json_buffer.c`, `component_json_escape.c`,
  `component_json_members.c`, `component_builder_serialize.c`,
  `component_builder_serialize_action_row.c`,
  `component_builder_serialize_button.c`, `component_builder_serialize_emoji.c`,
  `component_builder_serialize_select.c`,
  `component_builder_serialize_text_input.c`, and
  `component_builder_validate.c`. Component builder internal declarations keep
  `dcc_component_builder_internal.h` as the compatibility aggregate, while JSON
  buffer helpers, JSON member writers, serializer helpers, and validation
  helpers live in focused component internal headers.
- Message builder lifecycle lives in `message_builder.c`; shared JSON buffer
  helpers live in `message_json_buffer.c`, JSON string escaping lives in
  `message_json_escape.c`, JSON member writers live in `message_json_members.c`,
  poll top-level build/free lives in `message_builder_poll.c`, poll media serialization lives in
  `message_builder_poll_media.c`, poll validation lives in
  `message_builder_poll_validate.c`, public scalar setters live in
  `message_builder_setters.c`, collection/object setters live in
  `message_builder_setters_collections.c`, JSON passthrough setters live in
  `message_builder_setters_json.c`, and final message payload assembly
  orchestration lives in `message_builder_serialize.c`, basic message JSON
  members live in `message_builder_serialize_basic.c`, rich embed/component/poll
  members live in `message_builder_serialize_rich.c`, and message builder JSON
  validation lives in `message_builder_serialize_validate.c`. Message JSON declarations keep
  `dcc_message_json_internal.h` as the compatibility aggregate, while buffer
  primitives, member writers, and poll validation live in focused message
  internal headers.
- Modal builder lifecycle and setters live in `modal_builder.c`; modal JSON
  buffer, escaping, and member helpers live in `modal_json_buffer.c`,
  `modal_json_escape.c`, and `modal_json_members.c`, and final modal payload
  assembly lives in `modal_builder_serialize.c`.
- Embed builder public setters live in `embed_builder_setters.c`; shared JSON
  buffer, string escaping, member, and number helpers live in
  `embed_json_buffer.c`, `embed_json_escape.c`, `embed_json_members.c`, and
  `embed_json_numbers.c`; embed JSON build/free entry points live in
  `embed_builder_json.c`, top-level embed serialization lives in
  `embed_builder_serialize.c`, and author/field/footer/media serialization
  helpers live in `embed_builder_serialize_author.c`,
  `embed_builder_serialize_fields.c`, `embed_builder_serialize_footer.c`, and
  `embed_builder_serialize_media.c`; validation lives in
  `embed_builder_validate.c`.
- Application command builder public setters are split across
  `application_command_builder_setters.c`,
  `application_command_builder_payload_setters.c`, and
  `application_command_builder_permission_setters.c`; command JSON serialization/free
  entry points live in `application_command_builder_json.c`; top-level command
  member serialization lives in `application_command_builder_serialize.c`.
  Shared JSON helper code is split by role into
  `application_command_builder_buffer.c`,
  `application_command_builder_escape.c`, `application_command_builder_members.c`,
  and `application_command_builder_numbers.c`.
- Autocomplete builder public setters live in `autocomplete_builder_setters.c`;
  autocomplete response JSON serialization/free entry points live in
  `autocomplete_builder_json.c`. Shared JSON helper code is split by role into
  `autocomplete_json_buffer.c`, `autocomplete_json_escape.c`,
  `autocomplete_json_members.c`, and `autocomplete_json_numbers.c`.
The event module already follows this pattern: event bus lifecycle lives in
`src/events/events.c`, bus locking lives in `event_bus_lock.c`, listener
registration lives in `event_bus_listeners.c`, dispatch snapshots and callback
fan-out live in `event_bus_dispatch.c`, single-event waiter APIs live in
`event_waiters.c`, any-event waiter APIs live in `event_waiters_any.c`,
Gateway wait convenience helpers live in `event_waiter_gateway.c`, policy
dispatch lives in `event_waiter_policy.c`, common policy wait runtime lives in
`event_waiter_policy_runtime.c`, sequence/set policy wrappers live in
`event_waiter_policy_sequence.c`, count/quorum wrappers live in
`event_waiter_policy_count.c`, waiter matching and signal fan-out live in
`event_waiter_match.c`, waiter
snapshot/mask helpers live in `event_waiter_match_helpers.c`, waiter setup
validation lives in `event_waiter_match_setup.c`,
waiter-owned object clone helpers live in `event_waiter_owned.c`, owned object
free helpers live in `event_waiter_owned_free.c`, LLAM
wait-channel runtime helpers live in `event_waiter_runtime.c`, typed object
wait helper dispatch lives in `event_object_waiters.c`, single-event typed
object wait wrappers live in `event_object_waiters_single_core.c`,
`event_object_waiters_single_guild.c`, and
`event_object_waiters_single_voice.c`; any-of typed object wait wrappers live in
`event_object_waiters_any_core.c`, `event_object_waiters_any_guild.c`, and
`event_object_waiters_any_voice.c`; event type names
live in `event_types.c`, core event object views live in `event_accessors.c`,
message, guild core, guild resource/update, resource, channel/thread, voice,
interaction, and gateway-specific views live in the matching
`event_accessors_*.c` files, and
interaction listener public wrappers live in `interaction_listeners.c`, and
interaction listener registration lives in `interaction_listener_core.c`,
filter matching lives in `interaction_listener_match.c`, listener removal and
deinit live in `interaction_listener_remove.c`, and listener list state helpers
live in `interaction_listener_state.c`.

## Runtime

Gateway, REST, websocket, voice, interaction, and hot reload modules use LLAM as the runtime
integration boundary. DCC keeps that dependency in CMake as `llam::runtime`, so source modules do
not hard-code paths to `../LLAM`.

## Ownership

Gateway event payloads use scratch storage for hot-path parsing. User-retained objects must be
cloned with the public clone APIs, which deep-copy strings and owned arrays.

For public API ownership rules and operational use, see `api.md` and
`production-playbooks.md`.
