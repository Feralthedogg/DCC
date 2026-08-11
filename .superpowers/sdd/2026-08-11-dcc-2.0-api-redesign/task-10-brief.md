# Task 10 brief — finish REST, migrate composites, and retire App mirror use

Implement only Task 10 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The approved
design, the completed Task 6 request/result contract, the reviewed Task 7–9
briefs, and this brief are authoritative. This brief supersedes the original
231-entry candidate arithmetic and the original Task 10 count of 45. Work
test-first, do not push, and do not edit the ignored progress ledger. Keep
intermediate commits buildable.

## Outcome and boundaries

Complete the last 44 active Discord REST operations, make strict endpoint
auditing unconditional, and leave no internal consumer of an App REST mirror.
The installed REST surface after this task is exactly 224 canonical Discord
endpoint symbols plus 60 generic operations, for 284 exported `dcc_rest_*`
operations. Tasks 6 through 10 own exactly 41, 35, 47, 57, and 44 active
endpoints. Exactly 196 `dcc_app_*` REST compatibility exports remain frozen
until the Task 14/15 remaining-compatibility cutover; they are not part of the
284-operation `dcc_rest_*` inventory.

Task 10 removes the obsolete bot-facing Get Current User DMs candidate, all 28
Task 10 legacy endpoint spellings, `dcc_rest_official_body_json_free`, and the
two direct-message create-and-send composite symbols. It freezes, but does not
remove, exactly 196 App REST mirror declarations and definitions. Task 14 owns
their physical deletion together with the ten legacy `dcc_app_*` operations
implemented by `src/app/app_messages.c`. This task rewrites the retained
managed-message, command-sync, and send-with-thread consumers so none borrows
stack state or blocks a runtime worker.

Do not start Task 11 interaction-queue semantics, Task 12 Components v2 work,
or Task 14 wholesale DCC 1/Sugar deletion. `src/interactions/` is inbound HTTP
handling and has no outbound multi-request flow; keep its private interaction
response serializer private. Change it only if a removed symbol is an actual
compile-time dependency.

## Authoritative active inventory

The Task 10 domain counts are exact:

| Domain | Count |
| --- | ---: |
| applications | 3 |
| direct_messages | 0 |
| entitlements | 6 |
| gateway | 2 |
| group_dms | 2 |
| lobbies | 13 |
| role_connections | 5 |
| skus | 2 |
| soundboard | 2 |
| users | 4 |
| voice_states | 5 |
| **Total** | **44** |

Every row below has `audit_log_reason: false`. `BOT_OR_BEARER` means both modes
are allowed and `DEFAULT` resolves to Bot. A Bearer-only endpoint requires a
covered, explicit `BEARER` mode and non-empty per-call token; resolving
`DEFAULT` to Bearer without a token is a local rejection.

| Canonical symbol | Method and normalized route | Auth policy | Typed input |
| --- | --- | --- | --- |
| `dcc_rest_get_activity_instance` | `GET /applications/{application.id}/activity-instances/{instance_id}` | BOT | none |
| `dcc_rest_get_current_application` | `GET /applications/@me` | BOT | none |
| `dcc_rest_modify_current_application` | `PATCH /applications/@me` | BOT | `dcc_application_modify_params_t` |
| `dcc_rest_consume_entitlement` | `POST /applications/{application.id}/entitlements/{entitlement.id}/consume` | BOT | none |
| `dcc_rest_create_test_entitlement` | `POST /applications/{application.id}/entitlements` | BOT | `dcc_test_entitlement_params_t` |
| `dcc_rest_delete_test_entitlement` | `DELETE /applications/{application.id}/entitlements/{entitlement.id}` | BOT | none |
| `dcc_rest_get_entitlement` | `GET /applications/{application.id}/entitlements/{entitlement.id}` | BOT | none |
| `dcc_rest_get_entitlements` | `GET /applications/{application.id}/entitlements` | BOT | `dcc_rest_entitlement_query_t` |
| `dcc_rest_get_skus` | `GET /applications/{application.id}/skus` | BOT | none |
| `dcc_rest_get_gateway` | `GET /gateway` | NONE | none |
| `dcc_rest_get_gateway_bot` | `GET /gateway/bot` | BOT | none |
| `dcc_rest_add_group_dm_recipient` | `PUT /channels/{channel.id}/recipients/{user.id}` | BEARER | `dcc_group_dm_recipient_params_t` |
| `dcc_rest_remove_group_dm_recipient` | `DELETE /channels/{channel.id}/recipients/{user.id}` | BEARER | none |
| `dcc_rest_add_lobby_member` | `PUT /lobbies/{lobby.id}/members/{user.id}` | BOT | `dcc_lobby_member_params_t` |
| `dcc_rest_bulk_update_lobby_members` | `POST /lobbies/{lobby.id}/members/bulk` | BOT | `dcc_lobby_member_bulk_params_t` |
| `dcc_rest_create_lobby` | `POST /lobbies` | BOT | `dcc_lobby_params_t` |
| `dcc_rest_create_or_join_lobby` | `PUT /lobbies` | BEARER | `dcc_lobby_create_or_join_params_t` |
| `dcc_rest_delete_lobby` | `DELETE /lobbies/{lobby.id}` | BOT | none |
| `dcc_rest_get_lobby` | `GET /lobbies/{lobby.id}` | BOT | none |
| `dcc_rest_get_lobby_messages` | `GET /lobbies/{lobby.id}/messages` | BEARER | `dcc_rest_lobby_message_query_t` |
| `dcc_rest_leave_lobby` | `DELETE /lobbies/{lobby.id}/members/@me` | BEARER | none |
| `dcc_rest_link_lobby_channel` | `PATCH /lobbies/{lobby.id}/channel-linking` | BEARER | `dcc_lobby_channel_link_params_t` |
| `dcc_rest_modify_lobby` | `PATCH /lobbies/{lobby.id}` | BOT | `dcc_lobby_params_t` |
| `dcc_rest_remove_lobby_member` | `DELETE /lobbies/{lobby.id}/members/{user.id}` | BOT | none |
| `dcc_rest_send_lobby_message` | `POST /lobbies/{lobby.id}/messages` | BEARER | `dcc_lobby_message_params_t` |
| `dcc_rest_update_lobby_message_moderation_metadata` | `PUT /lobbies/{lobby.id}/messages/{message.id}/moderation-metadata` | BOT | `dcc_lobby_message_moderation_metadata_params_t` |
| `dcc_rest_delete_current_user_application_role_connection` | `DELETE /users/@me/applications/{application.id}/role-connection` | BEARER | none |
| `dcc_rest_get_application_role_connection_metadata` | `GET /applications/{application.id}/role-connections/metadata` | BOT | none |
| `dcc_rest_get_current_user_application_role_connection` | `GET /users/@me/applications/{application.id}/role-connection` | BEARER | none |
| `dcc_rest_update_application_role_connection_metadata` | `PUT /applications/{application.id}/role-connections/metadata` | BOT | `dcc_application_role_connection_metadata_params_t[]` plus count |
| `dcc_rest_update_current_user_application_role_connection` | `PUT /users/@me/applications/{application.id}/role-connection` | BEARER | `dcc_application_role_connection_params_t` |
| `dcc_rest_get_sku_subscription` | `GET /skus/{sku.id}/subscriptions/{subscription.id}` | BOT_OR_BEARER | none |
| `dcc_rest_get_sku_subscriptions` | `GET /skus/{sku.id}/subscriptions` | BOT_OR_BEARER | `dcc_rest_sku_subscription_query_t` |
| `dcc_rest_get_default_soundboard_sounds` | `GET /soundboard-default-sounds` | BOT | none |
| `dcc_rest_send_soundboard_sound` | `POST /channels/{channel.id}/send-soundboard-sound` | BOT | `dcc_soundboard_send_params_t` |
| `dcc_rest_get_current_user` | `GET /users/@me` | BOT_OR_BEARER | none |
| `dcc_rest_get_current_user_connections` | `GET /users/@me/connections` | BEARER | none |
| `dcc_rest_get_user` | `GET /users/{user.id}` | BOT | none |
| `dcc_rest_modify_current_user` | `PATCH /users/@me` | BOT | `dcc_current_user_params_t` |
| `dcc_rest_get_current_user_voice_state` | `GET /guilds/{guild.id}/voice-states/@me` | BOT | none |
| `dcc_rest_get_user_voice_state` | `GET /guilds/{guild.id}/voice-states/{user.id}` | BOT | none |
| `dcc_rest_get_voice_regions` | `GET /voice/regions` | BOT | none |
| `dcc_rest_modify_current_user_voice_state` | `PATCH /guilds/{guild.id}/voice-states/@me` | BOT | `dcc_current_user_voice_state_params_t` |
| `dcc_rest_modify_user_voice_state` | `PATCH /guilds/{guild.id}/voice-states/{user.id}` | BOT | `dcc_user_voice_state_params_t` |

The policy totals are exactly 29 BOT, 11 BEARER, 3 BOT_OR_BEARER (the reviewed
EITHER class), and 1 NONE. Do not silently send a Bot header to a Bearer or
no-auth operation.

Keep each active record's reviewed header/source owner already present in the
manifest. The only ownership correction is
`dcc_rest_get_current_application`: its one public declaration belongs in
`include/dcc/rest/resources/users.h`; remove the duplicate transition
declaration from `include/dcc/rest/official_surface.h`. Every path ID is a
nonzero scalar argument. `instance_id` is a required non-empty string segment
that is escaped and copied before return.

## Remove obsolete Get Current User DMs

Delete the active manifest entry, declaration, external definition, route
catalog identity, route macro, tests, and documentation for
`dcc_rest_get_current_user_dms` (`GET /users/@me/channels`). The current
[User resource](https://docs.discord.com/developers/resources/user) no longer
documents this GET operation. Discord's
[June 19, 2018 changelog](https://docs.discord.com/developers/change-log)
states that the response is an empty array for a bot user. It is removal-ledger
evidence, not an active DCC 2 endpoint.

Do not delete `src/rest/rest_users_dms.c`: the active Task 7
`dcc_rest_create_dm_channel` operation uses `POST /users/@me/channels` and
remains. Remove only the stale GET implementation and its now-unused
`DCC_REST_ROUTE_DPP_CURRENT_USER_GET_DMS` identity.

## Common options, authentication, and ownership

Extend the version-1 call-options layout exactly as approved:

```c
typedef enum dcc_rest_auth_mode {
    DCC_REST_AUTH_DEFAULT = 0,
    DCC_REST_AUTH_NONE = 1,
    DCC_REST_AUTH_BOT = 2,
    DCC_REST_AUTH_BEARER = 3
} dcc_rest_auth_mode_t;

typedef struct dcc_rest_call_options {
    size_t size;
    uint32_t version;
    dcc_rest_priority_t priority;
    dcc_rest_result_fn callback;
    void *user_data;
    const char *audit_log_reason;
    dcc_rest_auth_mode_t auth_mode;
    const char *auth_token;
    uint64_t flags;
} dcc_rest_call_options_t;
```

There is no call-options presence mask. The mandatory historical prefix ends
at `user_data`. Normalize each covered field into current-layout private state;
do not require `size >= sizeof(current)` and do not assign the caller's entire
struct. An uncovered suffix means null audit reason, `DEFAULT`, null auth
token, and zero flags. Reject partial field coverage. `BEARER` requires both the mode and token
fields to be covered and a non-empty token. `BOT` always uses the client token
and rejects a non-null override token. `NONE` and `DEFAULT` also reject a
non-null token. The only known flags remain
`DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY` and
`DCC_REST_CALL_FLAG_SENSITIVE_RESULT_BODY`; reject unknown covered bits.

The typed endpoint helper receives a private auth capability. BOT permits
DEFAULT/BOT, NONE permits DEFAULT/NONE, BEARER permits only a usable BEARER,
and BOT_OR_BEARER permits DEFAULT/BOT/BEARER. Unsupported modes fail locally
before allocation, queue admission, callback, or observer delivery. The raw
relative-Discord default is Bot. Absolute URLs accept no audit reason or auth
override and never receive a client credential.

Normalize the outer `dcc_rest_request_desc_t` by covered fields too. Its
declared size gates the optional options pointer; a non-null pointed options
record is independently size/version-normalized. Update the endpoint preparation path,
`dcc_rest_submit()`, the async request constructor/storage, HTTP preparation,
and header construction together. A queued Bearer token and encoded audit
reason are owned copies; wipe the Bearer token before free and never put it in
logs, errors, route keys, result bodies, or observer data.

`audit_log_reason` is unencoded canonical UTF-8 containing 1 through 512
Unicode scalar values. Percent-encode its UTF-8 bytes exactly once for
`X-Audit-Log-Reason`. All 44 Task 10 operations reject it because their
manifest capability is false. Consume the reviewed Task 7–9 auth and
audit-reason fields without weakening them; strict mode checks those fields on
all 224 active records.

All canonical endpoint functions retain the Task 6 call contract. They borrow
the input record, nested strings/maps/arrays, and call options only until the
function returns. Before returning `DCC_OK`, they copy or serialize every path,
query, body, audit reason, Bearer token, and exact byte sequence needed by the
worker. Callback and callback `user_data` are the only intentionally borrowed
values retained to terminal callback return. Local rejection clears a supplied
`*out_request`, queues nothing, and invokes neither callback nor observer;
`DCC_OK` guarantees one terminal result.

The endpoint manifest unconditionally forces
`DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY` for
`dcc_rest_add_group_dm_recipient` (`access_token`) and
`dcc_rest_create_or_join_lobby` (`secret`). This is ORed with caller flags and
cannot be disabled. Validation/measurement, exact serialization, interceptor
views, async ownership, every retired buffer, cancellation/rollback, and final
cleanup propagate the effective bit and securely wipe the full allocated body
capacity. Add allocator-hook tests for both endpoints and for a historical
call-options prefix whose uncovered flags default to zero but cannot suppress
manifest sensitivity.

## Exact caller-owned records

There are exactly 19 distinct non-null top-level Task 10 manifest input
spellings after replacing `dcc_rest_id_page_t` for lobby messages and splitting
the two voice-state bodies. Every evolvable record below starts with
`size_t size`, `uint32_t version`, and `uint64_t present`, has version `1U`, a
complete constant initializer, and a null-safe `static inline` initializer.
Presence constants occupy consecutive bits in the field order stated below.
Unknown bits and present-but-uncovered fields are invalid. Larger covered
version-1 records and historical prefixes covering every required/present field
are accepted.

Use this fixed leaf for typed dictionaries; it is not independently evolvable:

```c
typedef struct dcc_rest_string_map_entry {
    const char *key;
    const char *value;
} dcc_rest_string_map_entry_t;

typedef struct dcc_rest_string_map {
    size_t size;
    uint32_t version;
    uint64_t present; /* must be zero in version 1 */
    const dcc_rest_string_map_entry_t *entries;
    size_t entry_count;
} dcc_rest_string_map_t;
```

The map itself has version `1U`, an initializer/macro, checked pointer/count
arithmetic, canonical UTF-8 keys and values, and duplicate-key rejection.
Where a parent field is nullable, an absent parent presence bit omits the JSON
member, a present bit plus null map pointer emits JSON `null`, and a present bit
plus a valid zero-entry map emits `{}`. Where null is not documented, a present
bit plus null map is invalid.

### User, voice, group-DM, entitlement, and subscription records

```c
typedef struct dcc_current_user_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* USERNAME, AVATAR, BANNER */
    const char *username;
    const char *avatar;
    const char *banner;
} dcc_current_user_params_t;

typedef struct dcc_current_user_voice_state_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* CHANNEL_ID, SUPPRESS, REQUEST_TO_SPEAK_TIMESTAMP */
    dcc_snowflake_t channel_id;
    uint8_t suppress;
    const char *request_to_speak_timestamp;
} dcc_current_user_voice_state_params_t;

typedef struct dcc_user_voice_state_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* CHANNEL_ID, SUPPRESS */
    dcc_snowflake_t channel_id;
    uint8_t suppress;
} dcc_user_voice_state_params_t;

typedef struct dcc_group_dm_recipient_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* must be zero in version 1 */
    const char *access_token;
    const char *nick;
} dcc_group_dm_recipient_params_t;

typedef enum dcc_test_entitlement_owner_type {
    DCC_TEST_ENTITLEMENT_OWNER_GUILD = 1,
    DCC_TEST_ENTITLEMENT_OWNER_USER = 2
} dcc_test_entitlement_owner_type_t;

typedef struct dcc_test_entitlement_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* must be zero in version 1 */
    dcc_snowflake_t sku_id;
    dcc_snowflake_t owner_id;
    dcc_test_entitlement_owner_type_t owner_type;
} dcc_test_entitlement_params_t;

typedef struct dcc_rest_entitlement_query {
    size_t size;
    uint32_t version;
    uint64_t present; /* USER_ID, SKU_IDS, BEFORE, AFTER, LIMIT,
                         GUILD_ID, EXCLUDE_ENDED, EXCLUDE_DELETED */
    dcc_snowflake_t user_id;
    const dcc_snowflake_t *sku_ids;
    size_t sku_id_count;
    dcc_snowflake_t before;
    dcc_snowflake_t after;
    uint8_t limit;
    dcc_snowflake_t guild_id;
    uint8_t exclude_ended;
    uint8_t exclude_deleted;
} dcc_rest_entitlement_query_t;

typedef struct dcc_rest_sku_subscription_query {
    size_t size;
    uint32_t version;
    uint64_t present; /* BEFORE, AFTER, LIMIT, USER_ID */
    dcc_snowflake_t before;
    dcc_snowflake_t after;
    uint8_t limit;
    dcc_snowflake_t user_id;
} dcc_rest_sku_subscription_query_t;
```

Rename the current-user field from the incorrect `nickname` spelling to
`username`. Username is 2–32 Unicode scalar values when present. Avatar and
banner are nullable: present plus null serializes JSON null. Allow `{}` because
all three fields are optional.

Guild and user IDs are path arguments for voice operations and never appear in
the body records. All voice body fields are optional; a present channel ID is
nonzero, `suppress` is exactly 0 or 1, and a present null request timestamp
serializes JSON null. The other-user record cannot express a request-to-speak
timestamp.

Group-DM channel/user IDs are path arguments only. Its body requires a
non-empty access token and a non-null nickname and copies both. Test-entitlement
`application_id` is a path argument only; SKU and owner IDs are nonzero and
owner type is exactly 1 or 2.

Entitlement query order and names are exactly `user_id`, `sku_ids`, `before`,
`after`, `limit`, `guild_id`, `exclude_ended`, `exclude_deleted`. SKU IDs are a
comma-delimited nonzero-snowflake set, limit is 1–100, and a present boolean is
serialized even when false. Do not invent a before/after mutual-exclusion rule.
Subscription query order is exactly `before`, `after`, `limit`, `user_id`, with
limit 1–100. A Bot or DEFAULT list-subscriptions call requires a present,
nonzero `user_id`; a Bearer call permits omission. Do not add a query argument
to the singular Get SKU Subscription operation.

### Application records

Retain the existing public type names but make all three records versioned and
presence-driven:

```c
typedef struct dcc_application_install_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* SCOPES, PERMISSIONS */
    const char *const *scopes;
    size_t scope_count;
    const char *permissions;
} dcc_application_install_params_t;

typedef struct dcc_application_integration_type_config {
    size_t size;
    uint32_t version;
    uint64_t present; /* OAUTH2_INSTALL_PARAMS */
    dcc_application_integration_type_t integration_type;
    const dcc_application_install_params_t *oauth2_install_params;
} dcc_application_integration_type_config_t;

typedef struct dcc_application_modify_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* bits follow the field order below */
    const char *custom_install_url;
    const char *description;
    const char *role_connections_verification_url;
    const dcc_application_install_params_t *install_params;
    const dcc_application_integration_type_config_t *integration_types_config;
    size_t integration_type_config_count;
    uint64_t flags;
    const char *icon;
    const char *cover_image;
    const char *interactions_endpoint_url;
    const char *const *tags;
    size_t tag_count;
    const char *event_webhooks_url;
    dcc_application_event_webhook_status_t event_webhooks_status;
    const char *const *event_webhooks_types;
    size_t event_webhooks_type_count;
} dcc_application_modify_params_t;
```

Remove every `has_*` byte; `present` is authoritative. Nested pointer/count
groups must be fully covered. Integration config arrays use the first element's
declared size as checked stride; every element has the same size/version.
Integration types are unique and exactly Guild Install (0) or User Install
(1). A supplied install-params object has both covered/present `scopes` and
`permissions`; scopes use a valid pointer/count pair and permissions is a
non-empty decimal string. Tags number at most five and each tag is at most 20
Unicode scalar values.
Request event-webhook status accepts only Disabled (1) or Enabled (2), never
Disabled by Discord (3). `icon` and `cover_image` are nullable image-data
fields. Request flags are limited to the three documented limited-intent bits.

### Lobby records

Keep the existing lobby type names, add version/presence, and replace every
opaque `*_metadata_json` member with `const dcc_rest_string_map_t *`:

```c
typedef struct dcc_lobby_create_or_join_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* IDLE_TIMEOUT_SECONDS, LOBBY_METADATA, MEMBER_METADATA */
    const char *secret;
    uint32_t idle_timeout_seconds;
    const dcc_rest_string_map_t *lobby_metadata;
    const dcc_rest_string_map_t *member_metadata;
} dcc_lobby_create_or_join_params_t;

typedef struct dcc_lobby_member_spec {
    size_t size;
    uint32_t version;
    uint64_t present; /* METADATA, FLAGS */
    dcc_snowflake_t user_id;
    const dcc_rest_string_map_t *metadata;
    uint64_t flags;
} dcc_lobby_member_spec_t;

typedef struct dcc_lobby_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* METADATA, MEMBERS, IDLE_TIMEOUT_SECONDS */
    const dcc_rest_string_map_t *metadata;
    const dcc_lobby_member_spec_t *members;
    size_t member_count;
    uint32_t idle_timeout_seconds;
} dcc_lobby_params_t;

typedef struct dcc_lobby_channel_link_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* CHANNEL_ID */
    dcc_snowflake_t channel_id;
} dcc_lobby_channel_link_params_t;

typedef struct dcc_lobby_member_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* METADATA, FLAGS, ADDITIONAL_NAME */
    const dcc_rest_string_map_t *metadata;
    uint64_t flags;
    const char *additional_name;
} dcc_lobby_member_params_t;

typedef struct dcc_lobby_member_bulk_item {
    size_t size;
    uint32_t version;
    uint64_t present; /* METADATA, FLAGS, ADDITIONAL_NAME, REMOVE_MEMBER */
    dcc_snowflake_t user_id;
    const dcc_rest_string_map_t *metadata;
    uint64_t flags;
    const char *additional_name;
    uint8_t remove_member;
} dcc_lobby_member_bulk_item_t;

typedef struct dcc_lobby_member_bulk_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* must be zero in version 1 */
    const dcc_lobby_member_bulk_item_t *items;
    size_t item_count;
} dcc_lobby_member_bulk_params_t;

typedef struct dcc_lobby_message_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* METADATA, FLAGS */
    const char *content;
    const dcc_rest_string_map_t *metadata;
    uint64_t flags;
} dcc_lobby_message_params_t;

typedef struct dcc_lobby_message_moderation_metadata_item {
    const char *key;
    const char *value;
} dcc_lobby_message_moderation_metadata_item_t;

typedef struct dcc_lobby_message_moderation_metadata_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* must be zero in version 1 */
    const dcc_lobby_message_moderation_metadata_item_t *items;
    size_t item_count;
} dcc_lobby_message_moderation_metadata_params_t;

typedef struct dcc_rest_lobby_message_query {
    size_t size;
    uint32_t version;
    uint64_t present; /* LIMIT */
    uint16_t limit;
} dcc_rest_lobby_message_query_t;
```

The moderation metadata item is a fixed leaf, like the string-map entry; do
not add size/version fields to either fixed array element during ABI 2.

Secrets are non-empty and at most 250 Unicode scalar values. Idle timeouts are
5–604800. Create and modify accept at most 25 member specs; bulk update accepts
1–25 items. Versioned member arrays use first-element size as byte stride and
require identical size/version. Every user ID is nonzero. Create/modify member
specs intentionally have no `additional_name`; only Add Member and Bulk Update
can set it. A present additional name is either null to clear or 1–80 Unicode
scalar values. `remove_member` is exactly 0 or 1 when present.

Lobby/member/message metadata is nullable and each non-null map has a maximum
aggregate key-plus-value length of 1000 Unicode scalar values. Duplicate keys
are invalid. Lobby message content is required and non-empty. Moderation
metadata has 0–5 unique keys, key length 1–1024, and value length at most 2000.

`dcc_rest_link_lobby_channel` is the sole canonical link/unlink operation.
Present `CHANNEL_ID` emits `{"channel_id":"..."}` and requires a nonzero ID;
absent `CHANNEL_ID` emits an exact zero-length PATCH body, not `{}`. Delete
`dcc_rest_unlink_lobby_channel`. Lobby-message query contains only `limit`,
range 1–200 with Discord default 50; do not reuse `dcc_rest_id_page_t` or emit
before/after parameters.

### Role-connection and soundboard records

```c
typedef struct dcc_application_role_connection_metadata_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* NAME_LOCALIZATIONS, DESCRIPTION_LOCALIZATIONS */
    dcc_application_role_connection_metadata_type_t type;
    const char *key;
    const char *name;
    const dcc_localization_t *name_localizations;
    size_t name_localization_count;
    const char *description;
    const dcc_localization_t *description_localizations;
    size_t description_localization_count;
} dcc_application_role_connection_metadata_params_t;

typedef struct dcc_application_role_connection_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* PLATFORM_NAME, PLATFORM_USERNAME, METADATA */
    const char *platform_name;
    const char *platform_username;
    const dcc_rest_string_map_t *metadata;
} dcc_application_role_connection_params_t;

typedef struct dcc_soundboard_send_params {
    size_t size;
    uint32_t version;
    uint64_t present; /* SOURCE_GUILD_ID */
    dcc_snowflake_t sound_id;
    dcc_snowflake_t source_guild_id;
} dcc_soundboard_send_params_t;
```

`<dcc/rest/role_connections.h>` includes Task 9's neutral
`<dcc/application_types.h>` instead of owning a second locale-map type.
Application metadata replacement accepts 0–5 records. Each record type is
1–8; key is 1–50 ASCII `[a-z0-9_]`; name is 1–100 Unicode scalar values; and
description is 1–200. Localization maps are non-null when present, reject
duplicate or unsupported locale keys, and enforce the corresponding value
limit. A localization pointer is null exactly when its count is zero and span
arithmetic is checked before every read. The metadata array uses checked
historical stride.

Current-user role connection platform name is at most 50 and platform username
at most 100 Unicode scalar values. A present value is non-null. Metadata is a
non-null typed object when present; keys obey the application metadata key
grammar, values are at most 100 Unicode scalar values, and duplicates fail.

Soundboard `sound_id` is required and nonzero. A present `source_guild_id` is
nonzero. No Task 10 operation accepts an opaque JSON request body; Slack and
GitHub webhook compatibility payloads remain the only reviewed opaque endpoint
exceptions elsewhere in the manifest.

## Exact Task 10 REST removals

Delete these 28 legacy endpoint symbols from their public headers and owning
translation units; strict mode must reject any reintroduction:

```text
dcc_rest_add_group_dm_recipient_params
dcc_rest_add_lobby_member_params
dcc_rest_build_application_modify_body
dcc_rest_build_lobby_body
dcc_rest_build_lobby_channel_link_body
dcc_rest_build_lobby_create_or_join_body
dcc_rest_build_lobby_member_body
dcc_rest_build_lobby_member_bulk_body
dcc_rest_build_lobby_message_body
dcc_rest_build_lobby_message_moderation_metadata_body
dcc_rest_build_soundboard_send_body
dcc_rest_bulk_update_lobby_members_params
dcc_rest_create_lobby_params
dcc_rest_create_or_join_lobby_params
dcc_rest_create_test_entitlement_params
dcc_rest_get_entitlements_page
dcc_rest_link_lobby_channel_params
dcc_rest_modify_current_application_params
dcc_rest_modify_current_user_params
dcc_rest_modify_current_user_voice_state_params
dcc_rest_modify_lobby_params
dcc_rest_modify_user_voice_state_params
dcc_rest_send_lobby_message_params
dcc_rest_send_soundboard_sound_params
dcc_rest_unlink_lobby_channel
dcc_rest_update_application_role_connection_metadata_params
dcc_rest_update_current_user_application_role_connection_params
dcc_rest_update_lobby_message_moderation_metadata_params
```

Also remove transition helper `dcc_rest_official_body_json_free`, direct-message
composites `dcc_rest_create_direct_message` and
`dcc_rest_create_direct_message_builder`, and stale canonical
`dcc_rest_get_current_user_dms`. The Task 10 public REST deletion count is
therefore 32. The 28-list contains nine Task 10 body builders; the four body
builders removed in earlier tasks make all thirteen historical official body
builders absent by the end of Task 10.

The direct-message public header and implementation are composite-only. Delete:

- `include/dcc/rest/resources/direct_messages.h` and its include from
  `include/dcc/rest/resources.h`;
- `src/rest/rest_direct_messages.c`;
- `src/rest/rest_direct_messages_json.c`;
- `src/internal/rest/dcc_rest_direct_messages_internal.h`;
- their CMake and documentation entries.

Keep `src/rest/rest_capture.c` and its internal header: the asynchronous worker
still uses them. Do not replace the removed direct-message composite with dead
private machinery. There is no retained in-repository consumer after internal
App-mirror use is retired; callers compose active Create DM Channel and Create
Message requests explicitly.

## Freeze exactly 196 App REST mirrors for Task 14

Keep these four translation units and their CMake entries through Task 10:

| Unit | Exact external `dcc_app_*` definitions |
| --- | ---: |
| `src/app/app_rest_shortcuts.c` | 51 |
| `src/app/app_rest_shortcuts_guilds.c` | 72 |
| `src/app/app_rest_shortcuts_resources.c` | 45 |
| `src/app/app_rest_shortcuts_webhooks.c` | 28 |
| **Total** | **196** |

The sorted, newline-terminated 196-name baseline has SHA-256
`4b3246e7c89911bba7bf3256b1021294845aae343e8a6f32580fcd116e6a660f`.
Freeze that exact name set in the audit/self-test. Every name remains declared
exactly once in `include/dcc/app/legacy.h` and externally defined exactly once
by its frozen owner until Task 14 deletes all 196 in the remaining-
compatibility cutover.
Prefix-only matching is insufficient: the current removed-name wildcard set
misses 32 of these exact mirrors. Strict Task 10 audit must fail on one missing
name, one declaration without a definition, one definition without a
declaration, a duplicate, or one mirror moved to another owner. The records are
compatibility exports with `removal_task: 14`, never active REST endpoints or
generic operations.

Produce the frozen baseline with this exact extraction; the count and digest
are acceptance checks, not a generated allowlist at test time:

```sh
for f in \
    src/app/app_rest_shortcuts.c \
    src/app/app_rest_shortcuts_guilds.c \
    src/app/app_rest_shortcuts_resources.c \
    src/app/app_rest_shortcuts_webhooks.c
do
    rg -o '^dcc_status_t dcc_app_[A-Za-z0-9_]+' "$f" | awk '{print $2}'
done | LC_ALL=C sort -u > /tmp/task-10-app-mirror-names.txt
test "$(wc -l < /tmp/task-10-app-mirror-names.txt)" -eq 196
test "$(shasum -a 256 /tmp/task-10-app-mirror-names.txt | awk '{print $1}')" = \
    4b3246e7c89911bba7bf3256b1021294845aae343e8a6f32580fcd116e6a660f
```

`src/app/app_rest_shortcuts_resources.c` also owns nine non-mirror context
operations. Move their definitions, without changing behavior or public
ownership, into `src/app/app_context_shortcuts.c`, leaving the four frozen
mirror units with exactly the 196 compatibility definitions above:

```text
dcc_ctx_add_member_role
dcc_ctx_remove_member_role
dcc_ctx_create_thread_from_message
dcc_ctx_create_thread_from_message_name
dcc_ctx_archive_current_thread
dcc_ctx_lock_current_thread
dcc_ctx_unlock_current_thread
dcc_ctx_add_author_role
dcc_ctx_remove_author_role
```

These nine must still link. Do not count them in the 196 or delete their
declarations ahead of Task 11/14.

The ten exports in `src/app/app_messages.c` are also outside the 196:
`dcc_app_send`, `dcc_app_send_with_id`, `dcc_app_send_with_thread`,
`dcc_app_send_with_thread_name`, `dcc_app_send_text`,
`dcc_app_send_text_with_id`, `dcc_app_send_text_with_thread`,
`dcc_app_send_json`, `dcc_app_managed_message_publish_latest`, and
`dcc_app_managed_message_publish_latest_store`. Task 14 removes their legacy
surface. Task 10 may rewrite their internals, but must not delete the unit or
inflate the mirror count. Do not introduce new uses of the raw
`dcc_app_send_json` escape.

Update internal App call sites, package-consumer checks, focused docs, and new
transition tests to call canonical REST via `dcc_app_client(app)`. The frozen
wrappers remain functional compatibility shims and may delegate to canonical
REST, but no new in-tree consumer may call them. Do not add macros, inline
functions, or renamed App wrappers that reproduce the mirror. Sugar headers
are deleted in Task 14; do not add new mirror expansions to Task 10 tests.

`dcc_app_infer_guild_id_from_channel` remains only as one of the frozen
compatibility exports and disappears in Task 14. No retained internal consumer
may call it. Keep cache-only `dcc_client_infer_guild_id_from_channel`; do not
add another fallback network composite.

## Composite state machines

No composite waits on a request, retains caller stack storage, or launches all
legs concurrently. Once its first REST leg is accepted and the public function
returns `DCC_OK`, it owns its state until exactly one terminal composite
completion, including a later child-admission failure. A failure before the
first admission returns non-OK, frees all state, and invokes neither callback
nor observer. User code is never called while a REST, App, registry, store, or
state-machine lock is held.

### Managed-message publish

Implement this exact state progression in
`src/managed_message/managed_message.c`:

```text
LOAD -> [DELETE_PENDING] -> CREATE_PENDING -> SAVE -> DONE
```

`LOAD` runs synchronously before any network admission. `DCC_ERR_NOT_FOUND`
means no prior message; any other non-OK load result is an initial local
failure. If `keep_previous` is true or no old message exists, submit Create
Message directly. Otherwise submit canonical Delete Message first. Delete 404
is successful/idempotent and advances to create; any other non-2xx or transport
failure terminates.

The heap state owns the serialized/deep-copied message payload, normalized REST
options needed by each child, old/new refs, and current stage. Storage function
pointers, storage user data, final callback, and final user data are borrowed
until terminal return. Use canonical `dcc_rest_delete_message` and
`dcc_rest_create_message` with `dcc_rest_result_t`; do not format paths or call
`dcc_endpoint_submit_legacy_raw`.

Replace the legacy managed-message callback typedef in this task. The canonical
header publishes only this result-model shape:

```c
typedef enum dcc_managed_message_publish_stage {
    DCC_MANAGED_MESSAGE_STAGE_LOAD = 0,
    DCC_MANAGED_MESSAGE_STAGE_DELETE = 1,
    DCC_MANAGED_MESSAGE_STAGE_CREATE = 2,
    DCC_MANAGED_MESSAGE_STAGE_SAVE = 3,
    DCC_MANAGED_MESSAGE_STAGE_DONE = 4
} dcc_managed_message_publish_stage_t;

typedef struct dcc_managed_message_publish_result {
    size_t size;
    uint32_t version;
    dcc_status_t status;
    dcc_managed_message_publish_stage_t stage;
    const dcc_rest_result_t *rest_result;
    dcc_managed_message_ref_t new_ref;
    dcc_status_t storage_status;
    uint8_t has_new_ref;
} dcc_managed_message_publish_result_t;

typedef void (*dcc_managed_message_publish_fn)(
    dcc_client_t *client,
    const dcc_managed_message_publish_result_t *result,
    void *user_data);
```

`dcc_managed_message_publish_cb` and every `dcc_rest_response_t` bridge are
removed. The aggregate result is borrowed only through callback return.
`status` is the authoritative composite outcome; `stage` is DONE only on full
success and otherwise names the failing step. `rest_result` is the most recent
actual child result when one exists and is null for pre-request/local admission
failures; clone it to retain it. `storage_status` is non-OK only for load/save
failure, and `has_new_ref` gates `new_ref`. Thus ID parse failure reports
`DCC_ERR_JSON` at CREATE while retaining the successful create result; a create
admission failure after delete reports its local status at CREATE with null
`rest_result`; a save failure reports that status at SAVE with the created ref
and create result. The public publish function takes
`dcc_managed_message_publish_fn` directly and never fabricates a legacy
response view.

On successful create, parse the new message ID, set the new channel/message ref,
then call `save` synchronously if supplied. Preserve four separate final facts:
the child REST result, terminal stage, new ref, and `storage_status`. A save
failure does not rewrite a successful HTTP result into a fabricated REST
failure. First-leg or second-leg terminal failure, ID parse failure, a later
admission failure, cancellation, and shutdown each produce one final callback
and one observer path, with no double reporting. A terminal child REST failure
uses that child's existing observer delivery; a later child-admission failure
has no child result, so the already-accepted composite synthesizes exactly one
structured observer error itself.

`dcc_app_managed_message_publish_latest` remains a thin delegation.
`dcc_app_managed_message_publish_latest_store` additionally owns its temporary
store binding until the managed composite's one terminal callback.

### Command registry and App READY sync

Consume Task 9's sequential command-registry apply state machine. It submits
one diff at a time, stops at the first failure, and emits one aggregate
completion. Dry-run emits one aggregate terminal completion and performs zero
HTTP requests.

Fix the App lifetime bug in `src/app/app_commands.c`. The current fetch callback
builds stack `snapshot`/`plan` values around an asynchronous apply admission.
Replace that with an App-owned heap bridge:

```text
FETCH_PENDING -> PARSE -> PLAN -> [APPLY_PENDING] -> DONE
```

The bridge owns normalized command-sync options plus the source remote snapshot
and source plan only through apply admission. Task 9 apply synchronously validates
and deep-copies its complete executable plan before returning `DCC_OK`; therefore
the bridge must deinitialize both source snapshot and source plan immediately
after that successful return. Its terminal heap state retains only the client/App
lifetime guard, opaque apply operation, aggregate callback bridge, and terminal
bookkeeping. A local apply admission failure deinitializes the source values and
is terminal. Preserve READY application-ID inference, once-only behavior, and
structured logging.

`dcc_app_apply_command_plan` and `dcc_app_sync_commands_from_json` must not make
the caller retain a plan or remote JSON past return without an explicit public
ownership contract. Normalize/deep-copy the private apply plan before return;
an optional `out_plan` remains independently caller-owned. App destruction
already closes admission and drains accepted REST callbacks before freeing the
App; keep the command-sync state registered with that lifetime barrier.

### Send message then create thread

Rewrite the send-with-thread path in `src/app/app_messages.c` as:

```text
CREATE_MESSAGE_PENDING -> PARSE_MESSAGE_ID -> CREATE_THREAD_PENDING
    -> PARSE_THREAD_ID -> DONE
```

Submit canonical `dcc_rest_create_message` directly through
`dcc_app_client(app)`. After a successful result, parse and retain the message
ID, then submit Task 7 canonical `dcc_rest_create_thread_from_message`; never
call the frozen `dcc_app_create_thread_from_message` compatibility mirror. The
heap state
owns the copied thread name, normalized thread params/options, applied-tag
array, IDs, and callback bridge. The first endpoint serializes the message
before returning, so caller message storage is not retained.

A first REST failure or message-ID parse failure completes once with zero IDs.
A second-leg admission failure completes once with the known message ID and
zero thread ID. A second REST failure or thread-ID parse failure also completes
once. Cancellation/shutdown follow the same path. Initial validation,
serialization, allocation, or first-admission failure returns non-OK with no
callback. Bridge canonical result data to the legacy App callback only for the
duration of that callback; the state must not retain either borrowed child
result.

## Strict manifest and audit

Update `tools/rest_v2_endpoints.json` and
`tools/audit_rest_v2_endpoints.py` together:

- active counts are exactly 224 total and 41/35/47/57/44 by task;
- Task 10 contains the 44 rows and domain counts in this brief;
- the active manifest contains no Get Current User DMs record or orphan route;
- each active endpoint has exact `auth_policy` and boolean
  `audit_log_reason`; Task 10 values are fixed above;
- each canonical endpoint has exactly one public declaration and one external
  definition in its recorded owner, with exact parameters/method/route/input;
- all 28 Task 10 legacy names, the helper, both DM composites, and the stale
  canonical name are absent;
- all 196 exact App mirror names remain declared and externally defined once
  by their frozen owners, carry `removal_task: 14`, and have no internal caller,
  while the nine relocated context operations remain;
- exactly 60 immutable generic operations remain; helper/composite provenance
  can stay represented in a removal ledger, but never as an allowed public
  strict symbol;
- opaque endpoint payloads remain limited to the reviewed Slack and GitHub
  webhook compatibility records.

Delete the `--progress-through` argparse option, every progress/debt branch,
and synthetic progress fixture. Register
`dcc_rest_v2_endpoint_manifest_audit` in CMake without the flag. Invoking the
script with `--progress-through 6` or `--progress-through 10` must fail through
argparse as an unrecognized option; there is no hidden compatibility mode.

Add self-tests that restore one removed REST legacy, helper, composite, and
stale canonical symbol, delete one frozen App mirror, add a 197th mirror, and
move one mirror to the wrong owner, then prove rejection. Also prove rejection for missing or
duplicate canonical declaration/definition, wrong header/source owner, wrong
input/signature, method/route mismatch, invalid auth-policy value, missing
audit-reason capability, wrong final counts, and a third opaque-payload
exception.

## Required RED matrix

Add focused Task 10 cases to `dcc_rest_v2_endpoint_smoke` (split support files
by REST records/auth and composite behavior rather than growing one monolith).
Capture RED before production edits for at least the stale DMs entry, one raw
Task 10 overload, unconditional Bot header behavior, immediate App plan
deinitialization, legacy managed-message raw submission, one internal call to a
frozen App mirror, and the missing exact mirror compatibility ledger.

The green matrix must cover:

1. **All 44 operations:** compile the exact canonical signature and compare
   literal method/path/query/body/header capture against the table. A delayed
   server proves submission returns before response.
2. **Borrowed input:** mutate/free every caller string, map, array, and record
   immediately after successful return; captured wire bytes remain unchanged.
3. **Prefix and presence:** current initializer, larger covered record,
   supported historical prefix, zero/short size, bad version, unknown bit,
   present-uncovered field, nullable/empty/false/zero distinctions, mismatched
   versioned-array stride, overflow, and poisoned suffix/canaries.
4. **Preflight atomicity:** zero path IDs, empty required strings, invalid
   pointer/count pairs, every documented limit boundary, duplicate map keys,
   and allocation failpoints produce no network, callback, observer, or output
   handle and leave `*out_request == NULL`.
5. **Queries:** exact order, percent escaping, comma SKU IDs, limit bounds,
   explicit false booleans, no invented before/after exclusion,
   auth-dependent subscription `user_id`, and lobby limit-only behavior.
6. **Authentication:** capture NONE, Bot, and Bearer headers; reject every
   unsupported mode locally; prove DEFAULT resolution, uncovered/partial
   suffix handling, caller Bearer-token mutation after return, token wipe/no
   logging, no credential on absolute URLs, and no duplicate Authorization.
   Test decoded audit-reason scalar limits and exact one-time encoding even
   though every Task 10 typed endpoint rejects a reason.
7. **Managed message:** no prior ref, keep prior, delete 2xx, delete 404,
   delete failure, create failure, malformed create body, load NOT_FOUND/load
   error, save failure, later admission failure, cancel/shutdown, exactly-one
   callback/observer, and REST concurrency 1 without deadlock.
8. **Command sync:** sequential request count/order, dry-run zero requests and
   one terminal, stop on first failure, later admission failure, plan/snapshot
   lifetime, READY inference/once, App destruction, and callback reentry.
9. **Send-with-thread:** first/second HTTP failure, both ID parse failures,
   second admission failure, caller input mutation, cancel/shutdown, exact IDs
   at each failure stage, and exactly-one callback under ASan.
10. **App compatibility freeze:** all exact 196 declarations/definitions and
    four CMake units remain with unchanged signatures and behavior, the hash
    and per-owner counts match, no internal consumer invokes them, nine context
    functions link from their relocated owner, and the ten `app_messages.c`
    exports remain owned by Task 14.

## Verification and handoff

Run at minimum:

- strict `python3 tools/audit_rest_v2_endpoints.py` and its schema/self-tests;
- an expected-failure invocation with `--progress-through 10`;
- `dcc_rest_v2_endpoint_smoke`, request/result, HTTP, official-surface,
  App/listener/structured-error, managed-message, command-registry,
  interaction/server/flow, store, Sugar transition, and package-consumer
  focused targets;
- standalone C11 and C++17 compilation for every modified public header;
- ASan/UBSan runs for all owned state and historical-prefix fixtures;
- repeated TSan runs for command/App destruction, cancellation, and composite
  callback reentry;
- MinGW x86_64 and i686 header/layout/source probes;
- the full enabled CTest suite, public/transition/source-layout/package/install
  audits, and `git diff --check`.

Document exact RED evidence, final manifest arithmetic, auth-policy counts,
record minimum prefixes, deleted REST symbol/file counts, the preserved App
mirror set, owner counts, and hash, state-machine failure coverage,
sanitizer/platform evidence, known pre-existing failures, and implementation
hash in `task-10-implementer-report.md`. Commit implementation/tests with
`feat: complete request-based REST API`, commit the report separately, do not
push, and stop for independent review.

Official contract sources: [User](https://docs.discord.com/developers/resources/user),
[Channel](https://docs.discord.com/developers/resources/channel),
[Voice](https://docs.discord.com/developers/resources/voice),
[Entitlement](https://docs.discord.com/developers/resources/entitlement),
[Subscription](https://docs.discord.com/developers/resources/subscription),
[Application Role Connection Metadata](https://docs.discord.com/developers/resources/application-role-connection-metadata),
[Lobby](https://docs.discord.com/developers/resources/lobby),
[Gateway](https://docs.discord.com/developers/events/gateway),
[Application](https://docs.discord.com/developers/resources/application), and
[Soundboard](https://docs.discord.com/developers/resources/soundboard).
