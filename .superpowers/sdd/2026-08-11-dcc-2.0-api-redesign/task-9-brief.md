# Task 9 brief — command, moderation, and guild-resource requests

Implement only Task 9 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The REST design,
the corrected active-endpoint inventory below, the Task 7 common call-option
contract, and this brief are authoritative. Work test-first, do not push, and
do not edit the ignored progress ledger. Keep intermediate commits buildable
and do not begin Task 10 domains or remove the public App REST mirrors frozen
through Task 10 and owned by the Task 14/15 compatibility cutover.

## Outcome

Migrate the 57 active Task 9 Discord operations to one typed, request-handle
submission function per endpoint. This task covers application commands and
permissions, application emojis, auto moderation, guild emojis and stickers,
guild message search, onboarding, scheduled events, stage instances, and guild
templates. It also replaces command-registry callback fan-out with an owned,
sequential aggregate operation.

Every active endpoint returns after complete validation, serialization or
copying, and queue admission. A successful return guarantees one terminal REST
result through the optional request handle, Task 7 call-option callback, and
error observer. Remove all endpoint-specific raw JSON, callback/user-data,
`_params`, `_builder`, `_multipart`, `_page`, and
`_with_localizations` overloads in these domains. Remove the five scope-
dispatch application-command composites. Do not add `_v2`, async, future,
sync, or endpoint-specific convenience variants; `dcc_rest_submit()` remains
the raw escape hatch.

The checked manifest's corrected global active total after the Task 8 rebase
is **224**, not the original 231-endpoint discovery total. The Task 8 handoff
is **123/224**; Task 9 adds 57 and finishes at **180/224**, leaving exactly
**44** Task 10 endpoints deferred. The progress audit must report those
numbers and strict mode may fail only for the exact 44 Task 10 entries.

## Frozen active inventory

The active Task 9 distribution is normative:

| Domain | Active | Active legacy aliases removed | Multipart | Audit reason |
| --- | ---: | ---: | ---: | ---: |
| application commands | 20 | 13 | 0 | 0 |
| auto moderation | 5 | 2 | 0 | 3 |
| emojis and stickers | 13 | 4 | 1 | 6 |
| message search | 1 | 1 | 0 | 0 |
| onboarding | 2 | 1 | 0 | 1 |
| scheduled events | 6 | 5 | 0 | 2 |
| stage instances | 4 | 2 | 0 | 3 |
| templates | 6 | 2 | 0 | 0 |
| **Total** | **57** | **30** | **1** | **15** |

The method distribution is exactly GET 25, POST 9, PUT 5, PATCH 9, and DELETE
9. The 57 canonical symbols are:

- Application commands (20):
  `dcc_rest_bulk_overwrite_global_commands`,
  `dcc_rest_bulk_overwrite_guild_commands`,
  `dcc_rest_create_application_emoji`, `dcc_rest_create_global_command`,
  `dcc_rest_create_guild_command`, `dcc_rest_delete_application_emoji`,
  `dcc_rest_delete_global_command`, `dcc_rest_delete_guild_command`,
  `dcc_rest_edit_global_command`, `dcc_rest_edit_guild_command`,
  `dcc_rest_edit_guild_command_permissions`,
  `dcc_rest_get_all_guild_command_permissions`,
  `dcc_rest_get_application_emoji`, `dcc_rest_get_application_emojis`,
  `dcc_rest_get_global_command`, `dcc_rest_get_global_commands`,
  `dcc_rest_get_guild_command`, `dcc_rest_get_guild_command_permissions`,
  `dcc_rest_get_guild_commands`, and
  `dcc_rest_modify_application_emoji`.
- Auto moderation (5): `dcc_rest_create_auto_moderation_rule`,
  `dcc_rest_delete_auto_moderation_rule`,
  `dcc_rest_get_auto_moderation_rule`,
  `dcc_rest_get_auto_moderation_rules`, and
  `dcc_rest_modify_auto_moderation_rule`.
- Emojis and stickers (13): `dcc_rest_create_guild_emoji`,
  `dcc_rest_create_guild_sticker`, `dcc_rest_delete_guild_emoji`,
  `dcc_rest_delete_guild_sticker`, `dcc_rest_get_guild_emoji`,
  `dcc_rest_get_guild_emojis`, `dcc_rest_get_guild_sticker`,
  `dcc_rest_get_guild_stickers`, `dcc_rest_get_sticker`,
  `dcc_rest_get_sticker_pack`, `dcc_rest_get_sticker_packs`,
  `dcc_rest_modify_guild_emoji`, and `dcc_rest_modify_guild_sticker`.
- Message search (1): `dcc_rest_guild_message_search`.
- Onboarding (2): `dcc_rest_get_guild_onboarding` and
  `dcc_rest_modify_guild_onboarding`.
- Scheduled events (6): `dcc_rest_create_guild_scheduled_event`,
  `dcc_rest_delete_guild_scheduled_event`,
  `dcc_rest_get_guild_scheduled_event`,
  `dcc_rest_get_guild_scheduled_event_users`,
  `dcc_rest_get_guild_scheduled_events`, and
  `dcc_rest_modify_guild_scheduled_event`.
- Stage instances (4): `dcc_rest_create_stage_instance`,
  `dcc_rest_delete_stage_instance`, `dcc_rest_get_stage_instance`, and
  `dcc_rest_modify_stage_instance`.
- Templates (6): `dcc_rest_create_guild_template`,
  `dcc_rest_delete_guild_template`, `dcc_rest_get_guild_templates`,
  `dcc_rest_get_template`, `dcc_rest_modify_guild_template`, and
  `dcc_rest_sync_guild_template`.

`dcc_rest_create_guild_sticker` is the one planned-at-baseline canonical that
Task 9 adds. Message search remains a Task 9 endpoint even though its current
header is grouped with emoji/sticker resources.

## Corrected stale endpoints and complete removal ledger

The original Task 9 slice contained 59 active-manifest candidates. Two are not
active Discord operations and must be removed from the active endpoint array,
declarations, definitions, tests, installed headers, and documentation rather
than migrated. Preserve them only in the checked removal ledger described
below:

- `dcc_rest_bulk_edit_guild_command_permissions` is disabled by Discord
  Application Command Permissions v2. Its legacy
  `dcc_rest_bulk_edit_guild_command_permissions_params` is stale too.
- `dcc_rest_create_guild_from_template` is absent from the current official
  Guild Template API. Its legacy
  `dcc_rest_create_guild_from_template_params` is stale too.

The active legacy-removal set is exactly these 30 symbols:

```text
dcc_rest_bulk_overwrite_global_commands_builder
dcc_rest_bulk_overwrite_guild_commands_builder
dcc_rest_create_application_emoji_params
dcc_rest_create_auto_moderation_rule_params
dcc_rest_create_global_command_builder
dcc_rest_create_guild_command_builder
dcc_rest_create_guild_emoji_params
dcc_rest_create_guild_scheduled_event_params
dcc_rest_create_guild_sticker_multipart
dcc_rest_create_guild_sticker_params
dcc_rest_create_guild_template_params
dcc_rest_create_stage_instance_params
dcc_rest_delete_all_global_commands
dcc_rest_delete_all_guild_commands
dcc_rest_edit_global_command_builder
dcc_rest_edit_guild_command_builder
dcc_rest_edit_guild_command_permissions_params
dcc_rest_get_global_commands_with_localizations
dcc_rest_get_guild_commands_with_localizations
dcc_rest_get_guild_scheduled_event_users_page
dcc_rest_get_guild_scheduled_event_with_user_count
dcc_rest_get_guild_scheduled_events_with_user_count
dcc_rest_guild_message_search_params
dcc_rest_modify_application_emoji_params
dcc_rest_modify_auto_moderation_rule_params
dcc_rest_modify_guild_emoji_params
dcc_rest_modify_guild_onboarding_params
dcc_rest_modify_guild_scheduled_event_params
dcc_rest_modify_guild_template_params
dcc_rest_modify_stage_instance_params
```

Also remove these five Task 9 transition composites:

```text
dcc_rest_bulk_overwrite_application_commands
dcc_rest_bulk_overwrite_application_commands_builder
dcc_rest_create_application_command
dcc_rest_create_application_command_builder
dcc_rest_delete_all_application_commands
```

The complete current-public-symbol removal ledger is therefore 39: 30 legacy
aliases attached to active operations, two legacy aliases attached to stale
operations, two stale canonical symbols, and five transition composites. Keep
the active aliases and their owners in each endpoint's historical
`legacy_symbols` metadata; move the two stale endpoint/alias pairs into a
sorted top-level unavailable/removal record with reason, owner, method, and
route evidence; and retain the composite records with `removal_task: 9`.
Every removed symbol is classified exactly once without contributing an active
endpoint. The audit requires it to be absent from public declarations,
definitions, exports, and active canonical entries after Task 9.

The baseline arithmetic is explicit: 58 current canonical symbols plus 32
legacy symbols plus five composites equals 95; remove 39 and add the one
planned sticker canonical to leave the 57 active public endpoint symbols.
Empty arrays on each explicit global or guild bulk-overwrite canonical replace
all `delete_all` aliases. Consumers choose global or guild scope explicitly;
no scope-dispatch composite survives.

## Canonical endpoint shape and common call options

Every active endpoint has exactly this parameter order:

```c
dcc_status_t endpoint(
    dcc_client_t *client,
    /* required route scalars and strings, in route order, */
    /* const typed input/query and its adjacent count, when applicable, */
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);
```

IDs that occur in the path are scalar arguments and must not be duplicated in
payload types. Optional query groups and every JSON or multipart body use typed
input records; endpoint functions do not accept public JSON bodies. A null
query means Discord defaults. A required write body is never represented by a
null input.

Task 9 consumes the Task 7 extension of `dcc_rest_call_options_t` unchanged.
The common fields are named `audit_log_reason`,
`dcc_rest_auth_mode_t auth_mode` (`DEFAULT`, `NONE`, `BOT`, `BEARER`), and
`auth_token`; they have no presence bits. Historical Task 4 prefixes through
`user_data` remain valid, and normalization reads only size-covered fields.
Task 9 must use Task 7's private endpoint-capability policy for whether a route
allows an audit reason and which auth modes it allows or requires. It must not
add endpoint-specific reason/token parameters or reinterpret the common
fields.

`audit_log_reason` is the raw scalar defined by the Task 7 common contract:
callers do not percent-encode it. Task 7 validation, encoding, maximum length,
header-safety, copy-before-return, and redaction rules apply exactly once.
`NULL` omits the header. The Task 9 capability allowlist contains exactly 15
operations:

- Auto moderation: create, modify, and delete rule.
- Guild emoji: create, modify, and delete.
- Guild sticker: create, modify, and delete.
- Scheduled event: create and modify, but not delete.
- Stage instance: create, modify, and delete.
- Guild onboarding: modify.

```text
dcc_rest_create_auto_moderation_rule
dcc_rest_modify_auto_moderation_rule
dcc_rest_delete_auto_moderation_rule
dcc_rest_create_guild_emoji
dcc_rest_modify_guild_emoji
dcc_rest_delete_guild_emoji
dcc_rest_create_guild_sticker
dcc_rest_modify_guild_sticker
dcc_rest_delete_guild_sticker
dcc_rest_create_guild_scheduled_event
dcc_rest_modify_guild_scheduled_event
dcc_rest_create_stage_instance
dcc_rest_modify_stage_instance
dcc_rest_delete_stage_instance
dcc_rest_modify_guild_onboarding
```

Every other Task 9 operation locally rejects a non-null audit reason. The
endpoint audit compares this exact allowlist with manifest metadata and the
private capability table.

`dcc_rest_edit_guild_command_permissions` is Bearer-only. It requires
`DCC_REST_AUTH_BEARER` and a valid copied `auth_token`; default, none, Bot
auth, a missing token, and a malformed header value fail before admission.
The caller's OAuth2 user must have the required guild/role management
permission; Discord remains authoritative for that permission decision. No
Bearer token, generated Authorization header, audit reason, or body may appear
in diagnostics, observer metadata, or test failure logs.

The other application-command auth policies are exact. The 12 global/guild
command list/get/create/edit/delete/bulk-overwrite operations and the two
permission GETs default to Bot and accept `DEFAULT`, `BOT`, or `BEARER`; Bearer
there represents the application's client-credentials token. The five
application-emoji operations and all remaining 37 Task 9 resource operations
are Bot-only and accept `DEFAULT` or `BOT`. No Task 9 endpoint accepts `NONE`.
Thus the auth partition is 14 Bot-or-Bearer, one Bearer-only, and 42 Bot-only.

## Versioned Task 9 inputs

Every semantic Task 9 endpoint body, query, registry-configuration record, and
extensible nested input begins with this ABI prefix and publishes a version
constant, complete initializer macro, and null-safe initializer:

```c
size_t size;
uint32_t version;
uint64_t present;
```

A newly defined or wholly replaced record with any nullable member immediately
adds `uint64_t nulls`. It reuses that member's `present` bit: absent omits it,
present with a clear null bit reads and serializes its value, and present with
a set null bit emits null without reading the value storage. Reject unknown
null bits, null-without-present, and null on a nonnullable field. The already-
published Task 5 autocomplete/application-command builders retain their
version-1 prefix and existing default-permission null representation; append
the typed Task 9 fields and bits without shifting that transition layout.
Task 14 removes and compacts the mutually exclusive raw-JSON members in the
coordinated ABI-2 cut before the first stable release.

Use version 1. Larger version-1 records and historical prefixes are accepted
when the prefix covers every mandatory and present field. Reject zero/unknown
versions, unknown presence bits, present-but-uncovered fields, partial
pointer/count pairs, invalid booleans or enums, and all size/count arithmetic
overflow before allocation, output publication, or queue admission. An absent
bit omits a field; a present zero/false serializes zero/false. Nullable fields
use an explicit null state that is legal only with the field's presence bit.
A present array with count zero serializes `[]`; omission, null, and an empty
array are not conflated.

For arrays of versioned records, the first element's validated `size` is the
array stride. Require every element to have that same aligned stride and
supported version. Validate every element's readable prefix, presence/null
mask, and endpoint field mask using checked byte arithmetic before reading
nested pointers or serializing any element. Arrays of fixed semantic values
such as snowflakes or permission tuples use their fixed element size. Never
index a versioned array with `sizeof(current_type)` after accepting a
historical prefix.

The endpoint types include at least:

- `dcc_rest_command_list_query_t` for explicit `with_localizations`;
- versioned command-permission payload input without route IDs;
- the existing versioned `dcc_application_command_builder_t` and option
  builder, extended below;
- versioned auto-moderation rule, trigger metadata, and action inputs;
- `dcc_emoji_params_t` and `dcc_guild_sticker_params_t`;
- `dcc_rest_scheduled_event_query_t`,
  `dcc_rest_scheduled_event_users_query_t`, scheduled-event payload, recurrence
  rule, and nested recurrence inputs;
- stage-instance, onboarding top/prompt/option, template, and
  `dcc_message_search_params_t` inputs; and
- versioned/presence-gated `dcc_command_registry_options_t`.

Raw JSON compatibility members are not a substitute for typed fields. Existing
typed-builder JSON escape members may survive only where the Task 5 builder
contract explicitly retained them, with the same mutual-exclusion,
validation, and ownership rules.

### Shared application leaf types and typed replacement fields

Create one neutral `<dcc/application_types.h>` owner, included by autocomplete,
application-command, and Task 10 role-connection types. It owns exactly these
fixed semantic values:

```c
typedef struct dcc_localization {
    const char *locale;
    const char *value;
} dcc_localization_t;

typedef enum dcc_application_integration_type {
    DCC_APPLICATION_INTEGRATION_TYPE_GUILD_INSTALL = 0,
    DCC_APPLICATION_INTEGRATION_TYPE_USER_INSTALL = 1
} dcc_application_integration_type_t;

typedef enum dcc_interaction_context_type {
    DCC_INTERACTION_CONTEXT_GUILD = 0,
    DCC_INTERACTION_CONTEXT_BOT_DM = 1,
    DCC_INTERACTION_CONTEXT_PRIVATE_CHANNEL = 2
} dcc_interaction_context_type_t;
```

Remove the duplicate integration enum from the transition official-surface
header. A localization array is borrowed, fixed-stride, and ordered only for
deterministic serialization; locale keys must be nonempty, values must satisfy
the owning Discord field's constraints, and duplicate locale keys are invalid.
Integration/context arrays reject invalid values and duplicates. A null pointer
is valid only with count zero; checked span arithmetic precedes every read.

Append these exact seven typed pointer/count fields and seven semantic presence
values while the corresponding raw fields remain transition-only:

```c
/* dcc_autocomplete_choice_t */
const dcc_localization_t *name_localizations;
size_t name_localization_count;

/* dcc_application_command_option_builder_t */
const dcc_localization_t *name_localizations;
size_t name_localization_count;
const dcc_localization_t *description_localizations;
size_t description_localization_count;

/* dcc_application_command_builder_t */
const dcc_localization_t *name_localizations;
size_t name_localization_count;
const dcc_localization_t *description_localizations;
size_t description_localization_count;
const dcc_application_integration_type_t *integration_types;
size_t integration_type_count;
const dcc_interaction_context_type_t *contexts;
size_t context_count;
```

The word “seven” counts logical pointer/count fields: one choice, two option,
and four command fields. Publish these exact validated setters:

```c
dcc_status_t dcc_autocomplete_choice_set_name_localizations(
    dcc_autocomplete_choice_t *choice,
    const dcc_localization_t *localizations,
    size_t localization_count);
dcc_status_t dcc_application_command_option_builder_set_name_localizations(
    dcc_application_command_option_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count);
dcc_status_t dcc_application_command_option_builder_set_description_localizations(
    dcc_application_command_option_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count);
dcc_status_t dcc_application_command_builder_set_name_localizations(
    dcc_application_command_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count);
dcc_status_t dcc_application_command_builder_set_description_localizations(
    dcc_application_command_builder_t *builder,
    const dcc_localization_t *localizations,
    size_t localization_count);
dcc_status_t dcc_application_command_builder_set_integration_types(
    dcc_application_command_builder_t *builder,
    const dcc_application_integration_type_t *integration_types,
    size_t integration_type_count);
dcc_status_t dcc_application_command_builder_set_contexts(
    dcc_application_command_builder_t *builder,
    const dcc_interaction_context_type_t *contexts,
    size_t context_count);
```

Each typed bit is mutually exclusive with its corresponding raw-JSON bit.
`choices_json`/typed choices and `options_json`/typed options retain the same
rule. Task 9 endpoint and registry code uses only typed fields. Task 14 deletes
all ten autocomplete/command raw fields, their ten raw presence names, and the
six exported raw setters that exist for this subset; it does not delete the
JSON output build/free functions.

As a macro-hygiene prerequisite for the later Bot aggregate, convert every
autocomplete/application-command builder VERSION and PRESENT value, including
transition raw names and new Task 9 fields, from `#define` to an enum constant.
Do the same for the two request VERSION values. They are ordinary integral
constants and are never used by preprocessor conditionals. Keep only public
constant initializer macros; registry options/result/operation versions are
also enum constants, so Task 9 adds exactly three registry initializer macros,
not one VERSION macro per record.

## Application commands and permissions

List-global and list-guild commands take
`dcc_rest_command_list_query_t`; presence-gated `with_localizations` writes
both explicit `true` and explicit `false`. Bulk overwrite takes
`const dcc_application_command_builder_t *commands` followed by
`size_t command_count`; zero commands is valid and deletes the selected
scope's commands. Validate and serialize the entire array before admitting one
request.

Complete the Task 5 command builders for the current Discord schema:

- Add optional command `id` for bulk-overwrite identity preservation. Create
  and edit reject it; bulk create/update accepts it only as the official
  schema permits.
- Add the global `PRIMARY_ENTRY_POINT` command `handler` field and its official
  enum values APP_HANDLER 1 and DISCORD_LAUNCH_ACTIVITY 2. Reject it on guild
  routes and on non-entry-point command types; application Activity eligibility
  remains server-authoritative.
- Add string-option `min_length` and `max_length` and attachment-option
  `file_types` with at most 10 entries. Enforce option-type-specific field
  masks: minimum length is 0-6000, maximum length is 1-6000 and not below the
  present minimum, and each file type is `image`, `video`, `audio`, or a valid
  dot-prefixed extension.
- Enforce at most 25 options per level, 25 choices per option, and Discord's
  combined 8,000-character command name/description/choice-value limit.
- Enforce bulk scope counts by command type: at most 100 CHAT_INPUT, 15 USER,
  15 MESSAGE, and, globally only, one PRIMARY_ENTRY_POINT command. Names are
  unique per type within the selected scope.
- CHAT_INPUT commands enforce their name, description, nesting, required-
  before-optional, choices-versus-autocomplete, and localization contracts.
  USER and MESSAGE commands reject description and options. Entry-point
  commands enforce their handler/context contract.
- Global create/bulk/edit and guild create/bulk/edit each have an explicit
  field mask. Guild endpoints reject `contexts`, `integration_types`, and
  `handler`; edit endpoints reject `type`. Omitted optional fields stay
  omitted rather than being synthesized from zero values.

`dcc_rest_edit_guild_command_permissions` takes application, guild, and command
IDs as route scalars and a versioned payload containing only the permissions
array. The permissions presence bit is required; a zero count with a null
pointer serializes an explicit empty array for clear-all. It accepts zero
through 100 permission overwrites and validates every ID, type, and boolean.
It is the only mutable permission endpoint and uses the Bearer-only policy above.
The two GET permission endpoints retain normal endpoint-auth policy. The
disabled bulk permission route is not an alias or hidden helper.

## Sequential command-registry aggregate

`dcc_command_registry_fetch_remote` represents one physical list request. Its
public signature uses `dcc_rest_call_options_t` and returns an optional
`dcc_rest_request_t **`; it chooses the explicit global or guild list endpoint
from validated registry options. Callers obtain/parse the response with the
existing remote-snapshot parser. It never retains legacy `dcc_rest_cb` or
`user_data` arguments.

```c
dcc_status_t dcc_command_registry_fetch_remote(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *registry_options,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);
```

Make `dcc_command_registry_options_t` version 1 and presence-gated. An absent
guild ID means global scope; a present guild ID must be nonzero. Absent
`delete_stale` and `dry_run` mean false, while present booleans accept only 0
or 1. The existing setters mark the corresponding bit. Registry entries,
remote snapshots, diffs, and plans validate their public `size` contracts;
plans deep-copy normalized command builders and all JSON/name data required by
apply. Deinitializing the source registry, remote snapshot, plan, option
record, or caller strings immediately after a successful call is safe.

`dcc_command_registry_apply` returns an opaque
`dcc_command_registry_operation_t`. The public aggregate surface is fixed as:

```c
typedef struct dcc_command_registry_operation
    dcc_command_registry_operation_t;

typedef struct dcc_command_registry_operation_result {
    size_t size;
    uint32_t version;
    dcc_status_t status;
    size_t planned_count;
    size_t submitted_count;
    size_t succeeded_count;
    size_t create_completed;
    size_t update_completed;
    size_t delete_stale_completed;
    size_t noop_count;
    size_t failed_plan_index;
    dcc_command_registry_diff_action_t failed_action;
    const dcc_rest_result_t *failed_rest_result;
} dcc_command_registry_operation_result_t;

typedef void (*dcc_command_registry_operation_result_fn)(
    dcc_client_t *client,
    const dcc_command_registry_operation_result_t *result,
    void *user_data
);

typedef struct dcc_command_registry_operation_options {
    size_t size;
    uint32_t version;
    dcc_rest_priority_t priority;
    dcc_rest_auth_mode_t auth_mode;
    const char *auth_token;
    dcc_command_registry_operation_result_fn callback;
    void *user_data;
} dcc_command_registry_operation_options_t;

dcc_status_t dcc_command_registry_apply(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_command_registry_plan_t *plan,
    const dcc_command_registry_operation_options_t *operation_options,
    dcc_command_registry_operation_t **out_operation
);

dcc_status_t dcc_command_registry_operation_wait(
    dcc_command_registry_operation_t *operation,
    uint32_t timeout_ms,
    const dcc_command_registry_operation_result_t **out_result
);
dcc_status_t dcc_command_registry_operation_cancel(
    dcc_command_registry_operation_t *operation
);
uint8_t dcc_command_registry_operation_completed(
    const dcc_command_registry_operation_t *operation
);
void dcc_command_registry_operation_destroy(
    dcc_command_registry_operation_t *operation
);
```

`plan` is the sole scope/provenance authority for apply. Its deep-copied
`guild_id`, `delete_stale`, and `dry_run` values were normalized by
`dcc_command_registry_build_plan()` and cannot be overridden at execution.
There is deliberately no second `registry_options` argument whose global/guild
scope could disagree with remote IDs and diffs in the plan. Fetch and plan
construction still take registry options; apply validates the plan's own
version, ownership state, scope, and diff provenance before the first request.

Publish version values as enum constants and exactly three complete initializer
macros/static-inline initializers for registry options, operation options, and
operation result. `planned_count` counts executable create/update/delete
entries, while `noop_count` records skipped NOOP entries. A no-failure result
uses `SIZE_MAX` for `failed_plan_index`, `DCC_COMMAND_REGISTRY_NOOP` for
`failed_action`, and null for `failed_rest_result`. A later local admission
failure preserves that exact local status and has no REST result;
transport/HTTP failures use `dcc_rest_result_status()` and retain a clone;
cancellation reports `DCC_ERR_CANCELED`; complete apply/dry-run/no-op reports
`DCC_OK`. Null aggregate options mean normal priority, default Bot auth, and no
callback; Task 7's command-endpoint auth rules validate and copy an optional
client-credentials Bearer token for every leg and wipe it at final release.
Null output means auto-release. A non-OK submission publishes no operation and
runs no callback.
`DCC_OK` guarantees exactly one aggregate terminal result, including dry-run,
no-op, cancellation, teardown, and a later leg's admission failure.

Wait uses the Task 4 timeout/result-view convention: zero waits indefinitely,
a timeout leaves the handle usable, and terminal delivery makes wait return
`DCC_OK` while callers inspect the aggregate result's `status`. Cancel is
nonblocking and idempotent; completion may win the race. Destroy releases the
caller reference without waiting and cannot suppress the aggregate callback.
Completed on null is false, destroy on null is a no-op, and wait/cancel on null
return `DCC_ERR_INVALID_ARG`. Waiting on the same aggregate from its terminal
callback returns `DCC_ERR_STATE`. A successful wait is also the aggregate
callback-delivery barrier. The result view returned by wait remains operation-
owned until destroy; the callback view is borrowed until callback return, and
its nested failed REST result can be retained with `dcc_rest_result_clone()`.

The implementation is one sequential state machine, not a loop that submits
all diffs:

```text
INIT
  -> COMPLETE_ASYNC                 dry-run or no executable diffs
  -> SUBMIT_LEG(i)
       -> TERMINAL_ERROR            local validation/admission failure
       -> WAIT_LEG(i)
            -> TERMINAL_ERROR       HTTP/transport failure
            -> RECORD_SUCCESS
                 -> SUBMIT_LEG(i+1) next executable diff
                 -> COMPLETE_OK     last diff
  -> CANCEL_ACTIVE -> TERMINAL_CANCELED, unless completion already won
```

At most one physical REST request may exist at a time. Preserve deterministic
plan order, skip NOOP entries without a request, stop on the first local,
transport, or HTTP failure, and never submit later legs after failure or
cancellation. A leg callback records completion and schedules/submits the next
leg only after releasing request, registry, and operation locks. No wait,
endpoint submission, user callback, observer, or destruction occurs while an
operation mutex is held.

`WAIT_LEG` is a callback-driven logical state; it never calls
`dcc_rest_request_wait()` on a runtime worker. Cancellation marks the aggregate
under its mutex, takes a temporary reference to the active request, releases
the mutex, and calls `dcc_rest_request_cancel()`. If a leg is active, aggregate
terminal delivery waits for that leg's terminal callback to settle ownership;
if cancellation wins between legs, no next leg is submitted. Exactly one of
normal completion and cancellation publishes the aggregate result.

The operation state owns a client/runtime lifetime guard, application and
guild IDs, normalized options, a deep copy of the executable plan/builders,
current index and counts, first error, cloned failed result, aggregate callback
and user data, active request handle, cancellation/completion flags, mutex,
condition variable, and references needed for callback/destroy races. Publish
the output handle before terminal delivery becomes eligible. Dry-run and no-op
completion is dispatched asynchronously exactly once, never inline on the
`apply` call stack.

Sequential application is deliberate. Bounded parallel submission is rejected
because it loses fail-fast ordering and creates unnecessary rate-limit and
partial-failure ambiguity. Replacing a diff plan with unconditional bulk
overwrite is rejected because it changes delete/update semantics and result
accounting. A worker must never block waiting for another runtime request.

Migrate direct Task 9 consumers in the same change. The CLI may wait from its
non-runtime main thread before destroying the client. App/READY code must keep
the client/runtime guard and opaque operation state alive through terminal
completion. A successful `dcc_command_registry_apply()` has already deep-copied
the executable plan, builders, options, JSON, and names, so the source registry,
remote snapshot, diff/plan, and their strings may be deinitialized immediately
after that call; no App bridge may retain them merely for the operation. A
private bridge is allowed when needed to avoid exposing a new App mirror. Task
10 may later add a fetch-parse-plan-apply composite and retire internal mirror
use, but Task 9 must leave every current direct registry consumer correct and
compilable.

## Auto moderation

Update enums to the current official schema: remove the stale
`HARMFUL_LINK` trigger value 2, add `MEMBER_PROFILE` trigger value 6, add
`MEMBER_UPDATE` event value 2, and add `BLOCK_MEMBER_INTERACTION` action value
4. Unknown and removed values fail locally.

Create requires present name, event type, trigger type, and a non-empty actions
array. Modify makes writable fields optional and must reject `trigger_type`;
the rule ID is a route scalar, never a payload member. Enforce endpoint and
trigger/action-specific masks on every nested versioned record:

- keyword filters: at most 1,000 entries, each at most 60 characters;
- regular expressions: at most 10 entries, each at most 260 characters;
- allow lists: at most 100 entries for keyword/member-profile triggers or
  1,000 for keyword-preset triggers, each at most 60 characters;
- mention total limit: at most 50; exempt roles: at most 20; exempt channels:
  at most 50;
- timeout duration: at most 2,419,200 seconds; block-message custom text: at
  most 150 characters; and
- only fields defined for KEYWORD, SPAM, KEYWORD_PRESET, MENTION_SPAM, or
  MEMBER_PROFILE metadata and BLOCK_MESSAGE, SEND_ALERT, TIMEOUT, or
  BLOCK_MEMBER_INTERACTION actions may be present.

Create, modify, and delete are the three audit-capable operations. Both GETs
reject write-only input and audit reason.

## Emojis, stickers, and multipart

`dcc_emoji_params_t` is one versioned/presence-gated request record for name,
image, and roles; emoji IDs and application/guild IDs are route scalars.
Endpoint masks are exact:

- Guild create accepts required name/image and optional roles.
- Guild modify accepts optional name and roles, including distinct omitted,
  null, and empty-array states; it rejects image.
- Application create accepts required name/image and rejects roles.
- Application modify accepts name only.

Enforce official name rules, image data-URI/media rules, nonzero role IDs, and
the 256 KiB emoji-upload limit before admission. Only guild create/modify/delete
permit audit reason; application emoji operations do not.

`dcc_rest_create_guild_sticker` is the only Task 9 multipart operation and the
only planned canonical addition. Its `dcc_guild_sticker_params_t` contains
versioned/presence-gated name, description, tags, and one fixed file record;
guild ID remains a route scalar. Create requires a 2-30 character name, a
description that is empty or 2-100 characters, tags no longer than 200
characters, an official supported sticker media type, and at most 512 KiB of
file data. Validate file pointer/length pairs and reject CR/LF in filename or
content type. Build the complete multipart body, including exact embedded-NUL
bytes, before admission.

Sticker modify is JSON and accepts optional name, nullable description, and
tags. It rejects every file-only field. Omitted, explicit null, empty string,
and a non-empty scalar retain their documented distinct meanings. Guild
sticker create/modify/delete are audit-capable; sticker and pack GETs are not.
No public `_multipart` or `_params` wrapper remains.

## Scheduled events

`dcc_scheduled_event_params_t` contains only writable request members; guild,
event, creator, and entity response IDs do not appear in it. Add a typed,
versioned recurrence rule and typed nested recurrence elements. Exclude the
server-only or externally unsettable `end`, `by_year_day`, and `count` fields
from the write surface. Validate ISO-8601 timestamps and every recurrence enum,
interval, array, and pointer/count pair before serialization.

Create enforces the official entity matrix: voice/stage events require a
channel and reject external location; external events require no channel, a
location, and an end time. Name, privacy, entity type, and start time follow
the official required/default rules. Modify presence-gates every writable
field and supports documented null states for channel, description, image, and
recurrence. Locally accept only writable target statuses and document the
valid transitions—scheduled to active or cancelled, and active to completed;
Discord validates the source state because it is not part of the request. A
supplied entity-type change must provide enough covered fields to validate its
new entity matrix; do not guess omitted server state.

`dcc_rest_scheduled_event_query_t` presence-gates `with_user_count` for both
single and list GETs, including explicit false. The users query presence-gates
`limit` (1-100), `with_member`, `before`, and `after`; reject simultaneous
before and after locally for deterministic behavior. Create and modify allow
audit reason; delete and all GETs reject it.

## Stage instances

Use a versioned/presence-gated `dcc_stage_instance_params_t` with endpoint
field masks. Create requires body `channel_id` and topic, permits optional
GUILD_ONLY privacy (the default), `send_start_notification`, and
`guild_scheduled_event_id`, and validates topic length 1-120. Modify takes
channel ID as its route scalar and, under the current official schema, permits
only optional `privacy_level`; it rejects create-only fields. The deprecated
PUBLIC privacy level is rejected everywhere. Create, modify, and delete all
permit audit reason; GET does not.

## Guild onboarding

Make the top-level onboarding input and every prompt and prompt-option element
versioned and presence-gated. Use the first validated element size as stride
for each nested array. Route guild ID is not in the body. Write emoji data as
the official scalar members `emoji_id`, `emoji_name`, and `emoji_animated`, not
as the current unrelated emoji upload object. Validate prompt type, title,
flags, option titles/descriptions, all snowflake arrays, and pointer/count
pairs.

Optional prompt/option IDs preserve server identity. Present zero-length prompt,
option, default-channel, channel, and role arrays serialize as `[]` so callers
can clear them. Presence-gated booleans serialize explicit false. Locally
validate structural constraints that do not require guild state. Discord must
validate the enable-time requirement for at least seven default channels and
at least five channels that allow everyone to send, because permissions and
channel state are unavailable to this request builder. Modify is the one
audit-capable onboarding operation; GET rejects audit reason.

## Guild templates

Template codes are route strings and must be non-empty and percent-encoded as
one path segment before admission. `dcc_template_params_t` is a versioned body
record with no guild ID or code. Create requires a 1-100 character name and
allows an omitted or nullable 0-120 character description. Modify permits
optional name and nullable description and requires at least one writable
field. Sync and delete are scalar-only. None of the six active template
operations accepts audit reason. Do not retain or privately expose Create Guild
from Template.

## Guild message search

Replace every `has_*` flag in `dcc_message_search_params_t` with version and
presence semantics. Support the current positive and negated repeatable
filters; a negated filter must not be silently serialized as its positive
counterpart. Percent-encode each query element independently and preserve
repeat order.

Enforce limit 1-25, offset at most 9,975, slop at most 100, content at most
1,024 characters, at most 500 channel IDs, at most 100 entries for the other
major repeatable arrays, all official per-string maxima, valid enums, and all
pointer/count pairs. Explicit false booleans remain in the query when present.
An HTTP 202 response with Discord code 110000 and `retry_after` is a normal
terminal REST result, not a synchronous retry loop or local submission error.

## Ownership and admission contract

On entry, every endpoint and registry submitter sets a supplied output handle
to null. Before queue admission, validate the entire covered graph and copy or
serialize all route strings, query strings, JSON, nested arrays, builder data,
localization strings, recurrence data, audit reason, auth token/header, file
metadata, and exact multipart bytes needed after return. Caller mutation or
free immediately after `DCC_OK` must be harmless. Callback function pointers
and their `user_data` are the only intentionally borrowed values through
terminal callback return.

The rules by return path are:

| Return path | Queue/handle | Callback/observer | Ownership |
| --- | --- | --- | --- |
| local validation/allocation/admission error | no queued work; output null | none | no retained caller input |
| `DCC_OK`, retained output | caller owns handle | exactly one terminal delivery | worker owns serialized/copied request; caller destroys handle |
| `DCC_OK`, null output | auto-release | exactly one terminal delivery if configured; observer still sees failure | DCC releases after delivery |
| aggregate later-leg rejection | aggregate remains valid and becomes terminal | one aggregate callback; no callback for rejected leg | aggregate retains first error and prior counts |

No endpoint callback or observer runs for local rejection. After `DCC_OK`,
exactly one terminal result is required for success, HTTP error, transport
error, cancellation, and teardown. Follow the Task 4 retained-handle,
callback-barrier, cancellation, and destroy rules and the Task 7 raw audit/auth
ownership rules. Never perform network work, interceptor work, or a wait on the
submission stack.

## Required RED matrix

Register focused Task 9 endpoint and command-registry tests before production
changes. Capture RED for at least:

- inventory audit: active 57, active legacy 30, multipart 1, audit 15, stale
  canonicals 2, stale-attached aliases 2, composites 5, full removal ledger
  39, and planned addition 1;
- progress audit: `--progress-through 9` reports 180/224 and deferred 44;
  strict mode fails only on exact Task 10 debt, and stale allowances cannot
  remain when a domain has no violation;
- exact method, literal normalized route, ordered signature, declared input,
  implementation owner, auth capability, audit capability, and multipart bit
  for every one of the 57 active canonicals;
- absence from public headers, definitions, and exports of all 39 removed
  symbols; each remains classified exactly once in historical/removal metadata,
  while message search remains an active Task 9 endpoint;
- larger version-1 records, each accepted historical prefix, unknown version
  and bit rejection, present-but-uncovered rejection, nested historical-array
  stride, boolean/null/empty semantics, overflow, and mutation/free after
  return for every input family;
- Task 7 raw-scalar audit behavior on all 15 allowed endpoints and rejection on
  all 42 others, including percent-encoding exactly once and caller mutation;
- permission edit emitting exactly one redacted `Authorization: Bearer ...`
  header, rejecting Bot/default/none/missing/malformed auth, and copying the
  token before return;
- global/guild command field masks, create/edit/bulk differences, optional
  bulk ID, entry-point handler, min/max string length, file types, nested
  option/choice/8,000-character boundaries, localization query true/false, and
  zero-count bulk deletion;
- permission arrays at 0/100/101, malformed entries, and proof that the disabled
  bulk permission route cannot be reached;
- every auto-moderation trigger/event/action matrix and all boundary counts and
  lengths, including rejected trigger modification and removed trigger value
  2;
- application/guild emoji endpoint masks, null/empty roles, data URI and 256
  KiB boundaries, plus sticker JSON/multipart masks and exact 512 KiB embedded-
  NUL file ownership;
- scheduled-event entity, recurrence, nullable-field, status-transition, and
  query boundaries, including explicit false and before-plus-after rejection;
- stage create/modify masks and deprecated PUBLIC rejection; onboarding nested
  stride, scalar emoji fields, clear arrays, false booleans, and locally
  knowable constraints; template name/description/code encoding and absence of
  create-from-template;
- message-search positive/negated filters, repeated-array caps, query encoding,
  numeric/string limits, explicit false, and HTTP 202/code 110000 delivery;
- delayed command fetch and apply, strictly one in-flight leg, deterministic
  order, fail-fast HTTP/transport error, later admission failure, cancellation
  at every transition, completion-versus-cancel race, dry-run, all-NOOP,
  callback plus retained handle, callback self-wait rejection, immediate source
  teardown, client teardown, exactly-once delivery, and leak/race checks.

Use the local HTTP server/interceptor only; tests never call live Discord.
Snapshot request, callback, observer, and aggregate counters before every local
rejection.

## Migration and implementation order

1. Rebase manifest/audit truth onto the Task 8 active total of 224. Add the
   stale/removal ledger assertions and RED signature/route/auth/audit tests.
2. Add or extend versioned inputs and command builders with initializer,
   historical-prefix, nested-stride, and endpoint-mask tests before changing
   endpoint declarations.
3. Convert application-command, permission, application-emoji, and auto-
   moderation endpoints, then remove their overloads and composites.
4. Implement the command-registry operation state machine and migrate CLI/App
   direct consumers without touching Task 10 public App mirrors.
5. Convert guild emoji/sticker and exact multipart ownership, then scheduled
   events, stage instances, onboarding, templates, and message search.
6. Remove all remaining Task 9 ledger symbols and stale sources/build entries;
   update installed-header, package, documentation, and release-surface audits.
7. Run the complete verification matrix and record exact RED/GREEN evidence.

Private shared serializers, endpoint capability tables, and state-machine
helpers are preferred over duplicated wrappers. Do not retain a removed public
function as an uninstalled declaration, weak symbol, macro alias, or hidden
compatibility export. Do not change an active route's semantics merely to reuse
an existing serializer.

## Verification and handoff

Run at minimum:

- endpoint audit self-tests, progress-through-9 audit, and strict expected-
  debt assertion;
- focused Task 9 endpoint/auth/audit/multipart and command-registry suites;
- complete HTTP smoke, request/result, App/READY, command registry, official
  surface, transition surface, package-consumer, installed-header, source, and
  release audits;
- standalone every changed public header as strict C11 and C++17;
- i686 and x86_64 MinGW header plus changed production-source probes;
- focused ASan/UBSan for all typed serializers, ownership, multipart, and
  aggregate cancellation/teardown, plus TSan for request/aggregate races;
- full enabled CTest, MkDocs strict, install/package consumer, and
  `git diff --check`.

The implementation report beside this brief must contain the exact final 57
canonical list, 39-symbol removal ledger, manifest/domain/method/audit/
multipart counts, 180/224 progress output and exact deferred 44 list, RED then
GREEN commands/results, ownership and auth matrices, sequential state-machine
race evidence, platform/sanitizer evidence, known baseline debt, and the
implementation hash. The implementation commit message is
`feat: migrate command REST endpoints to requests`; commit the report
separately and stop for review. Do not push.

Official contract sources:
[application commands](https://docs.discord.com/developers/interactions/application-commands),
[auto moderation](https://docs.discord.com/developers/resources/auto-moderation),
[emoji](https://docs.discord.com/developers/resources/emoji),
[sticker](https://docs.discord.com/developers/resources/sticker),
[scheduled events](https://docs.discord.com/developers/resources/guild-scheduled-event),
[stage instances](https://docs.discord.com/developers/resources/stage-instance),
[guild onboarding](https://docs.discord.com/developers/resources/guild),
[guild templates](https://docs.discord.com/developers/resources/guild-template),
[message search](https://docs.discord.com/developers/resources/message),
[audit-log reasons](https://docs.discord.com/developers/resources/audit-log),
and the [change log](https://docs.discord.com/developers/change-log).
