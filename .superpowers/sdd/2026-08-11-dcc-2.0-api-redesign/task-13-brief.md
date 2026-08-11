# Task 13 brief — canonical Bot API

Implement only Task 13 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The approved
design, the completed Task 2 listener registry, the completed Task 11 ordered
interaction queue, the completed Task 12 tagged Components v2 contract, and
this brief are authoritative. This brief fixes the exact Bot topology, names,
macro arithmetic, ownership rules, language gates, runner behavior, and
Task 14 handoff. Work test-first, do not push, and do not edit the ignored
progress ledger. Keep every committed intermediate state buildable.

Task 13 must not start until Tasks 11 and 12 are implemented, committed, and
review-green. The safety of by-value reply/UI helpers depends on Task 11
serializing an admitted action before returning. The UI surface depends on
Task 12's version-1 tagged builder, historical-stride rules, and exact
three-macro contribution. Do not recreate either prerequisite behind a Bot
compatibility wrapper.

## Outcome and non-goals

After this task a normal application needs only `<dcc/bot.h>` and sees one
curated application vocabulary:

- nine status-returning handler declaration macros;
- exactly 19 flat listener constructor families, each with a short and one
  `_WITH` static-inline form;
- one composable `dcc_listener_config_t`, with no `DCC_ROUTE_*` vocabulary;
- state-aware static-inline context action families;
- allocation-free Components v2 `DCC_UI_*` value constructors;
- exactly nine C-only variadic UI array helpers;
- one versioned `dcc_bot_config_t` and one public static-inline runner; and
- exactly four entrypoint macros, split into development/production and
  default/advanced forms.

Task 13 does not delete Sugar, `include/dcc/app/legacy.h`, void-handler
adapters, or repository-wide DCC 1 consumers; Task 14 owns that cut. It does
not add REST endpoints, another registration engine, another interaction
state machine, another Components v2 layout, exported Bot runner symbols, or
listener suffix matrices. It does not add a coroutine abstraction.

The Bot implementation is a typed construction layer over the canonical App,
interaction, message/modal, and Components v2 APIs. `dcc_app_listen()` remains
the only dynamic registration path, Task 11 remains the only context action
queue, and Task 12 remains the only Components v2 validator/serializer.

## Files and dependency boundary

Create:

- `include/dcc/bot.h`
- `include/dcc/bot/handlers.h`
- `include/dcc/bot/listeners.h`
- `include/dcc/bot/replies.h`
- `include/dcc/bot/ui.h`
- `include/dcc/bot/main.h`
- `tests/bot_v2_surface_smoke.c`
- `tests/bot_v2_runner_smoke.c`
- `tests/bot_v2_main_compile.c`

Modify as required by the exact contract:

- `include/dcc/app.h`
- `include/dcc/app/options.h`
- `include/dcc/app/lifecycle.h`
- `include/dcc/app/env.h`
- `include/dcc/app/legacy.h`
- `src/app/app_env.c`
- `src/app/app_definition.c`
- `src/app/app_signal.c` only if a declaration/include adjustment is required;
  preserve its canonical signal-runner implementation
- `tests/api_v2_surface_smoke.c`
- focused App/env/interaction/component/package tests discovered by `rg`
- `tools/audit_v2_surface.py`
- `tools/api_v2_removed_names.txt` only if an immutable banned spelling is
  missing
- `CMakeLists.txt`

Do not include `<dcc/bot.h>` from `<dcc/dcc.h>` during this task. The transition
tree still installs Sugar macros with colliding DCC 1 spellings. Task 14 first
deletes Sugar and then makes `<dcc/dcc.h>` the complete aggregate containing
`<dcc/bot.h>`. Task 13 tests Bot-only translation units and focused dependencies;
mixing Bot and Sugar in one translation unit is not a supported transition
path.

## Exact five-leaf header topology

`include/dcc/bot.h` contains only its guard, documentation, and these five
includes in dependency-safe order:

```c
#include <dcc/bot/handlers.h>
#include <dcc/bot/listeners.h>
#include <dcc/bot/replies.h>
#include <dcc/bot/ui.h>
#include <dcc/bot/main.h>
```

The leaves are standalone public headers:

| Header | Owns | Direct focused dependencies |
| --- | --- | --- |
| `<dcc/bot/handlers.h>` | nine declaration macros | `<dcc/app/base.h>` |
| `<dcc/bot/listeners.h>` | config/handler values and 38 listener constructors | `<dcc/app/listeners.h>` and only types required by the descriptors |
| `<dcc/bot/replies.h>` | context action inlines | `<dcc/app/context.h>`, message, modal, and autocomplete focused headers as needed |
| `<dcc/bot/ui.h>` | fixed/array UI inlines and nine C-only variadic helpers | Task 12 `<dcc/component_v2.h>`, `<dcc/message.h>`, and `<dcc/modal.h>` as needed |
| `<dcc/bot/main.h>` | bot config, runner, and four mains | focused App options/lifecycle/listeners/env headers |

No leaf may include `<dcc/app.h>`, `<dcc/dcc.h>`, `<dcc/sugar.h>`, anything
under `dcc/sugar/`, or `<dcc/app/legacy.h>`. No leaf depends on another Bot leaf
for a type it can obtain from the focused owner. The umbrella includes exactly
the five leaves and no Core/REST aggregate. A structural audit parses include
edges rather than accepting transitive success.

Every leaf and the umbrella has one conventional include guard and compiles by
itself. These six guards are exact:

```text
DCC_BOT_H
DCC_BOT_HANDLERS_H
DCC_BOT_LISTENERS_H
DCC_BOT_REPLIES_H
DCC_BOT_UI_H
DCC_BOT_MAIN_H
```

## Exact macro budget

The compiler macro dump, not a source `#define` grep, is authoritative. Freeze
the arithmetic as follows:

| Contribution visible from C11 `<dcc/bot.h>` | Count |
| --- | ---: |
| Focused dependency baseline before Task 12 | 256 |
| Task 12 tagged Components v2 delta | 3 |
| Six Bot include guards | 6 |
| Nine handler declaration macros | 9 |
| `DCC_LISTENER_CONFIG_VERSION` and `DCC_LISTENER_CONFIG_INIT` | 2 |
| `DCC_BOT_CONFIG_VERSION` and `DCC_BOT_CONFIG_INIT` | 2 |
| Nine C-only variadic UI helpers | 9 |
| Four main macros | 4 |
| **Complete preprocessed `<dcc/bot.h>` surface** | **291** |

Thus Task 13's Bot-owned delta is exactly `6 + 9 + 2 + 2 + 9 + 4 = 32`,
and the final count is exactly `256 + 3 + 32 = 291`, leaving nine below the
hard ceiling of 300. A count below 291 is also a failure in this task because
it means a required surface disappeared or the baseline was measured
incorrectly. A later task may use the nine-name headroom only through a
separately reviewed design change.

All uppercase listener constructors, handler-union constructors, config
helpers, fixed/array UI constructors, context actions, `DCC_BOT_CONFIG(name)`,
and `dcc_bot_run()` are functions, not macros. Enum values are not macros.
There are no private helper macros with a `DCC_` prefix. Local implementation
helpers use lowercase static-inline functions.

`tools/audit_v2_surface.py` must:

1. preprocess a sentinel translation unit containing only
   `#include <dcc/bot.h>` with the configured C compiler's full macro-dump
   option;
2. prove the sentinel itself appears, so an empty/broken dump cannot pass;
3. count unique visible `DCC_` definitions and require exactly 291 and at most
   300;
4. compare the Bot-owned definitions against the exact 32-name inventory;
5. reject a function intended to be inline if it was accidentally implemented
   as a macro; and
6. run a C++17 dump proving the nine C-only UI macro names are absent while the
   fixed/array surface remains usable.

## Nine handler declaration macros

`<dcc/bot/handlers.h>` defines exactly these nine public macros and no public
or suffix aliases:

```c
DCC_SLASH_FN(name)
DCC_AUTOCOMPLETE_FN(name)
DCC_BUTTON_FN(name)
DCC_SELECT_FN(name)
DCC_MODAL_FN(name)
DCC_EVENT_FN(name)
DCC_READY_FN(name)
DCC_MESSAGE_FN(name)
DCC_TASK_FN(name)
```

Their expansions are fixed by the Task 2 callback typedefs:

```c
#define DCC_SLASH_FN(name_) \
    static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
#define DCC_AUTOCOMPLETE_FN(name_) \
    static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
#define DCC_BUTTON_FN(name_) \
    static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
#define DCC_SELECT_FN(name_) \
    static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
#define DCC_MODAL_FN(name_) \
    static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
#define DCC_EVENT_FN(name_) \
    static dcc_status_t name_(dcc_app_t *app, const dcc_event_t *event, void *user_data)
#define DCC_READY_FN(name_) \
    static dcc_status_t name_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)
#define DCC_MESSAGE_FN(name_) \
    static dcc_status_t name_(dcc_app_t *app, const dcc_message_t *message, \
        const dcc_event_t *event, void *user_data)
#define DCC_TASK_FN(name_) \
    static dcc_status_t name_(dcc_app_t *app, void *user_data)
```

They declare internal-linkage handlers and never append a body, implicit
return, `return;`, status-discarding adapter, registration, or user-data
capture. Every body must return `dcc_status_t`. The dispatcher forwards a
non-OK result exactly once to the App error path established by Tasks 2–3.

There is deliberately no `DCC_PUBLIC_*_FN`, `DCC_TYPED_*_FN`,
`DCC_MESSAGE_COMMAND_FN`, `DCC_COMMAND_FN`, `_DATA`, `_GUARDED`, or declaration
alias. A typed route or message-command callback is declared explicitly with
the canonical callback signature. The nine helpers cover the common path
without hiding the argument object type behind an unsafe function-pointer cast.

## Listener configuration and handler values

`<dcc/bot/listeners.h>` introduces this header-only convenience value:

```c
typedef struct dcc_listener_config {
    size_t size;
    uint32_t version;
    void *user_data;
    dcc_app_cleanup_fn cleanup;
    const dcc_application_command_builder_t *command;
    dcc_listener_route_policy_t policy;
    size_t args_size;
    dcc_listener_bindings_t bindings;
    dcc_listener_validators_t validators;
    dcc_listener_validation_policy_t validation;
    uint8_t once;
} dcc_listener_config_t;

#define DCC_LISTENER_CONFIG_VERSION 1U
#define DCC_LISTENER_CONFIG_INIT /* complete C11/C++17 initializer */
```

The initializer sets full current size/version and initializes every nested
evolvable value to its corresponding Task 2 size/version. It selects no user
data, cleanup, schema, policy, binding, validator, or once behavior. It is a
valid empty configuration, not a zero-filled object with invalid nested
versions.

Provide documented public static-inline value helpers, including:

```c
dcc_listener_config_t DCC_LISTENER_CONFIG(void);
dcc_listener_config_t DCC_LISTENER_CONFIG_WITH_POLICY(
    dcc_listener_route_policy_t policy
);
dcc_listener_handler_t DCC_LISTENER_PLAIN_HANDLER(dcc_app_handler_fn handler);
dcc_listener_handler_t DCC_LISTENER_TYPED_HANDLER(dcc_app_typed_handler_fn handler);
dcc_listener_handler_t DCC_LISTENER_EVENT_HANDLER(dcc_app_event_fn handler);
dcc_listener_handler_t DCC_LISTENER_READY_HANDLER(dcc_app_ready_fn handler);
dcc_listener_handler_t DCC_LISTENER_MESSAGE_HANDLER(dcc_app_message_fn handler);
dcc_listener_handler_t DCC_LISTENER_MESSAGE_COMMAND_HANDLER(
    dcc_app_message_command_fn handler
);
dcc_listener_handler_t DCC_LISTENER_TASK_HANDLER(dcc_app_task_fn handler);
dcc_listener_schedule_target_t DCC_LISTENER_INTERVAL(uint64_t interval_ms);
dcc_listener_schedule_target_t DCC_LISTENER_DAILY_KST(
    uint8_t hour,
    uint8_t minute
);
```

These spellings are functions even though they are uppercase. They evaluate
each argument once. Do not add `DCC_ROUTE_CONFIG`, `DCC_ROUTE_PARAMS`,
`DCC_ROUTE_POLICY`, a policy suffix matrix, or type-punning callback casts.
Advanced callers start from `DCC_LISTENER_CONFIG()` or
`DCC_LISTENER_CONFIG_INIT`, assign the desired policy/binding/validation fields,
and pass one value to one `_WITH` constructor.

`command` is an optional complete command schema. A constructor maps it to the
Task 2 route target without retaining redundant name/description fields that
would make the descriptor invalid. `once` is accepted only by event, ready,
and message-event constructors. Route, message-command, and task descriptors
leave it inactive. `args_size > 0` requires the typed handler member and a
binding kind compatible with the route family; `dcc_app_listen()` remains the
final validator and returns `DCC_ERR_INVALID_ARG` without partial registration
for an invalid combination.

## Exact 19 listener pairs

There are exactly 19 constructor stems and exactly two functions per stem: the
short spelling and the same spelling with `_WITH`. That is 38 public
`static inline dcc_listener_t` functions and zero `DCC_LISTEN_*` macros.

| Stem | Short-form target arguments before the callback | Short callback type |
| --- | --- | --- |
| `DCC_LISTEN_SLASH` | `name, description` | `dcc_app_handler_fn` |
| `DCC_LISTEN_SUBCOMMAND` | `command_name, subcommand_path, description` | `dcc_app_handler_fn` |
| `DCC_LISTEN_AUTOCOMPLETE` | `command_name, description` | `dcc_app_handler_fn` |
| `DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE` | `command_name, subcommand_path, description` | `dcc_app_handler_fn` |
| `DCC_LISTEN_USER_CONTEXT_MENU` | `name` | `dcc_app_handler_fn` |
| `DCC_LISTEN_MESSAGE_CONTEXT_MENU` | `name` | `dcc_app_handler_fn` |
| `DCC_LISTEN_BUTTON` | `custom_id` | `dcc_app_handler_fn` |
| `DCC_LISTEN_BUTTON_PREFIX` | `custom_id_prefix` | `dcc_app_handler_fn` |
| `DCC_LISTEN_SELECT` | `custom_id` | `dcc_app_handler_fn` |
| `DCC_LISTEN_SELECT_PREFIX` | `custom_id_prefix` | `dcc_app_handler_fn` |
| `DCC_LISTEN_MODAL` | `custom_id` | `dcc_app_handler_fn` |
| `DCC_LISTEN_MODAL_PREFIX` | `custom_id_prefix` | `dcc_app_handler_fn` |
| `DCC_LISTEN_EVENT` | `dcc_event_type_t type` | `dcc_app_event_fn` |
| `DCC_LISTEN_READY` | none | `dcc_app_ready_fn` |
| `DCC_LISTEN_MESSAGE_CREATE` | none | `dcc_app_message_fn` |
| `DCC_LISTEN_MESSAGE_UPDATE` | none | `dcc_app_message_fn` |
| `DCC_LISTEN_MESSAGE_DELETE` | none | `dcc_app_message_fn` |
| `DCC_LISTEN_MESSAGE_COMMAND` | `prefix, name` | `dcc_app_message_command_fn` |
| `DCC_LISTEN_TASK` | `dcc_listener_schedule_target_t schedule` | `dcc_app_task_fn` |

Every short form takes its typed callback last and is equivalent to its
`_WITH` form with the matching handler-union helper plus
`DCC_LISTENER_CONFIG()`. Every `_WITH` form takes the same target arguments,
then `dcc_listener_handler_t handler`, then `dcc_listener_config_t config`.
Representative exact signatures are:

```c
static inline dcc_listener_t DCC_LISTEN_SLASH(
    const char *name,
    const char *description,
    dcc_app_handler_fn handler
);

static inline dcc_listener_t DCC_LISTEN_SLASH_WITH(
    const char *name,
    const char *description,
    dcc_listener_handler_t handler,
    dcc_listener_config_t config
);

static inline dcc_listener_t DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_WITH(
    const char *command_name,
    const char *subcommand_path,
    const char *description,
    dcc_listener_handler_t handler,
    dcc_listener_config_t config
);

static inline dcc_listener_t DCC_LISTEN_TASK_WITH(
    dcc_listener_schedule_target_t schedule,
    dcc_listener_handler_t handler,
    dcc_listener_config_t config
);
```

`DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE` maps to
`DCC_LISTENER_AUTOCOMPLETE` with `command_name` and `subcommand_path`; it does
not add a twentieth enum kind. Do not substitute `READY_ONCE` or split task
schedules into extra listener pairs. Once behavior belongs to the one config,
and schedule shape belongs to the one task target value.

Each constructor calls/duplicates only the canonical initialization semantics
of `dcc_listener_init()`, sets exactly one compatible handler member, and fills
only the active target union. It does not register, allocate, serialize, or
retain a pointer to one of its own parameters. Invalid config size/version
produces a deterministically invalid descriptor that `dcc_app_listen()` rejects;
it never causes an uncovered read.

Because these are function calls, a listener expression is not a C constant
initializer. Update `tests/api_v2_surface_smoke.c` and examples introduced in
this task to create descriptor arrays at block scope. Do not turn the functions
back into macros merely to permit file-scope arrays. The main helpers create
their local flat array inside `main()`.

The exact-name audit rejects any additional public stem or suffix, including
`DCC_LISTEN_COMMAND`, `DCC_LISTEN_*_DATA`, `*_POLICY`, `*_GUARDED`, `*_FN`,
namespace aliases, `DCC_LISTEN_READY_ONCE`, and schedule-unit aliases.

## Static-inline context action families

`<dcc/bot/replies.h>` provides functions, not macros. The short forms pass a
null terminal callback/user-data to the lowercase Task 11 operation; callers
that need completion use the focused lowercase API directly. Do not rebuild a
parallel callback suffix matrix.

Expose these canonical groups:

| Family | Static-inline Bot spellings |
| --- | --- |
| state-aware reply | `DCC_CTX_REPLY`, `DCC_CTX_REPLY_TEXT`, `DCC_CTX_REPLY_EPHEMERAL`, `DCC_CTX_REPLY_EPHEMERAL_TEXT`, `DCC_CTX_REPLY_UI`, `DCC_CTX_REPLY_UI_ARRAY`, `DCC_CTX_REPLY_EPHEMERAL_UI`, `DCC_CTX_REPLY_EPHEMERAL_UI_ARRAY` |
| defer | `DCC_CTX_DEFER`, `DCC_CTX_DEFER_EPHEMERAL` |
| component-source update | `DCC_CTX_UPDATE`, `DCC_CTX_UPDATE_UI`, `DCC_CTX_UPDATE_UI_ARRAY` |
| forced followup | `DCC_CTX_FOLLOWUP`, `DCC_CTX_FOLLOWUP_TEXT`, `DCC_CTX_FOLLOWUP_EPHEMERAL`, `DCC_CTX_FOLLOWUP_EPHEMERAL_TEXT`, `DCC_CTX_FOLLOWUP_UI`, `DCC_CTX_FOLLOWUP_UI_ARRAY`, `DCC_CTX_FOLLOWUP_EPHEMERAL_UI`, `DCC_CTX_FOLLOWUP_EPHEMERAL_UI_ARRAY` |
| unrelated channel send | `DCC_CTX_SEND`, `DCC_CTX_SEND_TEXT`, `DCC_CTX_SEND_UI`, `DCC_CTX_SEND_UI_ARRAY` |
| modal | `DCC_CTX_SHOW_MODAL` |
| autocomplete | `DCC_CTX_AUTOCOMPLETE` |

Message/modal/autocomplete builder-taking short forms accept the value by value,
take its address only inside the inline body, and call the lowercase operation
before returning. Single-component UI helpers similarly make a local one-item
array and delegate to the matching `_ARRAY` function. This is safe only because
Task 11 and the Task 5/12 builder contracts serialize/copy every borrowed input
before the call returns. Add a focused regression that destroys or overwrites
the caller's stack immediately after admission and still observes the original
queued payload.

All functions return the queue-admission `dcc_status_t`; none converts failure
to success, calls the error observer itself, or expands to bare `return`. Reply
remains state-aware: initial callback when ready, original edit after defer, and
followup after a completed initial reply. Update always means component-source
update, while followup always forces a followup and send always creates an
unrelated channel message.

Do not expose `RESPONSE`, `RESPOND`, `PUBLIC`, `PRIVATE`, `DONE`, `OK`,
`OR_EDIT`, `OR_FOLLOWUP`, `_CB`, `V2`, or silent/no-embed aliases. Visibility is
spelled `EPHEMERAL`. `DCC_CTX_REPLY_AUTOCOMPLETE` is not an alias for
`DCC_CTX_AUTOCOMPLETE`.

## Canonical UI surface

`<dcc/bot/ui.h>` builds only Task 12 version-1 tagged values. Fixed and array
forms are documented public `static inline` functions and perform no
allocation. They call the low-level `dcc_component_v2_*` constructors/setters
instead of reintroducing flat designated initializers or inactive-union writes.

The fixed surface covers at least this one-way vocabulary:

- text: `DCC_UI_TEXT`;
- buttons: `DCC_UI_BUTTON`, `DCC_UI_PRIMARY`, `DCC_UI_SECONDARY`,
  `DCC_UI_SUCCESS`, `DCC_UI_DANGER`, `DCC_UI_LINK`, and `DCC_UI_PREMIUM`;
- layouts: `DCC_UI_ROW_ARRAY`, `DCC_UI_SECTION_ARRAY`, `DCC_UI_CARD_ARRAY`,
  and `DCC_UI_CARD_ACCENT_ARRAY`;
- separators: `DCC_UI_SEPARATOR` and `DCC_UI_SEPARATOR_LARGE`;
- media: `DCC_UI_MEDIA`, `DCC_UI_MEDIA_SPOILER`, `DCC_UI_THUMBNAIL`,
  `DCC_UI_FILE`, and `DCC_UI_GALLERY_ARRAY`;
- selects: `DCC_UI_STRING_SELECT_ARRAY`, `DCC_UI_USER_SELECT`,
  `DCC_UI_ROLE_SELECT`, `DCC_UI_MENTIONABLE_SELECT`, and
  `DCC_UI_CHANNEL_SELECT_ARRAY`;
- modal children: `DCC_UI_LABEL`, `DCC_UI_INPUT`,
  `DCC_UI_INPUT_PLACEHOLDER`, `DCC_UI_FILE_UPLOAD`,
  `DCC_UI_FILE_UPLOAD_WITH_TYPES`, `DCC_UI_RADIO_GROUP_ARRAY`,
  `DCC_UI_CHECKBOX_GROUP_ARRAY`, and `DCC_UI_CHECKBOX`;
- fixed leaf values for select options, select defaults, and radio/checkbox
  choice options; and
- `DCC_UI_MODAL_ARRAY` for a fixed component pointer/count.

Array functions always take an explicit pointer and count and are the safe
C/C++ interface. Pointer/count mismatch, checked-span overflow, wrong tagged
child, and Discord limit errors remain observable through Task 12 validation;
the convenience layer must not clamp, drop, or silently rewrite invalid input.
Custom IDs are passed explicitly. Do not recreate allocation-owning namespace
concatenation aliases in this task.

### Exact nine C-only variadic helpers

Only these nine `DCC_UI_*` names are macros:

```text
DCC_UI_ROW
DCC_UI_SECTION
DCC_UI_CARD
DCC_UI_CARD_ACCENT
DCC_UI_GALLERY
DCC_UI_STRING_SELECT
DCC_UI_RADIO_GROUP
DCC_UI_CHECKBOX_GROUP
DCC_UI_MODAL
```

Each macro creates the correctly typed C compound-literal array once, computes
its count with an unevaluated `sizeof`, and calls the matching fixed `_ARRAY`
function. `DCC_UI_SECTION` also creates one block-lifetime accessory value;
`DCC_UI_CARD_ACCENT` evaluates the accent once. The macros require at least one
variadic element, are valid at block scope, and borrow the compound literals
until the end of the enclosing C block. The action/build operation still must
serialize before those values expire.

`DCC_UI_FILE_UPLOAD` is a fixed static-inline function, not a variadic macro.
There is no tenth convenience macro for message, ephemeral message, option
arrays, default arrays, channel types, namespace IDs, or context actions.

Wrap the nine definitions in `#if !defined(__cplusplus)`. They must be wholly
undefined in C++17, not replaced by macros whose temporary arrays dangle after
the full expression. C++ callers use `_ARRAY` functions with caller-owned
storage. The rest of `ui.h`, including every fixed/array function, must compile
and run under C++17.

No canonical Bot header contains or expands a `DCC_V2_*` spelling. Transition
Sugar may continue to re-expand its existing aliases against Task 12 until
Task 14 deletes it, but Bot tests include no Sugar header.

## Bot config, runner, and four entrypoints

`<dcc/bot/main.h>` owns this exact version-1 configuration layout:

```c
typedef struct dcc_bot_config {
    size_t size;
    uint32_t version;
    const char *name;
    const dcc_app_options_t *app_options;
    const char *token_env;
    uint8_t install_default_error_handler;
    uint8_t handle_signals;
} dcc_bot_config_t;

#define DCC_BOT_CONFIG_VERSION 1U
#define DCC_BOT_CONFIG_INIT /* full size/version, NULL name/options,
                               "DISCORD_TOKEN", 1U, 1U */
```

Provide a documented public static-inline
`dcc_bot_config_t DCC_BOT_CONFIG(const char *name)`. It starts from the exact
initializer and assigns `name`; it is a function and contributes no macro.

Provide exactly one documented public runner, also static inline:

```c
static inline dcc_status_t dcc_bot_run(
    const dcc_bot_config_t *config,
    const dcc_listener_t *listeners,
    size_t listener_count,
    uint8_t development
);
```

Do not add a `DCC_API` declaration or external Bot runner symbol. The runner
performs these steps in this order:

1. Validate the config historical prefix and version, required non-empty name,
   boolean fields, `development` as exactly 0 or 1, and the listener
   pointer/count pair. `NULL, 0` is the only empty-listener representation.
2. Normalize only covered config fields, supplying initializer defaults for an
   uncovered suffix. Do not copy `sizeof(current)` from a historical prefix.
3. In development mode call `dcc_app_env_load_dotenv()` first. Continue on
   `DCC_ERR_NOT_FOUND`; return any other failure. Production never reads a
   dotenv file.
4. Start with `dcc_app_options_init()` and normalize the covered bytes of a
   provided `app_options` into a current local value. Validate the mandatory
   nested client prefix before inspecting it. Do not read an uncovered suffix.
5. A covered, non-empty `app_options->client.token` wins. Otherwise obtain a
   token through `dcc_app_env_get_token()` using the covered non-empty
   `token_env`, whose default is `"DISCORD_TOKEN"`. Missing token is a local
   failure before App creation.
6. Development forces command synchronization to apply once on READY with
   application-ID inference while preserving the selected registry scope.
   Production forces `command_sync_on_ready` off and never mutates Discord
   commands on READY, even if the provided options requested it.
7. Create the App. If requested, install the canonical default error response
   policy. Register listeners in source order through `dcc_app_listen()` only.
8. Select `dcc_app_run_with_signals()` when `handle_signals` is one and
   `dcc_app_run()` when it is zero.
9. Destroy the App on every post-create exit. Return the first setup/register/
   run failure. If run succeeds and destroy fails, return the destroy status;
   if both run and destroy fail, preserve the run status.

The runner borrows `config`, `config->name`, `config->token_env`, the pointed-to
App options and their nested values, and the listener array only for the call.
`dcc_app_listen()` deep-copies descriptor targets, policy arrays, bindings,
validators, and schema data before each successful registration returns.
Callback functions and callback `user_data` remain borrowed for the registered
listener lifetime; cleanup runs exactly once under the Task 2 contract.

Define exactly these four entrypoint macros:

```c
DCC_DEV_BOT_MAIN(name, ...)
DCC_BOT_MAIN(name, ...)
DCC_DEV_BOT_MAIN_WITH(config, ...)
DCC_BOT_MAIN_WITH(config, ...)
```

Each emits one `int main(void)`, evaluates its config/name expression once,
creates one block-local flat `dcc_listener_t[]` whose elements are each
evaluated once, calls only `dcc_bot_run()`, and maps `DCC_OK` to exit code 0 and
every other status to exit code 1. Default forms use `DCC_BOT_CONFIG(name)`.
Development forms pass `development = 1`; production forms pass zero.

The four macros require at least one listener in standard C11. A deliberately
listener-free program calls `dcc_bot_run(&config, NULL, 0U, mode)` directly.
There is no nested `DCC_APP_LISTENERS`, `DCC_SIMPLE_BOT`, token/env/main alias,
feature/module main, guild main, run-only macro, or status-to-exit helper macro.
Compile each entrypoint in a separate object; two entrypoint expansions cannot
share one translation unit.

## Focused App promotion needed by Bot

Task 13 completes the focused App ownership needed by the runner without doing
Task 14's deletion.

`<dcc/app.h>` gains `<dcc/app/env.h>` in the dependency-safe focused sequence:

1. `<dcc/app/base.h>`
2. `<dcc/app/options.h>`
3. `<dcc/app/lifecycle.h>`
4. `<dcc/app/listeners.h>`
5. `<dcc/app/context.h>`
6. `<dcc/app/modules.h>`
7. `<dcc/app/store.h>`
8. `<dcc/app/env.h>`

Until Task 14, the aggregate may include transition-only
`<dcc/app/legacy.h>` after those exact eight. Task 14 removes that ninth
transition edge, leaving exactly eight. No declaration fragment returns.

Promote these exact environment names to `<dcc/app/env.h>` and their real
implementations:

```c
dcc_status_t dcc_app_env_load_file(const char *path, uint8_t overwrite);
dcc_status_t dcc_app_env_load_dotenv(void);
dcc_status_t dcc_app_env_validate_requirements(
    const dcc_app_env_requirement_t *requirements,
    size_t count
);
```

Move the old spellings to `legacy.h` as transition wrappers, never macros:
`dcc_app_load_env_file`, `dcc_app_load_dotenv`, and
`dcc_app_validate_env_requirements`. Canonical Bot code and new focused tests
use only `dcc_app_env_*`. Move the validation implementation to its environment
owner now if necessary; do not leave canonical env behavior owned by the
legacy App-definition unit.

Promote the existing external `dcc_app_run_with_signals(dcc_app_t *)`
declaration from `legacy.h` to `<dcc/app/lifecycle.h>`, with the same symbol and
implementation. Document its borrowed App lifetime, owning-thread restriction,
single active signal monitor, signal-handler restoration, and returned run
status. Preserve `src/app/app_signal.c`; this is a declaration ownership move,
not a new ABI symbol and not a removal.

`<dcc/app/options.h>` documents the `size`-gated historical-prefix contract and
the lifetime of client token, registry, command-sync, and store-path inputs so
`dcc_bot_run()` can normalize them without whole-struct reads. Do not add
initializer macros that would change the frozen Bot count.

### Task 14 ledger correction

This brief supersedes Task 14 wording that deletes all of `app_signal.c` or
counts `dcc_app_run_with_signals` as legacy. Task 14 must preserve the canonical
function and implementation and remove only these three signal/App-definition
wrappers:

```text
dcc_app_run_defined_with_signals
dcc_app_run_from_env_defined_with_signals
dcc_app_run_dotenv_defined_with_signals
```

Consequently the final `legacy.h` actual-removal ledger is **361 functions**:
359 legacy `dcc_app_*` functions plus the two noncanonical context functions
`dcc_ctx_state` and `dcc_ctx_show_modal_flow`. `app_signal.c` is not a whole-file
deletion. Task 14's 83 unchanged context relocations and final eight-header App
aggregate remain unchanged.

## Ownership and behavior documentation

Every public Bot macro and every public static-inline function has a Doxygen
ownership/behavior comment immediately adjacent to its declaration/definition.
The audit must reject an undocumented name rather than accepting a header-level
blanket comment.

Document these rules explicitly:

- listener target strings, schemas, policies, binding/validator arrays, and
  their nested strings are borrowed until `dcc_app_listen()` returns; the App
  owns its accepted copy afterward;
- listener callback `user_data` is borrowed until unlisten/App destroy, and a
  supplied cleanup callback runs exactly once after dispatch quiesces;
- UI constructors allocate nothing and borrow strings, arrays, singleton
  children, and accessories until the enclosing message/modal/action is
  serialized;
- Task 11 context actions serialize/copy builder input before returning and
  return only queue-admission status; result callbacks receive borrowed result
  views under the Task 4/11 contract;
- `dcc_bot_run()` borrows config, options, strings, and listeners for its
  duration and owns the created App until its final destroy attempt;
- accessors never imply ownership, and no Bot helper returns a heap object; and
- no convenience function stores a pointer to a by-value parameter after it
  returns.

Tests must include stack poisoning/mutation after listener registration and
after context admission. Documentation examples must handle or return every
status rather than casting it to void.

## Required RED evidence

Before adding production Bot headers, expand `tests/api_v2_surface_smoke.c` and
create `tests/bot_v2_surface_smoke.c`. Keep missing-Bot targets unregistered or
excluded from the default build for the RED commit so the repository's existing
configured build remains green. Capture commands and exact diagnostics proving:

- `<dcc/bot.h>` is missing;
- the nine handler declarations are unavailable;
- the 38 listener constructors and both configs are unavailable;
- canonical context action inlines and Task 12-backed UI names are unavailable;
- the current Task 1 fixture's file-scope listener array must move to block
  scope for function constructors;
- all four main macros are unavailable; and
- the strict surface audit still reports missing Bot surface.

The RED Bot-only fixture includes no Sugar, `<dcc/dcc.h>`, or aggregate App
header. It contains:

- a simple slash handler and listener;
- an explicitly declared typed handler with option bindings, validation, and a
  route policy in one `dcc_listener_config_t`;
- root and subcommand autocomplete listeners;
- exact and prefix component listeners;
- generic event, ready, message create/update/delete, message-command, and task
  listeners;
- a state-aware text reply and ephemeral reply;
- a Task 12 card containing text plus an action row/button;
- a modal using tagged modal children; and
- compile-selection branches for development and production entrypoints.

## GREEN contract tests

Register the canonical fixtures only after production headers exist. Required
coverage includes:

### Header and exact-name surface

- every leaf and umbrella standalone under strict C11 and C++17;
- all permutations needed to catch include-order dependence among the five
  leaves and their focused owners;
- umbrella include graph exactly five leaves and forbidden include graph empty;
- exact nine handler macro names/signatures and status propagation;
- exact 19 short plus 19 `_WITH` function names, with no extra listener stem or
  suffix;
- exact six/config/handler/UI/main Bot macro inventory and 291 total dump; and
- banned-name scans of installed Bot headers and preprocessed Bot surface.

### Listener construction and ownership

- every short and `_WITH` constructor produces full listener/nested
  size/version values, correct kind, one active handler member, and only the
  active target union;
- subcommand autocomplete uses the autocomplete kind plus command/path target;
- short versus `_WITH` equivalence for all 19 stems;
- plain and typed routes, schema override, user data/cleanup, policy, bindings,
  validators, validation response policy, event once, interval task, and daily
  KST task;
- each argument is evaluated once;
- invalid config version/size, handler mismatch, policy/binding mismatch,
  illegal once, invalid target, and invalid schedule reject without partial App
  mutation; and
- caller mutation after successful listen cannot change copied metadata, while
  callback user data remains the documented borrowed pointer and cleanup runs
  once.

### Context and UI

- initial reply, queued second reply/followup, defer/edit, update, forced
  followup, unrelated send, modal, and autocomplete use Task 11 ordering;
- plain, text, ephemeral, one-component UI, and explicit UI-array forms;
- input stack overwrite immediately after admission;
- every fixed/array UI constructor maps to the correct Task 12 tag/presence
  bits and does not allocate;
- exact nine C variadic macros, single evaluation, block lifetime, nested card,
  modal, string-select, radio, and checkbox examples;
- C++17 sees none of the nine macros and successfully uses all corresponding
  `_ARRAY` functions with stable arrays; and
- invalid tag/placement/count continues to return Task 12's exact reason/path
  before network admission.

### Runner and App promotion

- null/short/future config, bad version, empty name, non-boolean flags,
  non-boolean development, and every listener pointer/count pairing;
- missing dotenv tolerated only in development, malformed/I/O dotenv failure
  propagated, and production never loads it;
- covered non-empty App client token beats environment lookup; otherwise the
  selected token env/default is used without logging the token;
- historical App option prefixes are read only when covered and normalized
  onto current defaults;
- development forces apply-on-READY command sync and production force-disables
  READY mutation;
- optional default error policy installation, ordered registration, partial
  registration failure cleanup, run choice with/without signals, and App
  destruction on every post-create path;
- first-failure versus destroy-failure precedence exactly as specified;
- all four main macros compile in separate C11 and C++17 object targets and
  evaluate config/listener expressions once;
- canonical env names link and behave identically to the transition wrappers;
  new Bot/focused code contains no old spelling; and
- `dcc_app_run_with_signals` is declared exactly once from the focused owner,
  links to the preserved implementation, restores handlers, and retains its
  owner-thread restrictions.

Use existing client/HTTP/runtime intercepts or a focused test seam. No runner
test may connect to Discord or depend on a developer token.

## Implementation and commit sequence

Keep the default repository green at each commit:

1. Add/expand the unregistered RED fixtures, exact-name/macro audit manifests,
   and captured RED evidence. Commit this buildable contract state as
   `test: define canonical Bot API surface`.
2. Add focused App env names and transition wrappers, promote
   `dcc_app_run_with_signals`, and add focused tests without deleting DCC 1.
3. Add the six-header Bot topology, config/constructor/action/UI inlines,
   runner, and four mains. Register the Bot/API/main targets only when they are
   GREEN.
4. Make the exact macro/include/documentation audits GREEN and run the complete
   verification matrix. Squash or retain the reviewable preparation as agreed,
   but the completed implementation commit is
   `feat: add canonical Bot API`.

Do not fold Task 14 Sugar deletion or its ABI cutover into this task. Do not
push. Do not start repository-wide example/generator migration beyond the
focused fixtures needed to keep current transition targets building.

## Verification and handoff

Run at minimum:

- direct RED commands before implementation and the same commands GREEN;
- `dcc_api_v2_surface_smoke`, `dcc_bot_v2_surface_smoke`, and
  `dcc_bot_v2_runner_smoke`;
- four separate main compile objects in C11 and C++17;
- exact compiler macro dump proving 291/300 and exact Bot-owned delta 32;
- exact 9-handler, 19-pair/38-function, nine-C-only-UI, four-main, six-guard,
  and five-leaf topology audits;
- standalone/include-order strict C11 and C++17 probes;
- canonical App listener, error, lifecycle, env, Task 11 interaction queue,
  Task 12 component ABI/smoke, message, modal, and Task 6 endpoint-preflight
  tests;
- transition surface/Sugar tests to prove Task 13 did not prematurely break
  Task 14 inputs;
- native strict warnings, i686/x86_64 MinGW compile/link probes, and package
  consumer configure/build/run from a fresh installation;
- focused ASan/UBSan and repeated TSan tests for listener cleanup, interaction
  admission, signal-runner, and App teardown paths;
- `mkdocs --strict`, public API/source-layout audits, and documentation snippet
  compilation; and
- the complete enabled CTest suite.

Run `git diff --check` before each handoff. Write
`task-13-implementer-report.md` beside this brief with the captured RED output,
exact name inventories, include graph, ownership decisions, compiler macro
dump and arithmetic, C/C++/MinGW evidence, runner mode matrix, App env/lifecycle
promotion, Task 14 ledger correction, full verification commands/results,
sanitizer evidence, changed files, and commit hashes. Commit that report
separately and stop for review. Do not start Task 14.
