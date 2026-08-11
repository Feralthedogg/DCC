# Task 8 brief — current guild, member, role, ban, and prune REST surface

Implement only Task 8 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`, as refined by
this brief. The REST section of the design, the completed Task 6 manifest,
Task 7's authentication/audit metadata contract, and this brief are
authoritative. Where the old 51-entry Task 8 inventory conflicts with the
current Discord documentation, this brief supersedes that inventory. Work
test-first, do not push, and do not edit the ignored progress ledger. Do not
start Task 9 domains. Keep intermediate commits buildable.

Official contract sources are Discord's current
[Guild](https://docs.discord.com/developers/resources/guild),
[User](https://docs.discord.com/developers/resources/user),
[Soundboard](https://docs.discord.com/developers/resources/soundboard),
[Audit Log](https://docs.discord.com/developers/resources/audit-log), and
[Permissions](https://docs.discord.com/developers/topics/permissions)
documentation, together with the
[Discord change log](https://docs.discord.com/developers/change-log). The
[Discord API documentation repository](https://github.com/discord/discord-api-docs)
is the authority for the third-party surface; its README explicitly excludes
features that are not generally available or fully supported for third-party
use.

## Outcome and authoritative accounting

Migrate exactly 47 active operations: 28 guild operations, 12 guild-member
operations, and seven role operations. Every retained endpoint has one typed,
request-handle-based public submission function ending in call options and an
optional request output. Remove four stale canonical operations and all 32
Task 8 legacy public symbols. Task 8 contains zero multipart-capable endpoints;
soundboard uploads are JSON data URIs, not multipart forms.

The globally reconciled active REST manifest contains 224 endpoints. Its task
counts are:

| Migration task | Active endpoints |
| --- | ---: |
| Task 6 | 41 |
| Task 7 | 35 |
| Task 8 | 47 |
| Task 9 | 57 |
| Task 10 | 44 |
| **Total** | **224** |

Task 7 hands off 76/224 strict active endpoints. On a successful
`--progress-through 8` run, Tasks 6 through 8 account for 123 strict active
endpoints and Tasks 9 through 10 account for 101 transition endpoints. That is
123/224, or 54.9 percent, of the active endpoint migration. The audit must not
continue to report the historical 231 total or the old 41/35/51/59/45 task
split.

## Remove four stale canonical operations

Delete these public declarations, external definitions, manifest entries,
route-catalog identities when no longer used, tests, documentation, App/Sugar
mirrors, and package-consumer references:

| Removed canonical symbol | Historical route | Reason |
| --- | --- | --- |
| `dcc_rest_create_guild` | `POST /guilds` | Discord made this endpoint unavailable to apps after 2025-07-15; it is absent from the current third-party Guild resource. |
| `dcc_rest_delete_guild` | `DELETE /guilds/{guild.id}` | It is absent from the current third-party Guild resource. |
| `dcc_rest_modify_guild_integration` | `PATCH /guilds/{guild.id}/integrations/{integration.id}` | Current integrations expose only list and delete operations. |
| `dcc_rest_sync_guild_integration` | historical sync operation | It is absent from the current third-party surface; the DCC route also lacks the historical `/sync` suffix. |

Do not retain compatibility wrappers for these four operations and do not
classify them as deprecated active endpoints. `dcc_rest_create_guild_params`
is separately included in the 32-symbol legacy removal list below. Add a
manifest/docs audit regression which fails if any of the four canonical names
or method/template pairs reappears.

## Task 7 authentication and audit dependency

Do not begin Task 8 implementation on a Task 7 base that has not completed the
common authentication and audit-header pipeline. Task 8 consumes, and does not
redesign, this exact public suffix on `dcc_rest_call_options_t`:

```c
const char *audit_log_reason;
dcc_rest_auth_mode_t auth_mode;
const char *auth_token;
```

`dcc_rest_auth_mode_t` has exactly `DCC_REST_AUTH_DEFAULT`,
`DCC_REST_AUTH_NONE`, `DCC_REST_AUTH_BOT`, and `DCC_REST_AUTH_BEARER`.
There is no presence mask in call options: a null `audit_log_reason` is absent,
and `DCC_REST_AUTH_DEFAULT` selects the typed endpoint's private default.
The mandatory historical call-options prefix ends after `user_data`. Read each
tail field only when fully covered by `size`. `DCC_REST_AUTH_BEARER` requires
both the complete `auth_token` field and a non-null, non-empty token. Typed
endpoints apply a private capability policy and reject unsupported modes before
body/query allocation or queue admission.

Task 7 must already copy normalized auth/audit metadata through raw submission,
the asynchronous request, worker, request preparation, header construction,
interceptors, cancellation, and cleanup. It must accept the historical
call-options prefix, default every uncovered suffix field, and avoid shallow
copying an uncovered tail. The enclosing `dcc_rest_request_desc_t` validator
must require a nested field to be covered by both the descriptor and nested
options sizes.

For Task 8, bot-authenticated operations accept `DEFAULT` and `BOT` and reject
`NONE` and `BEARER` unless explicitly listed below. Add Guild Member always
uses Bot authorization; its user OAuth token remains a required JSON field.
Get Current User Guilds accepts `DEFAULT`, `BOT`, and `BEARER`. Get Current
User Guild Member requires `BEARER` and the documented
`guilds.members.read` scope; reject `DEFAULT`, `NONE`, and `BOT` locally. The
public widget document and image operations default to `NONE`, accept only
`DEFAULT` or `NONE`, and emit no Authorization header. Every other Task 8
operation defaults to Bot authorization.

Never forward Bot/Bearer authorization or an audit reason to an absolute
external URL. `NONE` emits no Authorization header even when the client has a
bot token. A typed endpoint rejects a non-null audit reason unless its row in
the inventory below is marked `yes`.

Audit reasons are validated as the decoded value, not the encoded transport
string. Require valid UTF-8 containing 1 through 512 Unicode scalar values,
then RFC 3986-percent-encode the UTF-8 bytes for `X-Audit-Log-Reason`. Do not
apply the 512 limit to the encoded ASCII length: 512 Korean or emoji scalar
values remain valid even though their encoded representation exceeds 512
bytes. Use checked arithmetic for the worst-case encoded allocation and copy
the reason before submission returns.

## Canonical endpoint signature

Every retained operation uses this shape, with the table's scalar path
parameters and optional typed input inserted between `client` and `options`:

```c
dcc_status_t dcc_rest_operation(
    dcc_client_t *client,
    /* nonzero scalar path parameters, then optional typed input */
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);
```

Null options select `DCC_REST_CALL_OPTIONS_INIT`; an endpoint which requires
explicit Bearer authentication may still reject that default. Null
`out_request` means auto-release. On local rejection, clear a supplied output,
queue nothing, and run neither callback nor error observer. On `DCC_OK`, the
Task 4 terminal-result contract applies exactly once.

Simple required snowflakes stay scalar path arguments in route order. Do not
duplicate guild, user, role, integration, or sound IDs inside a typed input.
The only adjacent typed-array count is Role Positions. Canonical declarations
contain no `dcc_rest_cb`, `user_data`, raw JSON/query string, `_params`,
`_options`, `_page`, `_seconds`, `_timeout`, `_move`, `_clear`, builder, sync,
future, async, or `_v2` overload.

## Exact 47-operation inventory

`Input` is the exact public input type; `—` means scalar-only. `Bot` means
`DEFAULT` resolves to Bot auth. `User` is the current-user exception described
above. `Public` means `DEFAULT` resolves to no authentication. Every row has
`multipart: false` in the manifest.

### Guild operations — 28

| Canonical symbol | Method and literal route | Input | Auth | Audit reason |
| --- | --- | --- | --- | --- |
| `dcc_rest_begin_guild_prune` | `POST /guilds/{guild_id}/prune` | `const dcc_rest_guild_prune_t *body` | Bot | yes |
| `dcc_rest_create_guild_ban` | `PUT /guilds/{guild_id}/bans/{user_id}` | `const dcc_rest_guild_ban_create_t *body` | Bot | yes |
| `dcc_rest_create_guild_soundboard_sound` | `POST /guilds/{guild_id}/soundboard-sounds` | `const dcc_rest_guild_soundboard_sound_create_t *body` | Bot | yes |
| `dcc_rest_delete_guild_ban` | `DELETE /guilds/{guild_id}/bans/{user_id}` | — | Bot | yes |
| `dcc_rest_delete_guild_integration` | `DELETE /guilds/{guild_id}/integrations/{integration_id}` | — | Bot | yes |
| `dcc_rest_delete_guild_soundboard_sound` | `DELETE /guilds/{guild_id}/soundboard-sounds/{sound_id}` | — | Bot | yes |
| `dcc_rest_get_current_user_guilds` | `GET /users/@me/guilds` | `const dcc_rest_current_user_guilds_query_t *query` | User: Bot or Bearer | no |
| `dcc_rest_get_guild` | `GET /guilds/{guild_id}` | `const dcc_rest_guild_query_t *query` | Bot | no |
| `dcc_rest_get_guild_audit_log` | `GET /guilds/{guild_id}/audit-logs` | `const dcc_rest_guild_audit_log_query_t *query` | Bot | no |
| `dcc_rest_get_guild_ban` | `GET /guilds/{guild_id}/bans/{user_id}` | — | Bot | no |
| `dcc_rest_get_guild_bans` | `GET /guilds/{guild_id}/bans` | `const dcc_rest_guild_bans_query_t *query` | Bot | no |
| `dcc_rest_get_guild_integrations` | `GET /guilds/{guild_id}/integrations` | — | Bot | no |
| `dcc_rest_get_guild_preview` | `GET /guilds/{guild_id}/preview` | — | Bot | no |
| `dcc_rest_get_guild_prune_count` | `GET /guilds/{guild_id}/prune` | `const dcc_rest_guild_prune_query_t *query` | Bot | no |
| `dcc_rest_get_guild_soundboard_sound` | `GET /guilds/{guild_id}/soundboard-sounds/{sound_id}` | — | Bot | no |
| `dcc_rest_get_guild_soundboard_sounds` | `GET /guilds/{guild_id}/soundboard-sounds` | — | Bot | no |
| `dcc_rest_get_guild_vanity_url` | `GET /guilds/{guild_id}/vanity-url` | — | Bot | no |
| `dcc_rest_get_guild_voice_regions` | `GET /guilds/{guild_id}/regions` | — | Bot | no |
| `dcc_rest_get_guild_welcome_screen` | `GET /guilds/{guild_id}/welcome-screen` | — | Bot | no |
| `dcc_rest_get_guild_widget` | `GET /guilds/{guild_id}/widget` | — | Bot | no |
| `dcc_rest_get_guild_widget_json` | `GET /guilds/{guild_id}/widget.json` | — | Public | no |
| `dcc_rest_get_guild_widget_png` | `GET /guilds/{guild_id}/widget.png` | `const dcc_rest_guild_widget_image_query_t *query` | Public | no |
| `dcc_rest_leave_guild` | `DELETE /users/@me/guilds/{guild_id}` | — | Bot | no |
| `dcc_rest_modify_guild` | `PATCH /guilds/{guild_id}` | `const dcc_rest_guild_update_t *body` | Bot | yes |
| `dcc_rest_modify_guild_incident_actions` | `PUT /guilds/{guild_id}/incident-actions` | `const dcc_rest_guild_incident_actions_t *body` | Bot | no |
| `dcc_rest_modify_guild_soundboard_sound` | `PATCH /guilds/{guild_id}/soundboard-sounds/{sound_id}` | `const dcc_rest_guild_soundboard_sound_update_t *body` | Bot | yes |
| `dcc_rest_modify_guild_welcome_screen` | `PATCH /guilds/{guild_id}/welcome-screen` | `const dcc_rest_guild_welcome_screen_update_t *body` | Bot | yes |
| `dcc_rest_modify_guild_widget` | `PATCH /guilds/{guild_id}/widget` | `const dcc_rest_guild_widget_update_t *body` | Bot | yes |

### Guild-member operations — 12

| Canonical symbol | Method and literal route | Input | Auth | Audit reason |
| --- | --- | --- | --- | --- |
| `dcc_rest_add_guild_member` | `PUT /guilds/{guild_id}/members/{user_id}` | `const dcc_rest_guild_member_add_t *body` | Bot | no |
| `dcc_rest_add_guild_member_role` | `PUT /guilds/{guild_id}/members/{user_id}/roles/{role_id}` | — | Bot | yes |
| `dcc_rest_bulk_ban_guild_members` | `POST /guilds/{guild_id}/bulk-ban` | `const dcc_rest_guild_bulk_ban_t *body` | Bot | yes |
| `dcc_rest_get_current_user_guild_member` | `GET /users/@me/guilds/{guild_id}/member` | — | Bearer only | no |
| `dcc_rest_get_guild_member` | `GET /guilds/{guild_id}/members/{user_id}` | — | Bot | no |
| `dcc_rest_list_guild_members` | `GET /guilds/{guild_id}/members` | `const dcc_rest_guild_members_query_t *query` | Bot | no |
| `dcc_rest_modify_current_guild_member` | `PATCH /guilds/{guild_id}/members/@me` | `const dcc_rest_current_guild_member_update_t *body` | Bot | yes |
| `dcc_rest_modify_guild_member` | `PATCH /guilds/{guild_id}/members/{user_id}` | `const dcc_rest_guild_member_update_t *body` | Bot | yes |
| `dcc_rest_remove_guild_member` | `DELETE /guilds/{guild_id}/members/{user_id}` | — | Bot | yes |
| `dcc_rest_remove_guild_member_role` | `DELETE /guilds/{guild_id}/members/{user_id}/roles/{role_id}` | — | Bot | yes |
| `dcc_rest_search_guild_members` | `GET /guilds/{guild_id}/members/search` | `const dcc_rest_guild_member_search_query_t *query` | Bot | no |
| `dcc_rest_set_current_guild_member_nickname` | `PATCH /guilds/{guild_id}/members/@me/nick` | `const dcc_rest_current_guild_member_nickname_t *body` | Bot | yes |

The nickname operation remains active because Discord still documents it, but
mark it deprecated in the manifest and public documentation in favor of
`dcc_rest_modify_current_guild_member`.

### Role operations — seven

| Canonical symbol | Method and literal route | Input | Auth | Audit reason |
| --- | --- | --- | --- | --- |
| `dcc_rest_create_guild_role` | `POST /guilds/{guild_id}/roles` | `const dcc_rest_guild_role_create_t *body` | Bot | yes |
| `dcc_rest_delete_guild_role` | `DELETE /guilds/{guild_id}/roles/{role_id}` | — | Bot | yes |
| `dcc_rest_get_guild_role` | `GET /guilds/{guild_id}/roles/{role_id}` | — | Bot | no |
| `dcc_rest_get_guild_role_member_counts` | `GET /guilds/{guild_id}/roles/member-counts` | — | Bot | no |
| `dcc_rest_get_guild_roles` | `GET /guilds/{guild_id}/roles` | — | Bot | no |
| `dcc_rest_modify_guild_role` | `PATCH /guilds/{guild_id}/roles/{role_id}` | `const dcc_rest_guild_role_update_t *body` | Bot | yes |
| `dcc_rest_modify_guild_role_positions` | `PATCH /guilds/{guild_id}/roles` | `const dcc_rest_guild_role_position_t *positions, size_t position_count` | Bot | yes |

The exact audit-enabled set is therefore 21 operations: add member role, begin
prune, bulk ban, create ban, create role, create soundboard sound, delete ban,
delete integration, delete role, delete soundboard sound, modify current
member, modify guild, modify member, modify role, modify role positions, modify
soundboard sound, modify welcome screen, modify widget, remove member, remove
member role, and set current member nickname. Add Member and Modify Incident
Actions do not support the audit header. Do not duplicate an audit-reason field
inside any typed query or body.

## Versioned input ABI and exact presence/null rules

Define the new public records in focused guild, member, and role type headers.
Every top-level query/body and every evolvable nested record starts in this
order:

```c
size_t size;
uint32_t version;
uint64_t present;
```

Records with any nullable field immediately add:

```c
uint64_t nulls;
```

Each type has its own `*_VERSION` equal to `1U`, a complete `*_INIT` macro, and
a null-safe allocation-free `static inline` initializer. Initializers for
records with mandatory data accept that data; other initializers publish the
current size/version and no present/null bits. These conveniences are header
support, not exported REST operations or manifest entries.

Presence bits are assigned in the field order listed below, beginning with bit
zero, excluding mandatory unconditionally-read fields. Follow the established
public spelling `DCC_REST_<stem>_PRESENT_<field>` and use these exact stems:

| Type | Constant stem |
| --- | --- |
| `dcc_rest_guild_query_t` | `GUILD_QUERY` |
| `dcc_rest_current_user_guilds_query_t` | `CURRENT_USER_GUILDS_QUERY` |
| `dcc_rest_guild_audit_log_query_t` | `GUILD_AUDIT_LOG_QUERY` |
| `dcc_rest_guild_bans_query_t` | `GUILD_BANS_QUERY` |
| `dcc_rest_guild_members_query_t` | `GUILD_MEMBERS_QUERY` |
| `dcc_rest_guild_member_search_query_t` | `GUILD_MEMBER_SEARCH_QUERY` |
| `dcc_rest_guild_prune_query_t` | `GUILD_PRUNE_QUERY` |
| `dcc_rest_guild_widget_image_query_t` | `GUILD_WIDGET_IMAGE_QUERY` |
| `dcc_rest_guild_member_add_t` | `GUILD_MEMBER_ADD` |
| `dcc_rest_guild_member_update_t` | `GUILD_MEMBER_UPDATE` |
| `dcc_rest_current_guild_member_update_t` | `CURRENT_GUILD_MEMBER_UPDATE` |
| `dcc_rest_current_guild_member_nickname_t` | `CURRENT_GUILD_MEMBER_NICKNAME` |
| `dcc_rest_guild_ban_create_t` | `GUILD_BAN_CREATE` |
| `dcc_rest_guild_bulk_ban_t` | `GUILD_BULK_BAN` |
| `dcc_rest_guild_prune_t` | `GUILD_PRUNE` |
| `dcc_rest_role_colors_t` | `ROLE_COLORS` |
| `dcc_rest_guild_role_create_t` | `GUILD_ROLE_CREATE` |
| `dcc_rest_guild_role_update_t` | `GUILD_ROLE_UPDATE` |
| `dcc_rest_guild_role_position_t` | `GUILD_ROLE_POSITION` |
| `dcc_rest_guild_update_t` | `GUILD_UPDATE` |
| `dcc_rest_guild_incident_actions_t` | `GUILD_INCIDENT_ACTIONS` |
| `dcc_rest_guild_widget_update_t` | `GUILD_WIDGET_UPDATE` |
| `dcc_rest_welcome_channel_t` | `WELCOME_CHANNEL` |
| `dcc_rest_guild_welcome_screen_update_t` | `GUILD_WELCOME_SCREEN_UPDATE` |
| `dcc_rest_guild_soundboard_sound_create_t` | `GUILD_SOUNDBOARD_SOUND_CREATE` |
| `dcc_rest_guild_soundboard_sound_update_t` | `GUILD_SOUNDBOARD_SOUND_UPDATE` |

For example, the first Guild Update bit is
`DCC_REST_GUILD_UPDATE_PRESENT_NAME`; each subsequent field in that record is
the next bit. Version and initializer names use the same stem as
`DCC_REST_GUILD_UPDATE_VERSION` and `DCC_REST_GUILD_UPDATE_INIT`. Maintain a
private known-present mask per type for validation. A nullable field uses the
same public presence bit in `present` and `nulls`; do not create a second
null-bit namespace.

The semantics are exact:

- bit absent: omit the field and do not read its storage;
- bit present and null bit clear: validate and serialize the stored value,
  including explicit zero, false, empty string where the Discord field permits
  it, or an empty array;
- bit present and null bit set: emit JSON `null` and do not read the field's
  value storage;
- reject `nulls` bits outside the known nullable mask, any null bit without its
  present bit, an unknown present bit, or a set bit whose complete field/group
  is not covered by `size`.

Read `size` first with `memcpy`; read version/presence/null masks and fields only
after subtraction-based coverage checks. Accept version 1 larger layouts and
covered historical prefixes. Reject unsupported versions, undersized mandatory
prefixes, uncovered present/null fields, misaligned versioned arrays, and
checked address/span overflow. A pointer/count field is one logical presence
bit and both members must be covered before either is read. A present array may
use a null pointer only when its count is zero and null is not selected; that
encodes `[]`. A positive count requires a non-null pointer.

### Exact query records

The following lists fields after the common prefix. Fields in `bits` use bit
positions from left to right. None is nullable.

| Type | C fields | Presence bits and required fields | Null input |
| --- | --- | --- | --- |
| `dcc_rest_guild_query_t` | `uint8_t with_counts` | bits: `WITH_COUNTS` | no query |
| `dcc_rest_current_user_guilds_query_t` | `dcc_snowflake_t before`, `dcc_snowflake_t after`, `uint16_t limit`, `uint8_t with_counts` | bits: `BEFORE`, `AFTER`, `LIMIT`, `WITH_COUNTS` | no query; Discord defaults limit 200 and counts false |
| `dcc_rest_guild_audit_log_query_t` | `dcc_snowflake_t user_id`, `uint32_t action_type`, `dcc_snowflake_t before`, `dcc_snowflake_t after`, `uint16_t limit` | bits: `USER_ID`, `ACTION_TYPE`, `BEFORE`, `AFTER`, `LIMIT` | no query; Discord defaults limit 50 |
| `dcc_rest_guild_bans_query_t` | `dcc_snowflake_t before`, `dcc_snowflake_t after`, `uint16_t limit` | bits: `BEFORE`, `AFTER`, `LIMIT` | no query; Discord defaults limit 1000 |
| `dcc_rest_guild_members_query_t` | `dcc_snowflake_t after`, `uint16_t limit` | bits: `AFTER`, `LIMIT` | no query; Discord defaults after 0 and limit 1 |
| `dcc_rest_guild_member_search_query_t` | `const char *query`, `uint16_t limit` | mandatory covered, non-null `query`; bit: `LIMIT` | invalid |
| `dcc_rest_guild_prune_query_t` | `uint32_t days`, `const dcc_snowflake_t *include_roles`, `size_t include_role_count` | bits: `DAYS`, `INCLUDE_ROLES` | no query; Discord defaults days 7 |
| `dcc_rest_guild_widget_image_query_t` | `dcc_rest_guild_widget_style_t style` | bit: `STYLE` | no query; Discord defaults shield |

`dcc_rest_guild_widget_style_t` has exactly
`DCC_REST_GUILD_WIDGET_STYLE_SHIELD = 0`,
`DCC_REST_GUILD_WIDGET_STYLE_BANNER1 = 1`,
`DCC_REST_GUILD_WIDGET_STYLE_BANNER2 = 2`,
`DCC_REST_GUILD_WIDGET_STYLE_BANNER3 = 3`, and
`DCC_REST_GUILD_WIDGET_STYLE_BANNER4 = 4`. Serialize their lowercase strings.
Query output order is
fixed: current guilds `before`, `after`, `limit`, `with_counts`; audit log
`user_id`, `action_type`, `before`, `after`, `limit`; bans `limit`, `before`,
`after`; members `limit`, `after`; search `query`, `limit`; prune `days`,
`include_roles`; widget image `style`. Percent-encode string values, encode
prune roles as a comma-delimited snowflake list, and never emit a dangling
question mark.

Validate current-user-guild limit 1..200, audit limit 1..100, ban limit
1..1000, member/search limit 1..1000, and prune days 1..30. A present search
query must be valid UTF-8 and non-empty. A present prune role list must contain
at least one nonzero ID. Current-user-guild and ban cursors are nonzero when
present. Audit `after=0` is explicitly valid and selects the oldest entries;
member `after=0` is valid and must serialize when present. If both ban cursors
are provided, serialize both in the stated order and preserve Discord's
documented rule that `before` wins. Do not reuse `dcc_rest_id_page_t`: its
100-item ceiling and zero-cursor rule cannot represent these contracts.

### Exact member, ban, and prune body records

The following lists fields after the common prefix and, when applicable,
`nulls`:

| Type | Mandatory fields | Presence bits in bit order | Nullable bits | Null input |
| --- | --- | --- | --- | --- |
| `dcc_rest_guild_member_add_t` | `const char *access_token` | `NICK` (`const char *nick`), `ROLES` (`const dcc_snowflake_t *roles`, `size_t role_count`), `MUTE` (`uint8_t mute`), `DEAF` (`uint8_t deaf`) | none | invalid |
| `dcc_rest_guild_member_update_t` | none | `NICK` (`const char *nick`), `ROLES` (`const dcc_snowflake_t *roles`, `size_t role_count`), `MUTE` (`uint8_t mute`), `DEAF` (`uint8_t deaf`), `CHANNEL_ID` (`dcc_snowflake_t channel_id`), `COMMUNICATION_DISABLED_UNTIL` (`const char *communication_disabled_until`), `FLAGS` (`uint64_t flags`) | all seven | `{}` |
| `dcc_rest_current_guild_member_update_t` | none | `NICK`, `BANNER`, `AVATAR`, `BIO`, each stored as `const char *` | all four | `{}` |
| `dcc_rest_current_guild_member_nickname_t` | none | `NICK` (`const char *nick`) | `NICK` | `{}` |
| `dcc_rest_guild_ban_create_t` | none | `DELETE_MESSAGE_SECONDS` (`uint32_t delete_message_seconds`) | none | `{}` |
| `dcc_rest_guild_bulk_ban_t` | `const dcc_snowflake_t *user_ids`, `size_t user_id_count` | `DELETE_MESSAGE_SECONDS` (`uint32_t delete_message_seconds`) | none | invalid |
| `dcc_rest_guild_prune_t` | none | `DAYS` (`uint32_t days`), `COMPUTE_PRUNE_COUNT` (`uint8_t compute_prune_count`), `INCLUDE_ROLES` (`const dcc_snowflake_t *include_roles`, `size_t include_role_count`) | none | `{}` |

Add Member requires a non-empty user OAuth token with `guilds.join`; its HTTP
Authorization remains Bot. Non-null nicknames contain 1..32 Unicode scalar
values after valid UTF-8 validation. Every role/user ID in an array is nonzero.
Modify Member deliberately replaces the old combined voice flag and the
timeout/move/clear overloads: mute and deaf have distinct presence bits,
`channel_id: null` disconnects, and `communication_disabled_until: null`
clears a timeout. Validate a non-null timestamp as ISO 8601; absent a shared
clock parser, leave the dynamic 28-day future horizon to Discord and document
that server-authoritative boundary.

Ban and bulk-ban seconds accept exactly 0..604800 when present; 604801 is a
local error. Never clamp. Do not expose deprecated `delete_message_days`.
Bulk ban requires 1..200 nonzero IDs. Prune days accept 1..30 and default to 7;
`compute_prune_count` accepts only 0 or 1 and defaults to true when absent.
Do not expose the deprecated prune-body `reason`; the common audit header is
the only reason mechanism.

### Exact role records

Use `dcc_permission_t` for permissions and serialize it as a decimal JSON
string. Define these records:

| Type | Mandatory fields | Presence bits in bit order | Nullable bits | Null input |
| --- | --- | --- | --- | --- |
| `dcc_rest_role_colors_t` | `uint32_t primary_color` | `SECONDARY_COLOR` (`uint32_t secondary_color`), `TERTIARY_COLOR` (`uint32_t tertiary_color`) | `SECONDARY_COLOR`, `TERTIARY_COLOR` | invalid when parent `COLORS` is present and non-null |
| `dcc_rest_guild_role_create_t` | none | `NAME` (`const char *name`), `PERMISSIONS` (`dcc_permission_t permissions`), `COLOR` (`uint32_t color`), `COLORS` (`const dcc_rest_role_colors_t *colors`), `HOIST` (`uint8_t hoist`), `ICON` (`const char *icon`), `UNICODE_EMOJI` (`const char *unicode_emoji`), `MENTIONABLE` (`uint8_t mentionable`) | `ICON`, `UNICODE_EMOJI` | `{}` |
| `dcc_rest_guild_role_update_t` | none | the same eight fields and order as create | all eight | `{}` |
| `dcc_rest_guild_role_position_t` | `dcc_snowflake_t role_id` | `POSITION` (`int32_t position`) | `POSITION` | no top-level array: invalid |

All RGB components are 0..`0xFFFFFF`. If tertiary is non-null, enforce the
exact holographic triple: primary 11127295, secondary 16759788, tertiary
16761760. Reject simultaneous deprecated `COLOR` and current `COLORS` fields.
Reject simultaneous non-null icon and Unicode emoji. Role names are valid
UTF-8 and no more than 100 Unicode scalar values. Boolean values are exactly 0
or 1. Create defaults come from Discord when a field is absent.

Role Positions requires a non-null array with at least one element. The first
element's declared `size` is the byte stride; every element must have the same
stride and supported version. Locate elements with checked byte arithmetic,
require alignment, require every role ID to be nonzero, and reject negative
non-null positions. Do not retain the old `uint8_t` position or use
`positions[i]` across historical strides.

### Exact guild, widget, welcome, incident, and sound records

`dcc_rest_guild_update_t` has, after `size/version/present/nulls`, these fields
and bit positions in exact order:

1. `NAME` — `const char *name`
2. `REGION` — `const char *region`
3. `VERIFICATION_LEVEL` — `dcc_guild_verification_level_t verification_level`
4. `DEFAULT_MESSAGE_NOTIFICATIONS` —
   `dcc_guild_default_message_notification_t default_message_notifications`
5. `EXPLICIT_CONTENT_FILTER` —
   `dcc_guild_explicit_content_filter_t explicit_content_filter`
6. `AFK_CHANNEL_ID` — `dcc_snowflake_t afk_channel_id`
7. `AFK_TIMEOUT` — `dcc_guild_afk_timeout_t afk_timeout`
8. `ICON` — `const char *icon`
9. `SPLASH` — `const char *splash`
10. `DISCOVERY_SPLASH` — `const char *discovery_splash`
11. `BANNER` — `const char *banner`
12. `SYSTEM_CHANNEL_ID` — `dcc_snowflake_t system_channel_id`
13. `SYSTEM_CHANNEL_FLAGS` — `uint64_t system_channel_flags`
14. `RULES_CHANNEL_ID` — `dcc_snowflake_t rules_channel_id`
15. `PUBLIC_UPDATES_CHANNEL_ID` — `dcc_snowflake_t public_updates_channel_id`
16. `PREFERRED_LOCALE` — `const char *preferred_locale`
17. `FEATURES` — `const char *const *features`, `size_t feature_count`
18. `DESCRIPTION` — `const char *description`
19. `PREMIUM_PROGRESS_BAR_ENABLED` — `uint8_t premium_progress_bar_enabled`
20. `SAFETY_ALERTS_CHANNEL_ID` — `dcc_snowflake_t safety_alerts_channel_id`

Define `dcc_guild_verification_level_t` with
`DCC_GUILD_VERIFY_NONE = 0`, `DCC_GUILD_VERIFY_LOW = 1`,
`DCC_GUILD_VERIFY_MEDIUM = 2`, `DCC_GUILD_VERIFY_HIGH = 3`, and
`DCC_GUILD_VERIFY_VERY_HIGH = 4`.

The Guild Update nullable mask is REGION, VERIFICATION_LEVEL,
DEFAULT_MESSAGE_NOTIFICATIONS, EXPLICIT_CONTENT_FILTER, AFK_CHANNEL_ID, ICON,
SPLASH, DISCOVERY_SPLASH, BANNER, SYSTEM_CHANNEL_ID, RULES_CHANNEL_ID,
PUBLIC_UPDATES_CHANNEL_ID, PREFERRED_LOCALE, DESCRIPTION, and
SAFETY_ALERTS_CHANNEL_ID. A null body encodes `{}`. Validate name as 2..100
Unicode scalar values with no leading or trailing whitespace; verification
level as 0..4; default notifications as 0..1; content filter as 0..2; AFK
timeout as exactly 60, 300, 900, 1800, or 3600; and boolean as 0 or 1. Validate
every present non-null string as UTF-8, each feature string as non-empty UTF-8,
and every non-null channel ID as nonzero. Do not hardcode the extensible guild
feature or locale set.

Do not serialize the old unsupported `id`, widget fields, `mfa_level`, or
`vanity_url_code`. The old `dcc_guild_params_t` cannot represent omission or
null and is not reused by the endpoint.

The remaining exact records are:

| Type | Mandatory fields | Presence bits in bit order | Nullable bits | Null input |
| --- | --- | --- | --- | --- |
| `dcc_rest_guild_incident_actions_t` | none | `INVITES_DISABLED_UNTIL` (`const char *invites_disabled_until`), `DMS_DISABLED_UNTIL` (`const char *dms_disabled_until`) | both | `{}` |
| `dcc_rest_guild_widget_update_t` | none | `ENABLED` (`uint8_t enabled`), `CHANNEL_ID` (`dcc_snowflake_t channel_id`) | `CHANNEL_ID` | `{}` |
| `dcc_rest_welcome_channel_t` | `dcc_snowflake_t channel_id`, `const char *description` | `EMOJI_ID` (`dcc_snowflake_t emoji_id`), `EMOJI_NAME` (`const char *emoji_name`) | `EMOJI_ID`, `EMOJI_NAME` | invalid when a non-null welcome array uses it |
| `dcc_rest_guild_welcome_screen_update_t` | none | `ENABLED` (`uint8_t enabled`), `WELCOME_CHANNELS` (`const dcc_rest_welcome_channel_t *welcome_channels`, `size_t welcome_channel_count`), `DESCRIPTION` (`const char *description`) | all three | `{}` |
| `dcc_rest_guild_soundboard_sound_create_t` | `const char *name`, `const char *sound` | `VOLUME` (`double volume`), `EMOJI_ID` (`dcc_snowflake_t emoji_id`), `EMOJI_NAME` (`const char *emoji_name`) | all three | invalid |
| `dcc_rest_guild_soundboard_sound_update_t` | none | `NAME` (`const char *name`), `VOLUME` (`double volume`), `EMOJI_ID` (`dcc_snowflake_t emoji_id`), `EMOJI_NAME` (`const char *emoji_name`) | `VOLUME`, `EMOJI_ID`, `EMOJI_NAME` | `{}` |

Incident timestamps are non-null ISO 8601 strings; null disables the action.
The official dynamic limit is 24 hours in the future. Enforce syntax locally
and leave the clock-dependent horizon to Discord unless Task 8 adds one shared,
tested UTC parser rather than ad hoc endpoint parsing.

Welcome screens contain at most five channels. Versioned nested arrays use the
first element's size as stride and the Role Positions checked-stride rules.
Every non-null channel has a nonzero ID and non-null valid UTF-8 description.
Emoji ID and name are independently nullable but cannot both be non-null.
An explicit present, non-null zero-count welcome array encodes `[]`; a present
null bit encodes `null`.

Sound names contain 2..32 Unicode scalar values. `sound` is a structurally
valid MP3 or Ogg data URI with valid base64 and at most 512 KiB decoded bytes.
Measure and validate the decoded span with checked arithmetic before the first
construction allocation. A non-null volume is finite and within 0.0..1.0;
reject NaN and infinities. Emoji ID and name cannot both be non-null. Discord's
5.2-second duration limit remains server-authoritative unless a robust shared
audio parser is introduced. Modify Sound accepts `{}` because all parameters
are optional; do not reject a zero-presence update.

## Validation, serialization, and ownership

Each endpoint performs work in this order:

1. clear a supplied output handle;
2. validate client, nonzero path snowflakes, and Task 7 call options/auth/audit
   capability;
3. validate the complete transitive input graph, including record ABI,
   presence/null masks, pointer/count pairs, nested strides, checked spans,
   IDs, enums, ranges, strings, and conflict rules;
4. serialize the complete literal query and JSON body synchronously;
5. submit through the common typed endpoint helper and transfer only owned
   request data to the queue.

Malformed transitive graphs should reach zero construction allocations, as
observed through the Task 6 endpoint allocation probe. Validate all array
elements before JSON allocation where possible. Never silently clamp or
normalize an invalid caller value.

Every input pointer is borrowed only for the duration of the endpoint call.
Before returning `DCC_OK`, own or serialize path/query text, JSON, feature and
role strings, ID arrays, nested welcome/role records, data URIs, timestamps,
Bearer tokens, and audit reasons. The caller may immediately mutate or free
all of them. Only the callback function and its `user_data` retain the Task 4
borrowed lifetime. Do not serialize on a worker, block a REST worker, wait on a
request, or use a transition future.

JSON semantics are exact: omit absent fields, preserve explicit false/zero,
emit `null` only from a valid null bit, and distinguish `[]` from null. Use the
shared JSON string encoder. Permissions are decimal strings; snowflakes are
JSON strings except where Discord's request contract explicitly requires a
number. Build all JSON and query values before queue admission. No Task 8
endpoint may call a multipart encoder or be marked multipart-capable.

## Remove all 32 legacy Task 8 symbols

After migration, an exact-symbol search must find none of these outside the
historical removal ledger/brief:

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

Also remove raw query/JSON forms of the 47 retained canonical names by changing
those names in place to the signature in this brief. Public body-build helpers
do not survive. Generic `dcc_rest_submit()` remains the raw escape hatch.

## Manifest, implementation, and consumer migration

Update `tools/rest_v2_endpoints.json` first in the RED fixture and then make it
truthful. Remove the four stale entries, change Task 8's active count to 47,
change the whole manifest's active count to 224, record the exact typed inputs
and canonical parameters above, set all 47 multipart flags false, record the
21 audit-capable operations, record per-endpoint allowed/default auth modes,
and retain only the current-nickname deprecation marker. Remove unused stale
route identities. Extend the schema/audit if auth and audit capability are not
already mandatory Task 7 fields.

The primary public type owners are:

- `include/dcc/rest/types/guild_members.h`
- `include/dcc/rest/types/guilds.h`
- `include/dcc/rest/types/roles.h`
- their focused/aggregate include surfaces under `include/dcc/rest/types.h`

The endpoint declaration owners are:

- `include/dcc/rest/guild_members/lifecycle.h`
- `include/dcc/rest/guild_members/query.h`
- `include/dcc/rest/guild_members/roles.h`
- `include/dcc/rest/guilds/audit_log.h`
- `include/dcc/rest/guilds/bans.h`
- `include/dcc/rest/guilds/core.h`
- `include/dcc/rest/guilds/current_member.h`
- `include/dcc/rest/guilds/integrations.h`
- `include/dcc/rest/guilds/prune.h`
- `include/dcc/rest/guilds/welcome_screen.h`
- `include/dcc/rest/guilds/widgets.h`
- `include/dcc/rest/official_surface.h`
- `include/dcc/rest/resources/users.h`
- `include/dcc/rest/resources/voice_states.h`
- `include/dcc/rest/roles.h`

The direct implementation owners include:

- `src/rest/rest_guild_members_lifecycle.c`
- `src/rest/rest_guild_members_query.c`
- `src/rest/rest_guild_member_roles.c`
- `src/rest/rest_guilds_audit_log.c`
- `src/rest/rest_guild_bans_fetch.c`
- `src/rest/rest_guild_bans_modify.c`
- `src/rest/rest_guilds_core.c`
- `src/rest/rest_guilds_current_member.c`
- `src/rest/rest_guilds_integrations.c`
- `src/rest/rest_guilds_prune.c`
- `src/rest/rest_guilds_welcome.c`
- `src/rest/rest_guilds_widget.c`
- `src/rest/rest_official_surface.c`
- `src/rest/rest_users.c`
- `src/rest/rest_voice_regions.c`
- `src/rest/rest_roles_create.c`
- `src/rest/rest_roles_delete.c`
- `src/rest/rest_roles_fetch.c`
- `src/rest/rest_roles_modify.c`
- `src/rest/rest_roles_positions.c`

Remove or refactor the legacy body/control helpers
`rest_guild_bans_create_params.c`, `rest_guild_member_builders.c`,
`rest_guild_member_controls.c`, `rest_guild_prune_body.c`,
`rest_guild_body.c`, `rest_guild_body_tail.c`,
`rest_guild_body_validate.c`, `rest_guilds_current_member_body.c`,
`rest_guilds_welcome_body.c`, and `rest_role_body.c` as their callers are
migrated. Split new serializers/validators by record family when needed; do
not turn `rest_official_surface.c` into a larger catch-all.

Migrate every in-tree consumer in the same implementation commit. The reviewed
consumer set includes:

- transition App mirrors in `src/app/app_rest_shortcuts.c`,
  `src/app/app_rest_shortcuts_guilds.c`, and
  `src/app/app_rest_shortcuts_resources.c`, plus mirrored declarations/types
  in `include/dcc/app/legacy.h`;
- `include/dcc/sugar/official_surface.h`,
  `include/dcc/sugar/guild_shortcuts/application.h`,
  `include/dcc/sugar/guild_shortcuts/guild_admin.h`,
  `include/dcc/sugar/guild_shortcuts/media.h`,
  `include/dcc/sugar/guild_shortcuts/members.h`,
  `include/dcc/sugar/guild_shortcuts/moderation.h`,
  `include/dcc/sugar/command_routes/guild_roles.h`, and
  `include/dcc/sugar/command_routes/members.h` when their App/REST target
  changes;
- `tests/package_consumer/main.c`,
  `tests/package_consumer/package_consumer_app.c`, and
  `tests/package_consumer/package_consumer_official_surface.c`;
- `tests/support/http_smoke_gap.c`,
  `tests/support/http_smoke_official_surface.c`,
  `tests/support/http_smoke_rest_calls_channels.c`,
  `tests/support/http_smoke_rest_calls_guilds.c`,
  `tests/support/http_smoke_rest_calls_users_threads.c`, and the corresponding
  `http_smoke_rest_wrapper_channels.c`, `http_smoke_rest_wrapper_guilds.c`, and
  `http_smoke_rest_wrapper_users_threads.c` expectation/support units;
- `docs/reference/official-api-surface.md`, `CMakeLists.txt`,
  `tools/rest_v2_endpoints.json`, `tools/audit_rest_v2_endpoints.py`,
  `tools/audit_discord_api_docs_surface.py`,
  `tools/audit_official_surface.py`, `tools/audit_rest_surface.py`,
  `tools/audit_rest_field_smoke.py`, and
  `tools/audit_rest_request_assertions.py`.

Repeat an exact-symbol search after implementation because the compiler and
manifest audit may reveal additional aggregate-header or generated-fixture
consumers. Task 10 eventually removes App/Sugar transition APIs, but Task 8
must keep the current tree buildable or deliberately coordinate their early
removal rather than leave calls to deleted REST declarations.

There are no direct command-registry calls to the exact Task 8 endpoint names
at the reviewed base. Keep command-registry and command-sync tests as
regression gates because role/member type headers and App mirrors are shared.

## Required RED tests

Extend the focused endpoint fixture before production changes and capture RED
for every category below. Use the local HTTP server or interceptor; do not call
live Discord.

### Inventory and public surface

- The official-docs/manifest audit fails on at least one of the four stale
  operations. The current audit filters extra DCC routes and is insufficient;
  make unsupported manifest extras fatal.
- The manifest schema expects active counts 41/35/47/57/44 and total 224, then
  progress-through-8 expects 123 strict and 101 transition operations.
- All 47 canonical declarations and definitions match exactly, all 32 legacy
  symbols and four stale canonical symbols are rejected, and no canonical
  declaration contains callback/user-data/raw JSON/raw query/terminal suffixes.
- Changed public headers compile standalone as strict C11 and C++17 in both
  normal and reverse include orders; i686 and x86_64 MinGW probes cover the
  same headers and changed production sources.

### Literal wire matrix

Capture one exact method/path/query/body case for each of the 47 operations.
The high-risk cases include:

- Get Guild with explicit present `with_counts=false`;
- Get Current User Guilds with limit 200 and explicit false counts;
- ban/member limits of 1000 and search-query percent encoding;
- both ban cursors preserving the documented before precedence;
- audit `after=0` and member `after=0` serialization;
- prune defaults, comma-delimited roles, and explicit false compute count;
- every widget style and public widget requests with no Authorization header;
- Bot versus Bearer current-user requests and locally rejected auth modes;
- permissions as decimal strings, role colors, nullable role positions, and
  the exact holographic triple;
- ban seconds 0 and 604800, with 604801 rejected rather than clamped;
- explicit null, false, zero, `[]`, and absent fields for every nullable body
  family;
- sound data URI JSON with no multipart boundary or content type.

### ABI and validation matrix

For every new record, cover the current initializer macro/function, null
initializer, current layout, accepted larger version-1 layout, accepted
covered historical prefix, zero/short size, bad version, unknown present/null
bits, present-but-uncovered fields, null-without-present, nonnullable null bits,
poisoned uncovered storage, and unchanged surrounding canaries. For every
versioned array, cover two distinct historical-stride elements, mismatched
stride/version, misalignment, and `SIZE_MAX` span/address overflow without
undefined behavior.

Boundary RED cases include path ID zero; query limits zero and max plus one;
prune days 0 and 31; bulk-ban counts 0 and 201; zero IDs inside arrays; role
name 101 scalar values; RGB above `0xFFFFFF`; an invalid holographic triple;
simultaneous old/new role color; simultaneous icon/Unicode emoji; negative
role position; guild name 1/101 or leading/trailing whitespace; invalid guild
enums/AFK timeout; welcome count 6 and malformed nested records; sound name
1/33, NaN/infinite/out-of-range volume, conflicting emoji, bad data URI/base64,
and decoded data over 512 KiB; malformed timestamps and UTF-8; and checked
pointer/count overflow.

### Audit, ownership, and async lifecycle

- Cover each of the exact 21 eligible operations with one encoded
  `X-Audit-Log-Reason` header and reject a reason on representative ineligible
  guild/member/role operations before allocation/admission.
- Audit-reason boundaries are 512 ASCII scalars accepted, 513 rejected, and
  512 Korean or emoji scalars accepted after producing an encoded header longer
  than 512 bytes. Reject malformed UTF-8. Do not use encoded length as the
  semantic limit.
- Hold the sole REST worker with a blocker, submit borrowed guild-feature and
  member-role arrays, bulk IDs, prune roles, role strings/colors, nested
  welcome records/strings, sound data URI, timestamps, Bearer token, and audit
  reason, then mutate/free every input before release. The server must receive
  the original bytes.
- For representative GET, PATCH, and array-body calls, prove the output handle
  is published before callback eligibility, callback and retained handle see
  one identical terminal result, wait/cancel/auto-release behave per Task 4,
  and delayed submission returns without waiting for HTTP completion.
- Snapshot handle, queue, callback, observer, interceptor, and allocation
  counters on every local validation/admission rejection. The output remains
  null and all terminal-side counters remain zero.

### Migration regressions

- Exact-symbol search makes all 32 legacy names and four stale canonical names
  zero outside the removal record.
- App, Sugar, package-consumer, official-surface, complete HTTP smoke, command
  registry/sync, and public API audits compile and run with no transition call
  to a removed declaration.
- The active manifest has exactly 47 Task 8 entries, exactly 21 audit-enabled
  entries, and exactly zero multipart entries.

## Verification and handoff

Run at minimum:

- `python3 tools/audit_rest_v2_endpoints.py --source . --progress-through 8`
  after Task 7 is green;
- the endpoint-manifest audit and malformed-schema self-tests;
- `dcc_rest_v2_request_smoke`, `dcc_rest_v2_endpoint_smoke`, complete
  `dcc_http_smoke`, `dcc_official_surface_smoke`, and
  `dcc_official_surface_headers_smoke`;
- REST surface, request-assertion, field-smoke, public-API, and DCC 2 surface
  audits;
- App, Sugar, package-consumer, command-registry, and command-sync focused
  tests;
- a current local Discord documentation checkout through
  `DCC_DISCORD_API_DOCS_ROOT=/path/to/discord-api-docs` and the two-way official
  surface audit;
- standalone strict C11/C++17 headers and i686/x86_64 MinGW header/source
  probes;
- focused ASan/UBSan for every nested graph/stride/overflow/ownership path and
  TSan for delayed submit, cancel, callback, and owned auth/audit metadata;
- full enabled CTest, MkDocs strict, install plus standalone package consumer,
  source/release audits, and `git diff --check`.

The focused CTest selection must include the existing endpoint manifest/schema,
request/endpoint, HTTP, official-surface/header, REST-surface,
request-assertion, field-smoke, public-API, App, Sugar, package-consumer, and
command-sync targets. Record exact commands, counts, and outputs rather than
only stating that tests passed.

Commit implementation, manifest, audit, tests, migrations, and documentation
with exact message `feat: migrate guild REST endpoints to requests`. Then
write `task-8-implementer-report.md` beside this brief with RED evidence; the
47/21/0, 41/35/47/57/44, and 76/123/101/224 progress counts; the retained
endpoint and removed symbol lists;
record layouts, minimum prefixes, presence/null masks, and auth/audit matrix;
ownership and allocation evidence; exact native/sanitizer/MinGW/package/docs
commands and results; known pre-existing failures; and the implementation
commit hash. Commit the report separately and stop for independent review.
