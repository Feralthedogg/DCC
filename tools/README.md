# Tools

This directory contains runtime helper tools and maintainer scripts. Audits are
wired into CTest when Python is available.

- `dcc_new_app` scaffolds a small sugar-first DCC bot project with CMake,
  `.env.example`, `commands.json`, and a `/ping` feature. `dcc_new_app add feature
  PATH NAME` adds another `src/NAME.c`/`src/NAME.h` feature and wires generated app
  markers automatically. Generated callbacks use `DCC_COMMAND_IMPL`, `DCC_HANDLER`,
  `DCC_COMMAND_ARGS_IMPL`, `DCC_BUTTON_ARGS_IMPL`, `DCC_SELECT_ARGS_IMPL`,
  `DCC_MODAL_ARGS_IMPL`, `DCC_AUTOCOMPLETE_ARGS_IMPL`, `DCC_READY_FN`,
  `DCC_EVENT_FN`, `DCC_MESSAGE_COMMAND_FN`, and `DCC_TASK_FN` so users do not
  have to remember each app callback signature.
  `dcc_new_app add command PATH NAME COMMAND` appends a
  typed args struct, `*_PARAMS` list, slash route, and command JSON entry to
  that feature when its generated route markers are still present. Guard options
  like `--guild-only`, `--owner`, `--role`, `--any-role`, `--permission`, and
  `--cooldown-user LIMIT:MS` generate `_GUARDED` routes with short policy
  aliases such as `DCC_ONLY_GUILD()` and `DCC_RATE_LIMIT_USER(...)`.
  `dcc_new_app add subcommand PATH NAME COMMAND SUBCOMMAND` appends a typed
  subcommand args struct, `*_PARAMS` list, route, and rewrites the generated
  command schema to use `DCC_CMD_SUB_PARAMS(...)` while preserving parent
  command guards.
  `dcc_new_app add button`,
  `add select`, `add modal`, and `add event` append typed button, typed select,
  typed modal, and event handlers to the same feature file. Button/select/modal
  generation also creates a small args struct and inline
  `DCC_COMPONENT_ARGS(...)` or `DCC_FORM_ARGS(...)` bindings so handlers receive
  bound component or form data without extra binding macros.
  `dcc_new_app add view` appends a slash opener and a persistent
  `DCC_FEATURE_VIEWS(...)` block with a button-prefix route and modal route for
  Components v2 messages that should keep working across restarts.
  `dcc_new_app add config PATH FIELD_NAME TYPE ENV_NAME` appends a typed field
  to generated `src/config.h`, wires the matching `DCC_CONFIG_*` binding, and
  updates `.env.example`. Generated feature files include that header and can use
  `BOT_CONFIG(user_data)` or `BOT_CTX_CONFIG(ctx)`.
  `dcc_new_app add latest-message PATH NAME COMMAND CHANNEL_FIELD` appends a
  slash command that publishes a Components v2 payload through
  `DCC_APP_PUBLISH_LATEST_STORE(...)`, using an app store key so the previous
  message is deleted and replaced. `add scheduled-latest` uses the same pattern
  from a task route and accepts the normal task schedule flags.
  `dcc_new_app add autocomplete`
  appends a typed autocomplete handler and marks the generated slash option as
  autocomplete enabled when the command still has the generated `name` option.
  Autocomplete accepts the same guard options as interaction routes and emits
  the matching `_GUARDED` route when guards are present.
  `dcc_new_app add message-command` and `add task` append prefix commands and
  scheduled background loops. `dcc_new_app add preset PATH NAME announcement`
  appends a modal announcement composer with app-store draft state, ephemeral
  preview, and send/cancel buttons; `confirm` and `paginator` append common slash-command UI flows plus button routes using the
  component preset sugar; `form` appends a slash-to-modal flow with typed modal
  submit args; `settings` appends a settings panel, button routes, and an edit
  modal submit route; `wizard` appends a start panel, button flow, typed modal,
  and review buttons; `counter` appends a store-backed counter slash command
  with `+1` and reset buttons; `crud` appends a store-backed item list with
  add, delete-last, and clear actions; `help` appends a paged help center with
  buttons; `menu` appends a typed select-menu navigation panel; `poll` appends a
  store-backed yes/no vote panel; `profile` appends a user profile card with a
  refresh button; `roles` appends a guild-only self-role menu with typed select
  handling and add/remove role actions; `ticket` appends an open-ticket button,
  typed modal, ephemeral ticket card, and close button; `welcome` appends a
  guild member join event route plus slash preview; `flow` appends a slash-to-modal-to-confirm
  interaction flow. The generated app loads `.env`, binds typed feature
  config with `DCC_CONFIG(...)`, and then uses `DCC_RUN_APP_ENV(...)`,
  `DCC_APP(...)`, `DCC_DEFINE_PUBLIC_FEATURE(...)`, `DCC_APP_USE(...)`,
  `DCC_USE_FEATURE(...)`,
  `DCC_FEATURE_COMMAND_ROUTES(...)`, `DCC_COMMAND_ROUTE_NO_OPTIONS*`, `DCC_COMMAND_ROUTE*`, `DCC_ON_*`,
  `DCC_CTX_*` replies, `DCC_CTX_TRY(...)` for status-returning handler calls,
  `DCC_UI_*` Components v2 builders, `DCC_APP_DEV_MODE()`, and app-owned store
  access through `DCC_CTX_STORE_*`/`DCC_APP_STORE_*` with the same
  `DCC_TOKEN` environment convention as the other runtime tools.
- `dcc_command_sync` reconciles local application-command JSON with Discord
  REST or a saved remote fixture. It defaults to global scope and plan output;
  pass `--guild` for guild scope and `--apply` for writes.
- `dcc_replay` validates/summarizes replay JSONL files and can feed captured
  gateway/interaction records into a hot-reload module through isolated worker
  mode.
- `dcc_hot_reload_host` and `dcc_hot_reload_worker` provide the installed
  isolated hot-reload runtime.
- `dcc_interaction_webhook` provides the installed shardless interaction
  webhook runtime.

- `audit_project_layout.py` keeps the public/internal header boundary intact and
  enforces source-size budgets for module splits.
- `audit_gateway_fast_path.py` checks Gateway parser fast-path coverage.
- `audit_gateway_payload_fields.py` checks Gateway payload scratch coverage.
- `audit_event_surface.py` checks event API coverage.
- `audit_capacity_growth.py` checks that dynamic capacity doubling has
  overflow guards.
- `audit_rest_surface.py`, `audit_rest_request_assertions.py`, and `audit_rest_field_smoke.py`
  check REST API coverage.
- `audit_source_package.py` checks that hidden local `.env` files and cache/build
  metadata are excluded without dropping deployment env templates.
- `audit_deploy_templates.py` checks systemd, env, reverse-proxy, Kubernetes,
  and hot reload deployment template contracts.
- `release_check.sh` runs the full release validation flow.
  It also dry-runs the token-backed hot reload live-soak helper without opening
  a network connection and builds the installed hot reload example Makefile
  against the installed CMake package. Source packages are checked for hidden
  `.env`, Python cache, compile database, and build metadata leakage, and
  installed packages compile a pkg-config consumer when `pkg-config`/`pkgconf`
  is available. Installed hot reload plus interaction webhook deployment
  templates are checked against `dcc_hot_reload_host --check` and
  `dcc_interaction_webhook --check` with release fixture paths.
- `package_release.sh` builds the native CPack binary/source archives used by
  the GitHub Release workflow and writes normalized files plus sha256 manifests
  under `target/dist/`.

See `../docs/release.md` for when to run these checks and
`../docs/production-playbooks.md` for operational response guidance.
