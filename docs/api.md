# DCC Public API Guide

DCC public headers are installed under `include/dcc/` and are included as
`<dcc/name.h>`. Applications may also include `<dcc/dcc.h>` for the aggregate
header.

## Sugar Header

`<dcc/sugar.h>` is the recommended first stop for application code. It provides
compact struct literals and option defaults over the same public types used by
the explicit builder APIs.

```c
#include <dcc/sugar.h>

dcc_application_command_builder_t ping =
    DCC_SLASH_COMMAND("ping", "Latency check");

dcc_message_builder_t message =
    DCC_MESSAGE_COMPONENTS_V2(
        DCC_V2_CONTAINER_ACCENT(
            DCC_COLOR_BLURPLE,
            DCC_V2_SECTION(
                DCC_V2_BUTTON_PRIMARY("Refresh", "status.refresh"),
                DCC_V2_TEXT("# Runtime status")
            )
        )
    );
```

For app routes, prefer the role-specific aliases:
`dcc_command_route_t`, `dcc_slash_route_t`, `dcc_subcommand_route_t`,
`dcc_autocomplete_route_t`, `dcc_modal_route_t`, `dcc_button_route_t`,
`dcc_select_route_t`, and `dcc_modal_flow_t`. These names are the
sugar-first public surface; the lower-level `dcc_app_typed_*` structs remain
compatible implementation types for existing code.

The header covers client options, intent sets, messages, embeds, legacy
components, Components v2, modals, slash/user/message commands, command
registration scope, command option builders and choices, struct field bindings, form bindings,
struct field validators, validation result replies, app views,
typed slash/subcommand/autocomplete/modal routes, typed modal flow declarations,
role-specific route aliases,
single-source `DCC_COMMAND_ROUTE*`, no-option `DCC_COMMAND_ROUTE_NO_OPTIONS*`, `DCC_SUBCOMMAND_ROUTE*`,
`DCC_AUTOCOMPLETE_PARAMS*`, `DCC_MODAL_PARAMS*`,
`DCC_BUTTON_PARAMS*`, and `DCC_SELECT_PARAMS*` declarations, including
function-name params aliases such as `DCC_COMMAND_PARAMS_FN()`,
`DCC_COMMAND_ROUTE_FN_DATA()`, `DCC_AUTOCOMPLETE_PARAMS_FN_DATA()`,
`DCC_BUTTON_NS_PARAMS_FN()`, `DCC_SELECT_NS_PARAMS_FN()`, and
`DCC_MODAL_NS_PARAMS_FN()`, direct route registration aliases such as
`DCC_APP_REGISTER_COMMAND()`, `DCC_APP_REGISTER_SUBCOMMAND()`,
`DCC_APP_REGISTER_AUTOCOMPLETE()`, `DCC_APP_REGISTER_BUTTON()`,
`DCC_APP_REGISTER_SELECT()`, `DCC_APP_REGISTER_MODAL()`, and
`DCC_APP_REGISTER_MODAL_FLOW()`,
route-list aliases such as
`DCC_APP_COMMAND_ROUTES()`, `DCC_APP_BUTTON_ROUTES()`,
`DCC_APP_SELECT_ROUTES()`, and `DCC_APP_MODAL_ROUTES()`, direct typed
registration aliases such as `DCC_APP_TYPED_SLASH()`, `DCC_APP_SLASH_ROUTE_FN_DATA()`,
`DCC_APP_AUTOCOMPLETE_PARAMS_FN_DATA()`,
`DCC_APP_MODAL_V2_FLOW_NS_PARAMS_FN_DATA()`, and
`DCC_APP_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED()`,
role-specific handler declaration aliases such as `DCC_SLASH_FN()`,
`DCC_BUTTON_FN()`, `DCC_SELECT_FN()`, and `DCC_MODAL_FN()`,
handler wrapper aliases such as `DCC_COMMAND_IMPL()`,
`DCC_COMMAND_ARGS_IMPL()`, `DCC_SUBCOMMAND_ARGS_IMPL()`,
`DCC_BUTTON_ARGS_IMPL()`, `DCC_SELECT_ARGS_IMPL()`, `DCC_MODAL_ARGS_IMPL()`,
`DCC_AUTOCOMPLETE_ARGS_IMPL()`, and mutable aliases such as
`DCC_BUTTON_ARGS_MUT_IMPL()`, plus compatible `DCC_TYPED_*_IMPL()`
and `DCC_TYPED_*_MUT_IMPL()` variants,
function-name route aliases such as `DCC_ON_SLASH_FN()`,
`DCC_BUTTON_FN()`, `DCC_SELECT_FN()`, `DCC_MODAL_FN()`,
namespace function-name component aliases such as `DCC_BUTTON_NS_FN()`,
`DCC_SELECT_NS_FN()`, and `DCC_MODAL_NS_FN()`, decorator-style static
route descriptors such as `DCC_DECORATE_SLASH()`, `DCC_DECORATE_BUTTON_NS()`,
`DCC_DECORATE_READY_ONCE()`, and `DCC_ROUTE()`, and direct registration
aliases such as `DCC_APP_ON_SLASH_FN()`, `DCC_APP_ON_BUTTON_FN()`, and
`DCC_APP_ON_BUTTON_NS_FN()`,
readable form/component binding aliases such as `DCC_FORM_REQUIRED_TEXT()`,
`DCC_FORM_CHECKBOX()`, `DCC_COMPONENT_REQUIRED_ACTION()`, and
`DCC_COMPONENT_REQUIRED_VALUE()`,
inline `DCC_ON_TYPED_SLASH(...)`/`DCC_ON_TYPED_SLASH_DATA(...)` command definitions,
autocomplete choices, declarative app features and route/event/task
setup helpers including `DCC_DEFINE_PUBLIC_FEATURE`, `DCC_DECLARE_FEATURE`,
`DCC_APP_USE`, `DCC_APP_FEATURES`, `DCC_USE_FEATURE`, `DCC_BOT_USE`, `DCC_BOT_FEATURES`,
`DCC_FEATURE_MODULE`, `DCC_LOAD_FEATURE`, and the compatible Cog names
`DCC_DEFINE_COG`, `DCC_DEFINE_PUBLIC_COG`, `DCC_DECLARE_COG`, `DCC_COG_MODULE`,
`DCC_LOAD_COG`, route/event/task
aliases including `DCC_ON_*` and direct `DCC_APP_ON_*` registration helpers,
bot definition aliases such as `DCC_SIMPLE_BOT()`, `DCC_BOT()`,
`DCC_BOT_LISTENERS()`, `DCC_BOT_ROUTES()`, `DCC_GUILD_BOT()`, and
`DCC_GUILD_BOT_ROUTES()`, run helpers such as `DCC_RUN_SIMPLE_BOT()`,
`DCC_RUN_BOT()`, and `DCC_RUN_BOT_ROUTES()` plus `_TOKEN`/`_ENV` variants for
custom token environment names, and one-file entrypoint aliases such as
`DCC_SIMPLE_BOT_MAIN()`, `DCC_BOT_ROUTES_MAIN()`, `DCC_BOT_MAIN()`, and
`DCC_GUILD_BOT_ROUTES_MAIN()`,
Cog/app context menu routes including `DCC_COG_CONTEXT_MENUS()`,
`DCC_ON_USER_CONTEXT_MENU()`, `DCC_ON_MESSAGE_CONTEXT_MENU()`,
`DCC_APP_ON_USER_CONTEXT_MENU()`, and `DCC_APP_ON_MESSAGE_CONTEXT_MENU()`,
inline guarded route aliases such as `DCC_ON_SLASH_GUARDED()` and
`DCC_BUTTON_GUARDED()`, direct guarded registration aliases such as
`DCC_APP_ON_SLASH_GUARDED()` and `DCC_APP_ON_BUTTON_GUARDED()`,
component namespace route aliases such as `DCC_COMPONENT_ID()`,
`DCC_BUTTON_NS()`, `DCC_MODAL_NS_PREFIX()`, and
`DCC_APP_ON_BUTTON_NS_DATA_POLICY()`,
namespace-aware component builders such as `DCC_UI_PRIMARY_NS()`,
`DCC_UI_STRING_SELECT_NS()`, and `DCC_V2_TEXT_INPUT_PLACEHOLDER_NS()`,
namespace-aware params route aliases such as `DCC_BUTTON_NS_PARAMS()` and
`DCC_SELECT_NS_PARAMS()`, compatible lower-level typed route aliases such as
`DCC_TYPED_BUTTON_NS()`, and single-source params aliases such as `DCC_MODAL_NS_PARAMS()`,
`DCC_BUTTON_NS_PREFIX_PARAMS()`, `DCC_MODAL_V2_FLOW_NS_PARAMS()`, and
`DCC_MODAL_V2_FLOW_NS_PARAMS_FN()`,
namespace-aware persistent View helpers such as `DCC_VIEW_BUTTON_NS()` and
`DCC_VIEW_ACTION_PRIMARY_NS()`,
direct `DCC_APP_ON_*_POLICY` route registration helpers, typed routes inside cogs,
component sessions, declarative app definitions, direct interaction replies,
app-wide middleware and guard definitions, app-level error handler sugar such as
`DCC_APP_ERROR_HANDLER()`, `DCC_FRIENDLY_ERROR_FN()`,
`DCC_SIMPLE_ERROR_FN()`, `DCC_VERBOSE_ERROR_FN()`, context shortcut aliases,
message prefix command aliases such as `DCC_PREFIX_COMMAND_FN()`,
`DCC_PREFIX_COMMAND()`, and `DCC_APP_ON_PREFIX_COMMAND()`,
route group aliases including `DCC_GROUP_*` and `DCC_GROUP_POLICY`,
handler reply/update/followup/modal sugar including one-call Components v2 and modal display helpers,
confirm/paginator/settings panel component presets,
context-first reply aliases such as `DCC_CTX_REPLY_TEXT`,
`DCC_CTX_EPHEMERAL`, `DCC_CTX_FOLLOWUP_V2`, and `DCC_CTX_SHOW_MODAL_V2`,
short Components v2 UI aliases such as `DCC_UI_TEXT`, `DCC_UI_CARD_ACCENT`,
`DCC_UI_ROW`, and `DCC_CTX_REPLY_UI`,
button action view sugar such as `DCC_VIEW_ACTION_PRIMARY()`,
`DCC_VIEW_ACTION_ROW()`, `DCC_VIEW_ACTION_ROUTES_DATA()`, and
`DCC_APP_USE_VIEW_ACTIONS()`,
standard status replies such as `DCC_ACK`, `DCC_DENY`, `DCC_BAD_INPUT`,
`DCC_NOT_FOUND`, `DCC_BUSY`, and `DCC_INTERNAL_ERROR`,
safe send/reply helpers such as `DCC_REPLY_SAFE()` and `DCC_APP_SEND_SAFE()`,
status message builders such as `DCC_MESSAGE_SUCCESS()` and `DCC_MESSAGE_ERROR()`,
message flag helpers such as `DCC_MESSAGE_PRIVATE()`, `DCC_MESSAGE_SILENT()`,
`DCC_MESSAGE_NO_EMBEDS()`, and `DCC_MESSAGE_TEXT_FLAGS()`,
context reply aliases such as `DCC_CTX_PRIVATE()`, `DCC_CTX_SILENT()`, and
`DCC_CTX_NO_EMBEDS()`,
respond aliases such as `DCC_RESPOND_TEXT()`, `DCC_RESPOND_PRIVATE()`,
`DCC_RESPOND_OR_EDIT_TEXT()`, `DCC_RESPOND_OR_EDIT_UI()`,
`DCC_RESPOND_OR_FOLLOWUP_TEXT()`, `DCC_RESPOND_PRIVATE_EMBED()`, `DCC_RESPOND_UI()`,
`DCC_RESPOND_PRIVATE_UI()`, and `DCC_RESPOND_DEFER_PRIVATE()`,
private defer/embed/flow aliases such as `DCC_DEFER_PRIVATE()`,
`DCC_REPLY_PRIVATE_EMBED()`, and `DCC_FLOW_DEFER_PRIVATE()`,
app option presets and auto-defer aliases such as `DCC_APP_OPTIONS_DEV()`,
`DCC_APP_OPTIONS_DEV_ALL()`, `DCC_APP_OPTIONS_DEV_GUILD_ALL()`,
`DCC_CLIENT_OPTIONS_ALL()`, `DCC_APP_AUTO_DEFER_PRIVATE()`, and
`DCC_APP_OPTIONS_AUTO_DEFER_PRIVATE_DEFAULT()`,
command builder aliases such as `DCC_CMD()`, `DCC_CMD_OPTIONS()`,
`DCC_CMD_TREE()`, `DCC_CMD_SUBGROUP()`, `DCC_CMD_SUB()`,
listener-table command schema registration through `DCC_LISTEN_COMMAND_SCHEMA()`
and the compatible `DCC_ROUTE_COMMAND_SCHEMA()`,
and grouped-subcommand path helpers such as `DCC_SUB_PATH()` and
`DCC_SUBCOMMAND_ROUTE_IN_DATA()`,
slash argument aliases, modal field aliases, select value aliases such as
`DCC_CTX_ARG_STRING_OR()`, `DCC_CTX_FIELD_FIRST_VALUE_OR()`, and
`DCC_CTX_SELECT_FIRST()`,
component action helpers such as `DCC_CTX_COMPONENT_ACTION()` and
`DCC_CTX_COMPONENT_IS()`, with declaration guards such as
`DCC_REQUIRED_ACTION()`, `DCC_REQUIRED_COMPONENT_ACTION()`,
`DCC_CTX_REQUIRE_ACTION()`, and `DCC_CTX_REQUIRE_COMPONENT_ACTION()`,
handler-local default guards such as `DCC_REQUIRED_GUILD()`,
`DCC_REQUIRED_PERMISSIONS()`, `DCC_REQUIRED_ARG_TEXT()`, and
`DCC_REQUIRED_CUSTOM_ID_PREFIX()`, plus explicit-copy
`DCC_CTX_REQUIRE_*` guards with `DCC_REQUIRE_*` aliases,
status-embed guard aliases such as `DCC_CTX_REQUIRE_BAD_INPUT()`,
`DCC_CTX_REQUIRE_DENY()`, `DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT()`, and
required-option declaration helpers such as `DCC_REQUIRED_STRING_OPTION()`,
`DCC_REQUIRED_USER_OPTION()`, and `DCC_CTX_REQUIRE_STRING_OPTION()`, plus
required modal field helpers such as `DCC_REQUIRED_TEXT_FIELD()`,
`DCC_REQUIRED_CHECKBOX_FIELD()`, and `DCC_CTX_REQUIRE_TEXT_FIELD()`, and
required select value helpers such as `DCC_REQUIRED_SELECT_FIRST()`,
`DCC_REQUIRED_SELECT_VALUES()`, and `DCC_CTX_REQUIRE_SELECT_FIRST()`,
variadic role checks such as `DCC_CTX_HAS_ANY_ROLE_IDS()` and
`DCC_CTX_REQUIRE_ANY_ROLE_IDS()`,
handler-local bind exits such as `DCC_CTX_BIND_OPTIONS_OR_REPLY()`,
`DCC_CTX_BIND_OPTION_FIELDS_OR_REPLY()`, `DCC_CTX_BIND_FORM_OR_REPLY()`, and
`DCC_CTX_BIND_FORM_FIELDS_OR_REPLY()`,
handler-local `DCC_CTX_VALIDATE_OR_REPLY()` validation exits,
handler-local status guards such as `DCC_CTX_TRY()` and `DCC_CTX_TRY_MSG()`,
strict `DCC_PARSE_DATE*` and `DCC_PARSE_BIRTHDAY()` helpers,
handler option compatibility aliases such as `DCC_OPTION_STRING_OR` and
`DCC_OPTION_USER`,
typed allowed mention builders such as `DCC_MESSAGE_NO_MENTIONS()` and
`DCC_ALLOWED_MENTIONS_USERS(...)`,
Discord text formatting helpers such as `DCC_MENTION_USER()` and
`DCC_MENTION_COMMAND()`/`DCC_EMOJI()`/`DCC_TIMESTAMP_RELATIVE()`, plus
context-first aliases such as `DCC_CTX_USER_MENTION()` and
`DCC_CTX_MESSAGE_LINK()`, and target/option aliases such as
`DCC_CTX_TARGET_USER_MENTION()` and `DCC_CTX_ARG_ROLE_MENTION()`,
display-name and asset aliases such as `DCC_CTX_AUTHOR_DISPLAY_NAME()`,
`DCC_CTX_ARG_USER_DISPLAY_NAME()`, `DCC_CTX_GUILD_NAME()`, and
`DCC_CTX_AUTHOR_AVATAR_URL()`,
context REST shortcuts such as `DCC_CTX_THREAD_FROM_MESSAGE()`,
`DCC_CTX_ARCHIVE_THREAD()`, and `DCC_CTX_LOCK_THREAD()`,
short handler-local context aliases such as `DCC_USER()`, `DCC_MEMBER()`,
`DCC_CHANNEL()`, `DCC_USER_ID()`, `DCC_CUSTOM_ID()`, and `DCC_ACTION()`,
object-oriented REST shortcut aliases such as `DCC_CHANNEL_SEND_TEXT()`,
`DCC_CHANNEL_SEND_UI()`, `DCC_MESSAGE_EDIT_TEXT()`,
`DCC_MESSAGE_DELETE()`, `DCC_MESSAGE_THREAD()`, and `DCC_THREAD_LOCK()`,
webhook aliases such as `DCC_WEBHOOK_SEND_TEXT()`, `DCC_WEBHOOK_SEND()`,
`DCC_WEBHOOK_SEND_UI()`, `DCC_WEBHOOK_MESSAGE_EDIT()`, and
`DCC_WEBHOOK_MESSAGE_DELETE()`,
user/resource aliases such as `DCC_ME_FETCH()`, `DCC_USER_FETCH()`,
`DCC_USER_DM_SEND_TEXT()`, `DCC_GUILD_INVITES_FETCH()`,
`DCC_GROUP_DM_RECIPIENT_ADD_PARAMS()`, `DCC_STAGE_CREATE_PARAMS()`, and
`DCC_GUILD_TEMPLATE_SYNC()`,
guild operations such as `DCC_GUILD_AUDIT_LOG_FETCH_PAGE()`,
`DCC_GUILD_WIDGET_EDIT_PARAMS()`, `DCC_GUILD_ONBOARDING_EDIT_PARAMS()`,
`DCC_GUILD_WELCOME_SCREEN_EDIT_PARAMS()`, and `DCC_AUTO_MOD_RULE_CREATE_PARAMS()`,
emoji/sticker aliases such as `DCC_GUILD_EMOJI_CREATE_PARAMS()`,
`DCC_GUILD_STICKER_CREATE_PARAMS()`, `DCC_STICKER_PACKS_FETCH()`, and
`DCC_GUILD_MESSAGES_SEARCH_PARAMS()`,
gateway/commerce/voice aliases such as `DCC_GATEWAY_BOT_FETCH()`,
`DCC_USER_VOICE_STATE_EDIT_PARAMS()`, `DCC_ENTITLEMENTS_FETCH_PAGE()`, and
`DCC_ENTITLEMENT_CONSUME()`,
wait/collector aliases such as `DCC_WAIT_FOR_BUTTON()`,
`DCC_WAIT_FOR_MODAL_PREFIX()`, `DCC_COLLECT_BUTTON()`,
`DCC_APP_WAIT_FOR_BUTTON()`, and `DCC_APP_WAIT_FOR_COMPONENT_FILTERED()`,
formatted response helpers such as `DCC_CTX_REPLY_TEXT_F()`,
`DCC_CTX_PRIVATE_F()`, `DCC_CTX_FOLLOWUP_PRIVATE_F()`,
`DCC_CTX_UPDATE_TEXT_F()`, `DCC_CTX_EDIT_TEXT_F()`, `DCC_CTX_DONE_F()`,
`DCC_CTX_BAD_INPUT_F()`, `DCC_APP_SEND_TEXT_F()`, `DCC_CHANNEL_SEND_TEXT_F()`,
`DCC_APP_EDIT_TEXT_F()`, `DCC_MESSAGE_EDIT_TEXT_F()`,
and `DCC_FORMAT_ALLOC()`,
Discord markdown helpers such as `DCC_MD_BOLD()` and `DCC_MD_CODE_BLOCK()`,
Discord CDN URL helpers such as `DCC_CDN_USER_AVATAR()` and
`DCC_CDN_MEMBER_AVATAR()`,
Discord color helpers such as `DCC_COLOR_RGB()`, `DCC_COLOR_FROM_HEX()`, and
`DCC_COLOR_TO_HEX()`,
duration helpers such as `DCC_SECONDS()`, `DCC_MINUTES()`, and
`DCC_TIMEOUT_SECONDS_CHECKED()`,
raw id and Discord mention parsers such as `DCC_PARSE_ID()` and
`DCC_PARSE_CHANNEL_MENTION()`,
snowflake creation-time helpers such as `DCC_SNOWFLAKE_RELATIVE()`,
message link helpers such as `DCC_MESSAGE_LINK()` and
`DCC_PARSE_MESSAGE_LINK()`,
permission helpers such as `DCC_PERMISSIONS()`, `DCC_PERMISSION_TEXT_RICH`,
`DCC_CTX_CAN_ANY_PERMISSION()`, and `DCC_CTX_REQUIRE_CAN_ANY()`,
one-call app run helpers, interaction flows, `DCC_CTX_STORE_*` app-store sugar,
`DCC_CTX_REQUIRE_STORE()` handler guards,
friendly status aliases such as `DCC_REPLY_OK()`, `DCC_CTX_WARN()`,
`DCC_MESSAGE_INFO()`, `DCC_APP_PUBLISH_LATEST_STORE()` managed panels,
poll message helpers such as `DCC_POLL_YES_NO()` and `DCC_MESSAGE_TEXT_POLL()`,
hot reload canary options, REST firewall options, typed REST response ID extraction, and replay records.
It also includes typed env/config helpers such as `DCC_ENV_TOKEN`,
`DCC_CONFIG_CHANNEL`, `DCC_CONFIG_BOOL_DEFAULT`, `DCC_ENV_U32_RANGE_OR`, and
one-off `DCC_ENV_CHANNEL` reads, so application startup code does not need
scattered `getenv()` parsing.
Channel, role, and user config helpers accept raw IDs or the matching Discord
mention while guild and generic snowflake helpers stay raw-ID only.
Guard sugar includes `DCC_GUARDS(...)`/`DCC_POLICY(...)` with
guild-only/DM-only/NSFW-only guards, channel-type guards, owner checks, role
guards, predicate checks, plus per-user, per-channel, per-guild, and global
cooldowns for discord.py-style declarations. Use short decorator aliases such
as `DCC_ONLY_GUILD()`, `DCC_CAN(...)`, `DCC_HAS_ANY_ROLE(...)`, and
`DCC_RATE_LIMIT_USER(...)` with `DCC_ON_*_GUARDED(...)` route declarations when
you want policy inline.
Components v2 message sugar sets `DCC_MESSAGE_FLAG_IS_COMPONENTS_V2` and
intentionally avoids mixing v2 components with legacy `content` or `embeds`.

Variadic sugar macros hide short-lived arrays with C compound literals, so
message, embed, component, poll, select, and modal trees can be written inline.
Use the `*_ARRAY` variants when the data already lives in an explicit array and
you want to pass a pointer plus a count.

The explicit builder APIs remain the lower-level escape hatch. Use them when a
value is generated dynamically, when each setter needs error handling, or when
the data must outlive the current C block. The documentation shows sugar first
and calls out explicit APIs only where that extra control matters.

## Core Lifecycle

- `app.h`: discord.js/discord.py-style high-level application runtime with
  slash/component/modal/autocomplete routing, app-level Gateway event hooks,
  message prefix command routing,
  `dcc_app_send*()`, edit/delete message helpers, thread helpers, and member role helpers,
  object-oriented sugar aliases such as `DCC_CHANNEL_SEND_TEXT()`,
  `DCC_CHANNEL_FETCH()`, `DCC_CHANNEL_TYPING()`, `DCC_CHANNEL_DELETE()`,
  `DCC_CHANNEL_INVITE_CREATE_PARAMS()`, `DCC_CHANNEL_PERMISSION_EDIT_PARAMS()`,
  `DCC_GUILD_CHANNELS_REORDER_PARAMS()`,
  `DCC_MESSAGE_EDIT_TEXT()`, `DCC_MESSAGE_PIN()`, `DCC_MESSAGE_REACT()`,
  `DCC_MESSAGE_CROSSPOST()`, `DCC_MEMBER_ADD_ROLE()`, `DCC_MEMBER_TIMEOUT()`,
  `DCC_MEMBER_KICK()`, `DCC_GUILD_ROLE_CREATE_PARAMS()`, `DCC_ROLE_DELETE()`,
  `DCC_MEMBER_BAN()`, `DCC_MEMBER_UNBAN()`, `DCC_ME_SET_NICK()`,
  `DCC_GUILD_PRUNE_COUNT()`, `DCC_SCHEDULED_EVENT_CREATE_PARAMS()`,
  `DCC_WEBHOOK_SEND_TEXT()`, `DCC_WEBHOOK_SEND_UI()`,
  `DCC_USER_DM_SEND_TEXT()`, `DCC_INVITE_FETCH_FULL()`,
  `DCC_STAGE_CREATE_PARAMS()`, `DCC_GUILD_TEMPLATE_CREATE_PARAMS()`, and
  `DCC_GUILD_AUDIT_LOG_FETCH_PAGE()`, `DCC_GUILD_WIDGET_EDIT_PARAMS()`,
  `DCC_GUILD_ONBOARDING_EDIT_PARAMS()`, `DCC_GUILD_WELCOME_SCREEN_EDIT_PARAMS()`,
  `DCC_AUTO_MOD_RULE_CREATE_PARAMS()`, and
  `DCC_GUILD_EMOJI_CREATE_PARAMS()`, `DCC_GUILD_STICKER_CREATE_PARAMS()`,
  `DCC_GUILD_MESSAGES_SEARCH_PARAMS()`, and
  `DCC_GATEWAY_BOT_FETCH()`, `DCC_GROUP_DM_RECIPIENT_ADD_PARAMS()`,
  `DCC_USER_VOICE_STATE_EDIT_PARAMS()`, `DCC_ENTITLEMENTS_FETCH_PAGE()`,
  `DCC_ENTITLEMENT_CONSUME()`, and
  `DCC_AUTHOR_ADD_ROLE()`,
  app-level managed latest-message publishing, app-level waiter wrappers,
  filtered app waiters, context-scoped button/select/modal/message waits,
  `dcc_app_load_dotenv()`/env-based option loading, subcommand routing, app
  views, declarative extension descriptors for Cog-style feature files,
  prefixed route groups with shared middleware and guards, declarative route
  policies, route-scoped middleware, predicate checks, guild/DM/NSFW/channel/role guards, and cooldowns, command/form/component variable and struct-field
  binding helpers, command registry ownership, READY-time command sync, typed
  app state, Cog-like app modules, declarative app definitions,
  required environment validation, typed env/config binding helpers, one-shot READY handlers,
  top-level `DCC_APP_EVENTS(...)`, `DCC_REQUIRE_ENV(...)`,
  `DCC_ON_READY_ONCE(...)`, `dcc_app_create_defined()`, and `dcc_app_run_dotenv_defined()`,
  typed slash/subcommand/autocomplete/modal/button/select route wrappers, typed modal flow
  register/show helpers, compact typed slash sugar,
  typed Gateway event sugar such as `DCC_MEMBER_JOIN_FN()`,
  `DCC_MEMBER_JOIN_DATA()`, and `DCC_APP_ON_ROLE_UPDATE_DATA()`,
  app definition middleware, error handlers, and guard policies,
  persistent component-session store export/import,
  field validators for required strings, ranges and strict dates,
  strict date parsing into `dcc_ctx_parsed_date_t`,
  flow-aware interaction context shortcuts, `DCC_FLOW_*` action sugar,
  effective response-state helpers,
  wait/collector context helpers and owned wait-result accessors,
  app-owned file-backed store access via `dcc_app_store()`,
  focused autocomplete option shortcuts and static choice filtering,
  default error responses,
  global error handling,
  confirm, paginator, and settings panel component presets,
  app-owned signed component sessions, app-wide middleware, common guards,
  auto-defer, and interval or KST scheduled LLAM tasks.
- `client.h`: client creation, options, start/wait/stop, gateway discovery,
  event waits, typed object wait helpers, and `dcc_client_gateway_info()` for
  Gateway URL, identify budget, session ID, sequence, resumability, current
  reconnect request, task-running state, and last Gateway task status.
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
  users, messages, and voice states. `dcc_cache_infer_guild_id_from_channel()`
  and `dcc_client_infer_guild_id_from_channel()` resolve a cached channel ID to
  its guild ID without REST. Set `infer_guild_id_from_channel` in
  `dcc_client_options_t`, or use `DCC_CLIENT_OPTIONS_WITH_GUILD_INFERENCE()`
  / `DCC_CLIENT_OPTIONS_WITH_GUILD_INFERENCE_ALL()`, when voice helpers should
  fill `guild_id` automatically from `channel_id`.
- `message_link.h`: checked Discord message-link formatting and parsing.
  `dcc_message_link_parse()` accepts normal, canary, ptb, path-only, and DM
  `@me` links and returns a typed `dcc_message_link_t`; sugar aliases expose
  `DCC_MESSAGE_LINK()`, `DCC_DM_MESSAGE_LINK()`, and
  `DCC_PARSE_MESSAGE_LINK()`.
- `app.h`: app-runtime context helpers expose `dcc_ctx_user()`,
  `dcc_ctx_member()`, `dcc_ctx_guild()`, `dcc_ctx_channel()`, target helpers,
  and typed option object helpers such as `dcc_ctx_option_user()` and
  `dcc_ctx_option_channel()`. `dcc_ctx_response_state()` and the boolean
  response helpers expose replied/deferred/followup state, including app-level
  auto-defer. Option binding also supports direct borrowed
  object fields for users, members, roles, channels, messages, and attachments.
  They use interaction resolved objects first and the enabled cache as a
  borrowed fallback where that object type is cached.
- `sugar/app_registration.h`: focused direct app-registration sugar for
  slash, context menu, component, modal, event, message create/update/delete,
  ready, and message-prefix command routes, including `_DATA`, `_FN`,
  namespace, `_POLICY`, and `_GUARDED` route helpers.
- `sugar/decorators.h`: focused static route descriptor sugar for placing route
  metadata beside the handler. `DCC_DECORATE_SLASH()`,
  `DCC_DECORATE_BUTTON_NS()`, `DCC_DECORATE_MODAL_NS()`,
  `DCC_DECORATE_READY_ONCE()`, typed event variants such as
  `DCC_DECORATE_MEMBER_JOIN()`, task/event variants, and route policy/data
  forms such as `DCC_DECORATE_SLASH_DATA_GUARDED()` and
  `DCC_DECORATE_BUTTON_NS_DATA_GUARDED()` declare the handler and expose it to
  Cogs through `DCC_ROUTE(handler_name)`. Typed slash params decorators such as
  `DCC_DECORATE_SLASH_PARAMS_DATA_GUARDED()` and typed modal/component params
  decorators such as `DCC_DECORATE_MODAL_NS_PARAMS_DATA_GUARDED()`,
  `DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED()`, and
  `DCC_DECORATE_SELECT_NS_PARAMS_DATA_GUARDED()` expose typed descriptors
  through `DCC_TYPED_ROUTE(handler_name)` for the matching
  `DCC_COG_*_ROUTES` list.
- `sugar/listeners.h`: discord.js-style listener table sugar.
  `DCC_LISTEN_SLASH()`, `DCC_LISTEN_BUTTON_NS()`,
  `DCC_LISTEN_READY_ONCE()`, typed gateway event aliases such as
  `DCC_LISTEN_MEMBER_JOIN()`, `DCC_LISTEN_PREFIX_COMMAND()`,
  `DCC_LISTEN_TASK_DAILY_KST()`, `DCC_LISTEN_COMMAND_SCHEMA()`,
  `DCC_LISTEN_COMMAND_BUILDER_VALUE()`, `DCC_LISTEN_COMMAND_PARAMS()`,
  `DCC_LISTEN_COMMAND_FN_DATA_GUARDED()`, `DCC_LISTEN_MODAL_PARAMS_DATA()`,
  `DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED()`,
  `DCC_LISTEN_SELECT_NS_PARAMS_DATA()`, and decorated-handler aliases such as
  `DCC_LISTEN_DECORATED_COMMAND()`, `DCC_LISTEN_DECORATED_BUTTON()`, and
  `DCC_LISTEN_DECORATED_TYPED_COMMAND()` build a `DCC_LISTENERS(...)` table for
  `DCC_SIMPLE_BOT()`, `DCC_SIMPLE_BOT_MAIN()`,
  `DCC_LISTENERS_REGISTER(app, ...)`, or `DCC_DEFINE_LISTENERS()` plus
  `DCC_LISTENERS_MODULE(...)`. The older `DCC_ROUTE_*` names remain compatible.
- `sugar/messages.h`: focused message-builder literals such as
  `DCC_MESSAGE_TEXT()`, `DCC_MESSAGE_EMBEDS()`,
  `DCC_MESSAGE_COMPONENTS()`, and `DCC_MESSAGE_COMPONENTS_V2()`.
- `sugar/app_actions.h`: focused app, channel, message, thread, and member
  action shortcuts such as `DCC_APP_SEND_TEXT()`,
  `DCC_CHANNEL_SEND_SAFE()`, `DCC_MESSAGE_THREAD()`,
  `DCC_THREAD_LOCK()`, and `DCC_MEMBER_ADD_ROLE()`.
- `sugar/context.h`: focused interaction-context shortcuts, including option
  access, target/resource helpers, display-name and mention helpers, guards,
  `DCC_CTX_REPLY_UI()`, `DCC_CTX_UPDATE_UI()`, `DCC_CTX_SEND_UI()`, and
  `DCC_CTX_SHOW_MODAL_V2()`.

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
  interaction response/followup helpers. `dcc_rest_response_message_id()`,
  `dcc_rest_response_channel_id()`, and
  `dcc_rest_response_snowflake_field()` extract snowflake fields from callback
  bodies without constructing a JSON DOM.
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
- `interaction_helpers.h`: application-level shortcuts for the common initial
  interaction responses: text, ephemeral text, colored embed success/error,
  defer, ephemeral defer, message update, and modal display. These are thin
  wrappers over the typed REST interaction response functions.
- `managed_message.h`: storage-agnostic latest-message publishing. Callers
  provide load/save callbacks for a message reference, and DCC deletes the
  previous message, posts the new message, parses the created message ID, and
  saves the new reference.
- `store.h`: tiny key-value store abstraction with a dependency-free file
  backend, string/u64 helpers, context/app store sugar, and managed-message
  reference bindings for latest-message publishing.
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
  attached. `autocomplete.h` exposes the Discord response choice limit through
  `DCC_AUTOCOMPLETE_MAX_CHOICES`.
- `objects/clone/interactions.h`: command option accessors, resolved object
  lookups, and Components v2 modal submit helpers such as
  `dcc_interaction_form_value()` and `dcc_interaction_form_boolean()`.
- `objects/base.h`: gateway message objects preserve message `components` as
  raw `dcc_message_t::components_json` and project a typed
  `dcc_message_t::components` tree for Components v2 layouts.
- `component_session.h`: signed stateful UI sessions for legacy and Components
  v2 controls, including `dcc_component_session_button_v2()`; app users can
  route sessions with `dcc_app_component_session()`.

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
