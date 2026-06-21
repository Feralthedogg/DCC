# DCC Public API Guide

DCC public headers are installed under `include/dcc/` and are included as
`<dcc/name.h>`. Applications may also include `<dcc/dcc.h>` for the aggregate
header.

## Sugar Header

`<dcc/sugar.h>` is an opt-in convenience layer for common struct literals and
option defaults. It does not replace the explicit builder APIs; use it where a
simple value can stay readable as one expression.

```c
#include <dcc/sugar.h>

dcc_application_command_builder_t ping =
    DCC_SLASH_COMMAND("ping", "Latency check");

dcc_message_builder_t message =
    DCC_MESSAGE_COMPONENTS_V2(
        DCC_V2_CONTAINER_ACCENT(
            0x5865F2,
            DCC_V2_SECTION(
                DCC_V2_BUTTON_PRIMARY("Refresh", "status.refresh"),
                DCC_V2_TEXT("# Runtime status")
            )
        )
    );
```

The header covers client options, intent sets, messages, embeds, legacy
components, Components v2, modals, slash/user/message commands, command
registration scope, component sessions, interaction flows, hot reload canary
options, REST firewall options, and replay records. Components v2 message sugar
sets `DCC_MESSAGE_FLAG_IS_COMPONENTS_V2` and intentionally avoids mixing v2
components with legacy `content` or `embeds`.

Variadic sugar macros hide short-lived arrays with C compound literals, so
message, embed, component, poll, select, and modal trees can be written inline.
Use the `*_ARRAY` variants when the data already lives in an explicit array and
you want to pass a pointer plus a count.

## Core Lifecycle

- `client.h`: client creation, options, start/wait/stop, gateway discovery,
  event waits, and typed object wait helpers.
- `cluster.h`: compatibility aggregate for multi-shard lifecycle, shard access,
  gateway admission and discovery, rolling operations, health summaries, fixed
  buffer health/recovery/remediation-loop/operation-status/cluster-status JSON,
  unsigned cluster-status sidecar route helpers, restart controls,
  drain/resume/reconnect operations, recovery plans, conservative remediation hooks and bounded loops,
  startup READY and recovery-plan health admission waits, supervisor decision
  callbacks/event JSON/status snapshots, policy-driven supervisor decisions, and listener fanout. Focused subheaders under
  `dcc/cluster/` separate base declarations,
  shared types, lifecycle, shard control, gateway helpers, rolling operations,
  health/recovery APIs, supervisor policy helpers, and listener registration.
  `dcc_cluster_wait_until_ready()` is the control-thread/supervisor helper for
  waiting until every shard reaches READY while returning early on close, stop,
  log-error, or error-shard state; `dcc_cluster_wait()` remains the blocking
  runtime shutdown wait. `dcc_cluster_wait_until_health()` waits for a target
  `dcc_cluster_recovery_plan()` health state with configurable timeout,
  interval, and critical-health fail-fast behavior, returning the last plan for
  admission logs or sidecar state. `dcc_cluster_set_supervisor_decision_callback()`
  lets a host mirror each supervisor decision into metrics or logs without
  parsing cluster state; `dcc_cluster_supervisor_decision_event_json()` turns
  that callback event into a fixed-buffer JSON log body. Callbacks run on the
  supervisor task and should stay short.
- `tasks.h`: LLAM-backed client task groups with fan-in waits and cancellation.
- `hot_reload.h`: compatibility aggregate for shared-library bot modules,
  short module export macros, env/simple/full host runners including isolated
  worker convenience runners, watcher state, generation and loaded waits,
  health snapshots, and backend selection between in-process loading and isolated worker mode.
  Focused subheaders under `dcc/hot_reload/` separate base declarations, module
  authoring helpers, run options, lifecycle controls, health snapshots, and
  state accessors. Module authoring declarations split further under
  `dcc/hot_reload/module/` for module types, context helpers, descriptor-based
  initialization, and export macros. `DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER`
  keeps Gateway/cache ownership in the parent and executes module handlers in a
  worker process selected by `dcc_hot_reload_options_t::worker_path`, or by the
  convenience-runner default `DCC_HOT_RELOAD_DEFAULT_WORKER`. The default CMake
  package build installs the `dcc_hot_reload_host` and `dcc_hot_reload_worker`
  helpers through `DCC_BUILD_TOOLS=ON`. Parent supervision uses LLAM tasks, and
  blocking worker IPC waits use LLAM blocking calls when running inside a
  managed task. Worker IPC writes and reads are bounded by the configured worker
  health timeout, so stalled parent/worker pipes report timeout instead of
  blocking Gateway ownership. Forwarded event payloads are capped, partial child
  EVENT bodies time out, and shutdown escalates after the drain window instead
  of waiting forever. When a worker dies while handling an interaction and last-good
  retry is unavailable or also fails, the parent responds with an ephemeral
  temporary interaction error; health snapshots include worker failure,
  promotion, dispatch failure, and temporary-response counters. The installed
  `dcc_hot_reload_host` defaults to `guilds` for
  slash-command bots, accepts `--guild` to set `DCC_HOT_RELOAD_GUILD_ID` for
  guild-scoped command development, accepts `--global` to clear an inherited
  guild scope, and accepts `--intents` or
  `DCC_HOT_RELOAD_INTENTS` for numeric or comma-separated named Gateway intents.
  It also loads `.env`, accepts `DISCORD_TOKEN` as a token fallback, and
  provides `--check` for no-network config validation of token presence, module
  readability, worker executable/PATH resolution, command scope, and optional
  health sidecar and timing settings. `--health-port` or
  `DCC_HOT_RELOAD_HEALTH_PORT` enables unsigned parent-process
  `GET /hot-reloadz`; `--health-address` defaults to `127.0.0.1`.
  `--poll-ms`, `--settle-ms`, `--worker-health-ms`, and `--worker-drain-ms`
  map to the hot-reload watcher and isolated-worker timeout options. Installed
  CMake packages also expose
  `dcc_add_hot_reload_module(<target> [SOURCES] ...)` so consumers can build
  reloadable MODULE libraries with DCC's prefix/link settings applied.
  Hot-reload health JSON includes the backend name, worker supervisor state,
  active/last-good/candidate worker generations and PIDs, canary routing
  counters, promotion/rollback counts, and rollback reason for isolated-worker
  deployment probes. `dcc_hot_reload_add_health_route()` exposes that body as
  an unsigned `GET /hot-reloadz` interaction-server sidecar route.

## Gateway Events

- `events.h`: compatibility aggregate for event types, payload structs,
  listener registration, typed event accessors, raw frame views, gateway
  payload access, and wait helpers. Focused subheaders under `dcc/events/`
  separate base declarations, event enums, payload views, accessors, listeners,
  and wait APIs. Event wait declarations split further under `dcc/events/wait/`
  for wait result/policy types, generic event waits, and owned object waits.
  Owned object wait declarations split further under
  `dcc/events/wait/owned_objects/` for single-type waits and any-of waits.
- `objects.h`: value objects returned by event accessors and clone/free helpers.
  The aggregate header includes focused subheaders under `dcc/objects/` for
  base objects, resources, gateway update payloads, interactions, generic
  gateway data, and clone/accessor APIs. Resource object declarations split
  further under `dcc/objects/resources/` for guilds, channels, roles, members,
  scheduled events, commerce, invites, stages, soundboard, emojis, and stickers.
  Generic gateway data declarations split further under `dcc/objects/gateway/`
  for voice state, activity, and normalized event-data objects.
  Gateway update payload declarations split further under
  `dcc/objects/gateway_updates/` for messages and polls, voice, moderation,
  threads, guilds, channels and presence, and application command permissions.
  Clone/free and interaction accessor declarations split further under
  `dcc/objects/clone/` for base objects, resources, gateway update objects,
  interactions, and generic gateway data.
- `cache.h`: optional client-owned cache for guilds, channels, roles, members,
  users, messages, and voice states.

Gateway event accessors return borrowed callback views unless the API name says
`wait` and returns an owned pointer. Use `dcc_*_clone()` when an object must
outlive the callback. For composed waits, use the
`dcc_event_wait_policy_init_*()` helpers before
`dcc_client_wait_for_event_policy()` so the policy size, mode, target count, and
timeout fields stay consistent. Use
`dcc_event_wait_policy_init_gateway_ready_or_resumed()` or
`dcc_client_wait_for_gateway_ready_or_resumed()` for the common Gateway
READY/RESUMED startup and reconnect wait. Use
`dcc_event_wait_policy_init_gateway_ready_resumed_or_close()` or
`dcc_client_wait_for_gateway_ready_resumed_or_close()` when the caller should
wake on `SOCKET_CLOSE` instead of waiting for a timeout after an early Gateway
disconnect. Use `dcc_event_wait_policy_init_interaction_or_close()` or
`dcc_client_wait_for_interaction_or_close()` for Gateway-side interaction
supervisors that should wake on typed interaction dispatches, raw
`INTERACTION_CREATE`, or `SOCKET_CLOSE`. The owned variant,
`dcc_client_wait_for_interaction_or_close_owned()`, returns an owned
`dcc_interaction_t` clone for interaction events and `DCC_OK` with a `NULL`
interaction pointer for `SOCKET_CLOSE`. Use
`dcc_client_wait_until_interaction()` or
`dcc_client_wait_until_interaction_owned()` when worker admission should report
`SOCKET_CLOSE` as `DCC_ERR_STATE` while keeping the close snapshot available.

## REST

- `rest.h`: low-level `dcc_rest_request()` plus DPP 10.1.5-mapped typed REST
  wrappers, params structs, multipart helpers, pagination helpers, and direct
  interaction response/followup helpers.
- `rest/core.h`: compatibility aggregate for low-level REST core helpers.
  Focused subheaders under `dcc/rest/core/` separate method names, direct
  request helpers, async queue controls, and future-based request helpers.
- `rest/types.h`: compatibility aggregate for REST params and enums. Focused
  subheaders under `dcc/rest/types/` mirror REST domains such as channels,
  guilds, messages, interactions, scheduled events, and webhooks.
- `rest/channels.h`: compatibility aggregate for channel REST helpers. Focused
  subheaders under `dcc/rest/channels/` separate channel lifecycle, guild
  channel positions, permission overwrites, channel actions, and channel invite
  operations.
- `rest/guilds.h`: compatibility aggregate for guild REST endpoints. Focused
  subheaders under `dcc/rest/guilds/` separate core guild operations, current
  member updates, audit logs, bans, integrations, widgets, prune, onboarding,
  welcome screen, auto moderation, and scheduled event APIs.
- `rest/guild_members.h`: compatibility aggregate for guild member REST
  helpers. Focused subheaders under `dcc/rest/guild_members/` separate member
  lookup/list/search, member lifecycle and voice/timeout updates, and member
  role operations.
- `rest/emojis_stickers.h`: compatibility aggregate for guild emoji, guild
  sticker, sticker lookup, sticker pack, and guild message search helpers.
  Focused subheaders under `dcc/rest/emojis_stickers/` separate guild emojis,
  guild stickers, global sticker lookups, and guild message search operations.
- `rest/resources.h`: compatibility aggregate for secondary REST resource
  groups. Focused subheaders under `dcc/rest/resources/` separate stage
  instances, templates, users and DM channel operations, direct-message helpers,
  group DMs, voice states, entitlements, gateway discovery, and invite APIs.
- `rest/messages.h`: compatibility aggregate for message REST helpers. Focused
  subheaders under `dcc/rest/messages/` separate message fetch, create, edit and
  delete, reaction, pin, and poll operations.
- `rest/threads.h`: compatibility aggregate for thread REST helpers. Focused
  subheaders under `dcc/rest/threads/` separate thread lifecycle, creation,
  membership, and archive/list operations.
- `rest/interactions.h`: compatibility aggregate for interaction REST helpers.
  Focused subheaders under `dcc/rest/interactions/` separate initial callback
  responses, named data-less callback helpers, original response operations, and
  followup message operations.
  Followup declarations split further under `dcc/rest/interactions/followups/`
  for token-based helpers and `dcc_interaction_t` convenience helpers.
- `rest/application_commands.h`: compatibility aggregate for application command
  REST helpers. Focused subheaders under `dcc/rest/application_commands/`
  separate application emojis, default-global create/bulk/delete-all
  registration with `dcc_application_command_registration_options_init()` and
  explicit `dcc_application_command_registration_options_set_global()` or
  `dcc_application_command_registration_options_set_guild()` scoping for global
  or one-guild rollout, global commands, guild commands, and guild command
  permission operations. Global and guild command declarations split
  further under `dcc/rest/application_commands/global_commands/` and
  `dcc/rest/application_commands/guild_commands/` for fetch, write, and bulk
  operations.
- `rest/webhooks.h`: compatibility aggregate for webhook REST helpers. Focused
  subheaders under `dcc/rest/webhooks/` separate webhook management, execution,
  and webhook message operations. Webhook management declarations split further
  under `dcc/rest/webhooks/management/` for create, fetch, modify, and delete
  operations. Webhook message declarations split further under
  `dcc/rest/webhooks/messages/` for fetch, edit, and delete operations.
- `message.h`, `embed.h`, `component.h`, `component_v2.h`, `modal.h`,
  `autocomplete.h`, and `application_command.h`: stack-friendly JSON builders
  for payloads used by REST and interaction responses. `component_v2.h`
  provides Components v2 layout, media, select-default, modal-input
  serializers, convenience initializers, and typed message component accessors;
  `message.h` sets `DCC_MESSAGE_FLAG_IS_COMPONENTS_V2` when v2 components are
  attached.
- `objects/clone/interactions.h`: command option accessors, resolved object
  lookups, and Components v2 modal submit helpers such as
  `dcc_interaction_form_value()` and `dcc_interaction_form_boolean()`.
- `objects/base.h`: gateway message objects preserve message `components` as
  raw `dcc_message_t::components_json` and project a typed
  `dcc_message_t::components` tree for Components v2 layouts.
- `component_session.h`: signed stateful UI sessions for legacy and Components
  v2 controls, including `dcc_component_session_button_v2()`.

REST futures are LLAM-backed. Completion callbacks run on DCC worker context;
callers that need synchronous behavior should use the wait helpers.

## Interactions

- `interaction_server.h`: shardless Discord interaction webhook server with
  Ed25519 verification, signed and unsigned routes, a built-in health route
  helper, named ping/deferred response helpers, message/modal/autocomplete
  builder callback replies, text/ephemeral/update message convenience replies,
  health snapshots, stats, and drain support. Focused subheaders under
  `dcc/interaction_server/` separate base declarations, route registration,
  lifecycle and drain controls, health and stats snapshots, request accessors
  and responses, and signature verification. Lifecycle helpers also include
  `dcc_interaction_server_wait_until_ready()` and
  `dcc_interaction_server_wait_until_drained()` for LLAM-backed operational
  probes.
- `events.h`: gateway-side named listener helpers for slash commands,
  autocomplete, context menus, buttons, select menus, modal submits, and
  gateway/interaction admission plus interaction-or-close wait helpers for
  supervisors.
- `objects.h`: fixed-buffer command option views, resolved object lookups,
  top-level interaction metadata, authorizing integration owner lookup, and
  entitlement lookup.

Top-level `dcc_interaction_t` metadata includes locale, guild locale, app
permissions, context, message id, version, attachment size limit,
`authorizing_integration_owners`, and `entitlements`.

## Voice

- `voice.h`: compatibility aggregate for voice client lifecycle, Gateway
  Opcode 4 connect/leave helpers, websocket loop management, UDP/RTP helpers,
  optional Opus backend, encryption/decryption helpers, receive jitter/loss
  tracking, health snapshots, connect-and-wait supervision, close-code reconnect
  policy, and DAVE/MLS frame bridge helpers. Focused subheaders under
  `dcc/voice/` separate base declarations, shared types, lifecycle, supervisor,
  DAVE, gateway, media, transport, and notification APIs. Shared voice type
  declarations split further under
  `dcc/voice/types/` for enums, session and health snapshots, codec callbacks,
  and DAVE MLS frame types.
  `dcc_voice_client_wait_until_ready()` and
  `dcc_voice_client_wait_until_active()` are control-thread/supervisor helpers
  for blocking a voice join until the session reaches READY or ACTIVE while
  returning early for disconnected, degraded, or terminated health.
  `dcc_voice_client_connect_and_wait_until_ready()` arms the descriptor waiter
  before sending Gateway Opcode 4, waits for the `VOICE_STATE_UPDATE` plus
  `VOICE_SERVER_UPDATE` pair, then waits for managed voice READY. Gateway-side
  helpers `dcc_event_wait_policy_init_voice_session_descriptor()` and
  `dcc_client_wait_for_voice_session_descriptor()` remain available when the
  join was sent elsewhere. The `dcc_live_voice_probe` example shows the public
  API order for live validation: Gateway READY, connect-and-wait READY, and
  optional voice ACTIVE wait.

DCC exposes DAVE transition and MLS frame plumbing for external MLS engines. Full
built-in MLS group crypto remains an explicit roadmap item.

## Ownership Rules

- Borrowed callback views: event accessors, `dcc_event_data()`, and raw frame
  pointers.
- Owned clones: `dcc_*_clone()` results and typed wait helper results.
- Stack builders: caller owns the builder struct and output buffer.
- REST responses: caller receives callback views from the HTTP layer unless a
  wrapper explicitly documents owned data.

## Compatibility Audits

CTest runs source audits when Python is available:

- `dcc_event_surface_audit`
- `dcc_gateway_fast_path_audit`
- `dcc_gateway_payload_field_audit`
- `dcc_rest_surface_audit`
- `dcc_rest_request_assertion_audit`
- `dcc_rest_field_smoke_audit`
- `dcc_project_layout_audit`
- `dcc_source_package_audit`
- `dcc_deploy_template_audit`

Run `tools/release_check.sh` before tagging a release.
