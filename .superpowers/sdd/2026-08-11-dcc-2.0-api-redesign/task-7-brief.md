# Task 7 brief — request-based channel, thread, and invite endpoints

Implement only Task 7 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The REST section
of the design, the 2026-08-11 official Discord documentation inventory, and
this brief are authoritative. Work test-first, do not push, and do not edit the
ignored progress ledger. Do not start Task 8 domains. Keep the RED contract
commit separate from the GREEN implementation commit and keep every committed
state buildable.

## Outcome

Migrate exactly 35 active channel, thread, and invite operations to the Task 4
request handle: 12 channel, 13 thread, and 10 invite entries. Every operation
has one typed public submission function, validates and owns all data needed
after return, and ends with call options plus an optional request handle.

Remove the 18 reviewed REST aliases and overloads for these operations. Raw
endpoint JSON, raw query strings, callback/user-data parameters, `_params`,
`_page`, and thread aliases do not survive in the installed REST surface.
`dcc_rest_submit()` is the only raw escape hatch. Task 8–10 endpoints remain
explicit transition debt.

Task 7 also appends audit-reason and authentication policy to the Task 4 call
options without breaking its historical prefix. The manifest becomes a checked
inventory of exactly 224 active endpoints, not the stale 231-candidate
baseline. Task ownership is 41/35/47/57/44 for Tasks 6/7/8/9/10; after Task 7,
the progress audit reports 76 migrated and 148 deferred active endpoints.

## Canonical submission and ownership contract

Every canonical endpoint ends with:

```c
const dcc_rest_call_options_t *options,
dcc_rest_request_t **out_request
```

Null options select the full current initializer. Null `out_request` means
auto-release. A local validation, allocation, runtime-state, or admission
failure sets a supplied `*out_request` to null, queues nothing, and invokes
neither callback nor error observer. `DCC_OK` guarantees exactly one terminal
result through the Task 4 lifecycle.

Before returning, copy or serialize every path segment, query value, audit
reason, Bearer token, JSON field, string, array, nested record, filename, file
metadata value, and exact file byte span needed by the worker. Only the callback
function and its `user_data` remain borrowed until callback return. Do not wait
on a request, submit through the legacy future family, or retain a pointer into
a caller-owned input record.

All new evolvable input records begin with `size`, `version`, and a presence
mask. Nullable values must distinguish omission, explicit null, and an actual
zero/false value; use an explicit null mask or an equivalent typed state, never
a sentinel that collapses those states. Provide a version constant, complete
initializer macro, and null-safe initializer. Header-only init/set convenience
functions are `static inline`, not new exported `dcc_rest_*` operations.
Serialize only present fields and let Discord apply documented defaults; DCC
must not inject convenience defaults for omitted channel, thread, invite, or
query fields.
Accept larger layouts and valid historical prefixes. Reject an unsupported
version, unknown bits, a present-but-uncovered field, a partially covered
field, malformed nested layout, invalid pointer/count pair, or arithmetic
overflow before endpoint-owned allocation or output mutation. Versioned arrays
use their validated declared element stride rather than the library's current
`sizeof`.

## Historical-prefix-safe call options

Append this exact tail to the existing version-1 type; do not add a separate
presence mask and do not increment its version:

```c
typedef enum dcc_rest_auth_mode {
    DCC_REST_AUTH_DEFAULT = 0,
    DCC_REST_AUTH_NONE,
    DCC_REST_AUTH_BOT,
    DCC_REST_AUTH_BEARER
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
} dcc_rest_call_options_t;
```

The historical mandatory prefix ends after `user_data`. Read each appended
field only when `size` fully covers that field and default every uncovered
suffix field to null/default authentication. Reject a size that ends inside a
logical field. `DCC_REST_CALL_OPTIONS_INIT` and
`dcc_rest_call_options_init()` initialize the full current size, null audit
reason, `DCC_REST_AUTH_DEFAULT`, and null token.

The embedded options inside `dcc_rest_request_desc_t` obey the same rule. Its
outer declared size must cover every nested option byte that the nested option
size claims. A descriptor and nested options compiled to the Task 4 layout
remain accepted. Never copy either caller structure with a current-size raw
assignment or `memcpy`.

Authentication semantics are exact:

- `DEFAULT` selects the typed endpoint's private manifest policy; for a
  relative raw Discord request it selects Bot authentication.
- `BOT` uses the client's configured bot token and rejects an override token.
- `BEARER` requires both the mode and token fields to be covered and a non-empty
  token. Copy it before return, exclude it from logs, errors, route keys, and
  observers, and wipe the owned bytes before free.
- `NONE` suppresses authorization only when the endpoint capability permits
  it. An `auth_token` with any non-Bearer mode is invalid.
- General Task 7 channel, thread, invite, and target-user endpoints are
  Bot-only. `dcc_rest_get_invite` alone defaults to `NONE` and permits exactly
  `NONE` or `BOT`. `dcc_rest_create_lobby_channel_invite_for_self` is
  Bearer-only and rejects `DEFAULT`, because no per-call token can be inferred.
  `dcc_rest_create_lobby_channel_invite_for_user` is Bot-only and accepts
  `DEFAULT` or `BOT`. No Task 7 endpoint accepts arbitrary Bearer auth outside
  the self-lobby operation.

`audit_log_reason` is an unencoded UTF-8 string. Validate canonical UTF-8 and
1 through 512 Unicode scalar values, then RFC 3986 percent-encode its UTF-8
bytes exactly once for `X-Audit-Log-Reason`. The 512 limit applies before
encoding. Copy both the logical value needed during preparation and its owned
transport representation before return. Empty, over-limit, noncanonical, or
malformed UTF-8 is a local rejection.

Exactly these 12 Task 7 canonical endpoints accept an audit reason:

- `dcc_rest_create_guild_channel`
- `dcc_rest_modify_channel`
- `dcc_rest_delete_channel`
- `dcc_rest_modify_channel_permission`
- `dcc_rest_delete_channel_permission`
- `dcc_rest_follow_news_channel`
- `dcc_rest_set_channel_voice_status`
- `dcc_rest_create_thread_from_message`
- `dcc_rest_create_thread`
- `dcc_rest_create_forum_thread`
- `dcc_rest_create_channel_invite`
- `dcc_rest_delete_invite`

Every other Task 7 endpoint, including
`dcc_rest_modify_guild_channel_positions`, rejects a non-null reason before
allocation. The current official Modify Guild Channel Positions section does
not advertise audit-reason support; do not inherit the preceding Create Guild
Channel section's capability.

## Raw and absolute URL policy

Typed endpoints always construct a relative Discord route internally. They
never accept a caller path or URL. A relative `dcc_rest_submit()` request may
use the call-options audit reason and `DEFAULT`, `NONE`, `BOT`, or `BEARER`
authentication subject to the validation above; its default is Bot auth.

An absolute `http://` or `https://` raw URL accepts only the default, empty
Discord-specific option state: no audit reason, no Bearer token, and no auth
override. It never receives the client bot token or any other DCC credential.
Reject rather than silently discard an audit reason or non-default auth state.
Do not include credentials in redirects, diagnostics, observer payloads, or
route/rate-limit keys. Preserve the existing rule that path, content type, and
exact body bytes are copied before admission.

## Complete Task 7 endpoint manifest

Update `tools/rest_v2_endpoints.json` and its audit from the candidate baseline
to exactly 224 active entries. The active task counts are 41, 35, 47, 57, and
44. Keep the seven unavailable candidates only in an explicit removal ledger:
Create Guild, Delete Guild, Modify Guild Integration, Sync Guild Integration,
disabled Bulk Edit Guild Command Permissions, unavailable Create Guild From
Template, and obsolete bot-facing Get Current User DMs. They are not active
canonical endpoints and must not be installed.

For the tables below, `A` means audit-reason capable and `M` means multipart
capable. An omitted capability means the option is rejected locally. Input
names are normative; scalar path arguments precede the typed input, and every
row then ends in common call options/request output.

### Channels — 12

| Canonical operation | Method and normalized route | Typed input | Capabilities | Auth |
| --- | --- | --- | --- | --- |
| `dcc_rest_create_dm_channel` | `POST /users/@me/channels` | `dcc_dm_channel_params_t` | — | Bot |
| `dcc_rest_create_guild_channel` | `POST /guilds/{guild_id}/channels` | `dcc_channel_params_t` | A | Bot |
| `dcc_rest_delete_channel` | `DELETE /channels/{channel_id}` | none | A | Bot |
| `dcc_rest_delete_channel_permission` | `DELETE /channels/{channel_id}/permissions/{overwrite_id}` | none | A | Bot |
| `dcc_rest_follow_news_channel` | `POST /channels/{channel_id}/followers` | `dcc_rest_follow_news_channel_t` | A | Bot |
| `dcc_rest_get_channel` | `GET /channels/{channel_id}` | none | — | Bot |
| `dcc_rest_get_guild_channels` | `GET /guilds/{guild_id}/channels` | none | — | Bot |
| `dcc_rest_modify_channel` | `PATCH /channels/{channel_id}` | `dcc_channel_params_t` | A | Bot |
| `dcc_rest_modify_channel_permission` | `PUT /channels/{channel_id}/permissions/{overwrite_id}` | `dcc_channel_permission_overwrite_t` | A | Bot |
| `dcc_rest_modify_guild_channel_positions` | `PATCH /guilds/{guild_id}/channels` | `dcc_channel_positions_params_t` | — | Bot |
| `dcc_rest_set_channel_voice_status` | `PUT /channels/{channel_id}/voice-status` | `dcc_channel_voice_status_params_t` | A | Bot |
| `dcc_rest_trigger_channel_typing` | `POST /channels/{channel_id}/typing` | none | — | Bot |

### Threads — 13

| Canonical operation | Method and normalized route | Typed input | Capabilities | Auth |
| --- | --- | --- | --- | --- |
| `dcc_rest_add_thread_member` | `PUT /channels/{thread_id}/thread-members/{user_id}` | none | — | Bot |
| `dcc_rest_create_forum_thread` | `POST /channels/{channel_id}/threads` | `dcc_thread_params_t` | A, M | Bot |
| `dcc_rest_create_thread` | `POST /channels/{channel_id}/threads` | `dcc_thread_params_t` | A | Bot |
| `dcc_rest_create_thread_from_message` | `POST /channels/{channel_id}/messages/{message_id}/threads` | `dcc_thread_params_t` | A | Bot |
| `dcc_rest_get_active_threads` | `GET /guilds/{guild_id}/threads/active` | none | — | Bot |
| `dcc_rest_get_joined_private_archived_threads` | `GET /channels/{channel_id}/users/@me/threads/archived/private` | `dcc_rest_joined_thread_archive_query_t` | — | Bot |
| `dcc_rest_get_private_archived_threads` | `GET /channels/{channel_id}/threads/archived/private` | `dcc_rest_thread_archive_query_t` | — | Bot |
| `dcc_rest_get_public_archived_threads` | `GET /channels/{channel_id}/threads/archived/public` | `dcc_rest_thread_archive_query_t` | — | Bot |
| `dcc_rest_get_thread_member` | `GET /channels/{thread_id}/thread-members/{user_id}` | `dcc_rest_thread_member_query_t` | — | Bot |
| `dcc_rest_get_thread_members` | `GET /channels/{thread_id}/thread-members` | `dcc_rest_thread_member_query_t` | — | Bot |
| `dcc_rest_join_thread` | `PUT /channels/{thread_id}/thread-members/@me` | none | — | Bot |
| `dcc_rest_leave_thread` | `DELETE /channels/{thread_id}/thread-members/@me` | none | — | Bot |
| `dcc_rest_remove_thread_member` | `DELETE /channels/{thread_id}/thread-members/{user_id}` | none | — | Bot |

The ordinary and forum thread creates deliberately share one method/template
but have different typed semantics and multipart capability. Keep this as the
single reviewed Task 7 collision justification. `dcc_rest_get_thread` is an
alias of Get Channel, not another endpoint.

### Invites — 10

| Canonical operation | Method and normalized route | Typed input | Capabilities | Auth |
| --- | --- | --- | --- | --- |
| `dcc_rest_create_channel_invite` | `POST /channels/{channel_id}/invites` | `dcc_invite_params_t` | A, M | Bot |
| `dcc_rest_create_lobby_channel_invite_for_self` | `POST /lobbies/{lobby_id}/members/@me/invites` | none | — | Bearer |
| `dcc_rest_create_lobby_channel_invite_for_user` | `POST /lobbies/{lobby_id}/members/{user_id}/invites` | none | — | Bot |
| `dcc_rest_delete_invite` | `DELETE /invites/{invite_code}` | none | A | Bot |
| `dcc_rest_get_channel_invites` | `GET /channels/{channel_id}/invites` | none | — | Bot |
| `dcc_rest_get_guild_invites` | `GET /guilds/{guild_id}/invites` | none | — | Bot |
| `dcc_rest_get_invite` | `GET /invites/{invite_code}` | `dcc_rest_invite_query_t` | — | None/Bot; default None |
| `dcc_rest_get_invite_target_users` | `GET /invites/{invite_code}/target-users` | none | — | Bot |
| `dcc_rest_get_invite_target_users_job_status` | `GET /invites/{invite_code}/target-users/job-status` | none | — | Bot |
| `dcc_rest_put_invite_target_users` | `PUT /invites/{invite_code}/target-users` | `dcc_rest_invite_target_users_upload_t` | M | Bot |

All snowflake path values are nonzero. Invite codes are non-empty path segments
and are percent-encoded exactly once; encoded separators must stay inside the
segment. Route tests compare against literal expected paths rather than the
same formatter used by production.

## Channel input contracts

Redefine the current channel inputs as version-1, presence-gated records with
no embedded guild/channel path IDs. `dcc_channel_params_t` has an explicit
payload kind for guild channel, group DM, or thread modification and uses a
tagged payload rather than preserving the current all-purpose field bag.
`dcc_rest_create_guild_channel` accepts only the guild-channel tag and requires
`name`; `dcc_rest_modify_channel` accepts all three tags. Endpoint-specific
inactive fields are invalid, not silently ignored.

The 2026 field contract is:

- Create guild channel: required `name`; optional `type`, `topic`, `bitrate`,
  `user_limit`, `rate_limit_per_user`, `position`, permission overwrites,
  `parent_id`, `nsfw`, `rtc_region`, `video_quality_mode`,
  `default_auto_archive_duration`, `default_reaction_emoji`, `available_tags`,
  `default_sort_order`, `default_forum_layout`,
  `default_thread_rate_limit_per_user`, and `flags`.
- Modify guild channel: optional `name`, `type`, `position`, `topic`, `nsfw`,
  `rate_limit_per_user`, `bitrate`, `user_limit`, permission overwrites,
  `parent_id`, `rtc_region`, `video_quality_mode`,
  `default_auto_archive_duration`, `flags`, `available_tags`,
  `default_reaction_emoji`, `default_thread_rate_limit_per_user`,
  `default_sort_order`, and `default_forum_layout`.
- Modify group DM: only `name` and base64 image-data `icon`.
- Modify thread: only `name`, `archived`, `auto_archive_duration`, `locked`,
  `invitable`, `rate_limit_per_user`, `flags`, and `applied_tags`.

Enforce names at 1–100 characters. Create Guild Channel topics are 0–1024
characters for every supported type; Modify Channel permits 0–4096 only for
forum/media and 0–1024 otherwise. Slowmode values are 0–21600 seconds,
voice/stage bitrate is in bits per second with an 8000 minimum, voice user limit
is at most 99, and stage user limit is at most 10000. Do not retain the
incorrect `bitrate_kbps` unit or an 8-bit user-limit field. Auto-archive accepts
only 60, 1440, 4320, or 10080 minutes. Video quality accepts only 1 or 2.
Create permits `nsfw` for text, voice, announcement, stage, and forum; Modify
also permits it for media. Validate every field against both operation and
tagged channel type before serialization.

Available forum/media tags are limited to 20; tag names are 0–20 characters,
and at most one of custom `emoji_id` and Unicode `emoji_name` is non-null. A
default reaction has exactly one of those two values when non-null. Applied
tags are limited to five nonzero IDs. Expose channel `flags` directly and
validate known/type-appropriate bits; remove the redundant `require_tag` and
`hide_media_download_options` booleans. `PINNED` is thread-only,
`REQUIRE_TAG` is forum/media-only, and `HIDE_MEDIA_DOWNLOAD_OPTIONS` is
media-only.

`dcc_channel_positions_params_t` owns a non-empty array of versioned position
records. Each record requires a nonzero channel ID and supports independent
presence for `position`, `lock_permissions`, nullable `parent_id`, and the
2026 `flags` field. Reject duplicate IDs and more than one record changing
`parent_id`; Discord permits at most one parent move per request. Preserve the
top-level JSON array shape. This endpoint is not audit-reason capable.

The edit-permission body requires overwrite `type` 0 (role) or 1 (member), and
supports independently present `allow` and `deny` permission bitfields,
serialized as decimal strings. The overwrite ID exists only in the path; array
overwrite records used by channel create/modify require their own ID. Do not
conflate these two shapes or serialize a redundant/mismatched body ID.

`dcc_rest_follow_news_channel_t` contains the required nonzero
`webhook_channel_id`. `dcc_channel_voice_status_params_t` contains a required
present nullable `status`; non-null status is at most 500 characters and null
clears it. `dcc_dm_channel_params_t` contains the required nonzero
`recipient_id`. These records contain no path ID and serialize the official
field spelling exactly.

## Thread input and pagination contracts

Make `dcc_thread_params_t` versioned and presence-gated, with no embedded
channel, thread, or message path IDs and no raw `message_json`. The submitting
operation determines which fields are legal:

- From message requires `name` and accepts `auto_archive_duration` and
  `rate_limit_per_user`.
- Without message requires `name` and accepts `auto_archive_duration`, `type`,
  `invitable`, and `rate_limit_per_user`. `invitable` is legal only for a
  private thread. Accept only documented thread channel types.
- Forum/media thread requires `name` and an initial typed message payload, and
  accepts `auto_archive_duration`, `rate_limit_per_user`, and up to five
  `applied_tags`. It rejects `type` and `invitable`.

Names are 1–100 characters, slowmode is 0–21600 seconds, and auto-archive uses
the four documented minute values. The forum/media message reuses the Task 6
typed message builder and fixed multipart files; it allows only `content`, up
to ten embeds, allowed mentions, components, up to three sticker IDs,
attachment metadata, and message flags `SUPPRESS_EMBEDS` and
`SUPPRESS_NOTIFICATIONS`. At least one of content, embeds, sticker IDs,
components, or a file is required. Reject every other message field and flag.
The complete forum request, including files, is limited to 25 MiB.

`dcc_rest_thread_archive_query_t` has independently present `before` and
`limit`. `before` is an ISO-8601 timestamp string for public/private archives,
not the current numeric Unix timestamp. Copy it and percent-encode it exactly
once. `dcc_rest_joined_thread_archive_query_t` instead uses a nonzero snowflake
`before`. For all three archive endpoints, null query means defaults and a
present limit must be positive; do not invent an undocumented Discord upper
bound. Remove the three `_page` APIs.

`dcc_rest_thread_member_query_t` supports `with_member`, `after`, and `limit`.
Get Thread Member permits only `with_member`. List Thread Members permits all
three; limit is 1–100 and `after` is a nonzero user snowflake. Because DCC's
current base route is API v10, `after` or `limit` requires `with_member=true`;
preserve explicit false serialization. Null query selects Discord defaults.
Archive and member query order is stable and tests assert literal percent-
encoded query strings.

## Invite input contracts

Make `dcc_invite_params_t` a version-1 presence-gated body record with no
embedded channel ID. It exposes exactly the 2026 Create Channel Invite fields:
`max_age`, `max_uses`, `temporary`, `unique`, `target_type`,
`target_user_id`, `target_application_id`, optional target-users CSV file, and
`role_ids`. Preserve explicit zero and false. `max_age` is 0–604800 seconds and
`max_uses` is 0–100. Target type 1 requires only a nonzero target user; target
type 2 requires only a nonzero embedded application; absent target type rejects
either target ID. Role IDs are a validated copied array of nonzero snowflakes.
Even with no fields, send the required JSON body `{}`.

When a target-users CSV is present, create the invite as multipart with the
fixed `target_users_file` form name and encode every non-file member inside
`payload_json`. Copy the filename and exact byte span, preserve embedded NULs,
and reject empty data, invalid pointer/length pairs, and CR/LF in file metadata.
Do not locally reinterpret CSV contents that Discord processes asynchronously.

`dcc_rest_invite_query_t` supports only presence-gated `with_counts` and
`guild_scheduled_event_id`; explicit false is serialized. The obsolete
`with_expiration` parameter from `dcc_rest_get_invite_full` is not part of the
2026 contract. `dcc_rest_invite_target_users_upload_t` requires filename plus
a non-empty exact CSV byte span and always builds multipart with the fixed
`target_users_file` field. GET target users returns the exact CSV response in
the normal result body/body length; job-status GET remains scalar-only.

The only three multipart-capable Task 7 operations are therefore:

1. `dcc_rest_create_forum_thread` — optional message files;
2. `dcc_rest_create_channel_invite` — optional target-users CSV;
3. `dcc_rest_put_invite_target_users` — required target-users CSV.

For all three, run complete structured validation and exact aggregate
measurement before the first JSON or multipart allocation. Then build the
entire body before queue admission using Task 6's checked multipart primitives.
Test zero and multiple forum files, optional invite CSV, required update CSV,
embedded NULs, caller mutation/free after return, overflow, and cancellation.

Official contract sources are Discord's current [Guild resource](https://docs.discord.com/developers/resources/guild),
[Channel resource](https://docs.discord.com/developers/resources/channel),
[Invite resource](https://docs.discord.com/developers/resources/invite),
[User resource](https://docs.discord.com/developers/resources/user),
[Lobby resource](https://docs.discord.com/developers/resources/lobby), and
[API authentication reference](https://docs.discord.com/developers/reference).

## Legacy removals and App migration

Remove exactly these 18 public REST symbols and their external definitions:

- `dcc_rest_add_thread_member_params`
- `dcc_rest_create_channel_invite_params`
- `dcc_rest_create_dm_channel_params`
- `dcc_rest_create_forum_thread_params`
- `dcc_rest_create_guild_channel_params`
- `dcc_rest_create_thread_params`
- `dcc_rest_create_thread_from_message_params`
- `dcc_rest_get_thread`
- `dcc_rest_get_invite_full`
- `dcc_rest_get_joined_private_archived_threads_page`
- `dcc_rest_get_private_archived_threads_page`
- `dcc_rest_get_public_archived_threads_page`
- `dcc_rest_modify_channel_params`
- `dcc_rest_modify_thread`
- `dcc_rest_modify_thread_params`
- `dcc_rest_modify_channel_permission_params`
- `dcc_rest_modify_guild_channel_positions_params`
- `dcc_rest_set_channel_voice_status_params`

The canonical `dcc_rest_create_forum_thread` replaces the planned-at-baseline
`_params` symbol. `dcc_rest_get_channel` serves channels and threads.
`dcc_rest_modify_channel` serves guild channels, group DMs, and threads through
its tagged typed input. Remove empty leaf headers/units or references left only
for the deleted thread aliases.

Migrate every in-tree caller in the same GREEN commit: HTTP/official/sugar and
package fixtures, examples, generated shortcuts, and the transition App REST
mirrors in `include/dcc/app/legacy.h` plus `src/app/app_rest_shortcuts*.c`.
Task 10 still owns removal of the App mirror surface, so existing App symbols
may remain only as transition adapters. Typed mirrors create call options from
their callback/user data and submit the canonical `dcc_rest_*` function through
`dcc_app_client(app)`. Raw-JSON App mirrors may use the private Task 6 legacy
raw adapter over `dcc_rest_submit()` until Task 10; they must not resurrect a
public raw REST overload.

In particular, rewrite App thread create/modify/archive/lock/unlock helpers to
the canonical thread create or typed `dcc_rest_modify_channel`; rewrite channel,
permission, position, invite, and DM mirrors to the matching canonical request.
The composed send-with-thread state machine deep-copies only the fields legal
for Start Thread from Message and never copies the full legacy thread record.
Do not add App mirrors for the newly canonical forum, target-user, or lobby
operations. No adapter waits on a request, reaches the legacy async/future
machinery directly, or suppresses Task 3 observer delivery. Update sugar macros
and package consumers so removed REST names no longer compile anywhere in-tree.

## Manifest and audit rules

Extend each active manifest entry with checked authentication and audit-reason
capabilities in addition to its method, route, owner, typed input, canonical
parameters, multipart flag, source evidence, and legacy list. The audit must
prove all of the following:

- exactly 224 active endpoint entries with task counts 41/35/47/57/44;
- exactly 35 Task 7 entries split 12 channels, 13 threads, and 10 invites;
- exactly the 18 legacy Task 7 symbols above, all absent after migration;
- exactly three Task 7 multipart entries and exactly 12 audit-reason entries;
- the exact per-symbol auth matrix, including self-lobby Bearer-only,
  user-lobby Bot-only, and Get Invite None/Bot;
- the common client/options/request-output positions and exact public
  declaration/external-definition equality;
- typed-input correspondence, including a query on both thread-member GET
  forms, and numeric/string path-role arity for every normalized route;
- the one justified ordinary/forum thread collision and no alias disguised as
  a second endpoint;
- no unclassified endpoint, unavailable removal-ledger symbol, public raw JSON
  overload, callback/user-data endpoint signature, future/sync/async variant,
  terminal overload suffix, App REST mirror bypass, or exported init/set helper.

Audit source behavior rather than accepting manifest claims alone. A typed
endpoint must pass a private auth/audit policy to the shared submitter; a
manifest boolean that is not enforced is a failure. Add malformed-manifest
self-tests for every new field, count, auth mode, capability mismatch, stale
231 inventory, unavailable endpoint resurrection, wrong thread-member query,
wrong multipart set, and the false audit capability on channel positions.

`--progress-through 7` is the registered CTest gate. It is strict for Tasks 6
and 7 and permits violations only for exact Task 8–10 `(task, domain)` pairs.
It reports 224 active endpoints and 148 explicitly deferred entries. After the
migration, `--progress-through 6` must fail as stale because Task 7 no longer
contains an allowed violation. Method, route, ownership, auth, audit,
multipart, body-builder, transition-helper, composite, and opaque-payload
truth are never relaxed by progress mode.

## Required RED tests

Extend `dcc_rest_v2_endpoint_smoke` with a dedicated Task 7 support unit and
register any audit fixtures before editing production implementation. Capture
RED for at least:

- all 35 exact canonical declarations/signatures and absence of all 18 legacy
  REST names;
- literal GET-without-body, typed PATCH, archive pagination, permission
  overwrite, invite query, and every route/query encoding in the tables;
- the complete 2026 channel/thread/invite field matrix, presence/null behavior,
  renamed/missing fields such as bitrate-in-bits, position `flags`, invite
  `target_application_id`/`role_ids`, and thread-member `with_member`;
- invalid range, relationship, channel-kind, target-type, tag/emoji, nullable,
  array-stride, unknown-bit, uncovered-field, and overflow cases rejected with
  null output and zero allocation/admission/callback/observer counters;
- a packed, aligned call-options historical prefix ending at `user_data`, with
  poison after it, plus every partial-tail boundary, larger layout, and nested
  raw-descriptor coverage mismatch;
- every auth mode against every exceptional endpoint policy, empty/uncovered
  Bearer tokens, copied-and-wiped Bearer lifetime, and absence of secrets in
  diagnostics;
- all 12 audit-capable endpoints accepting a Unicode reason and all other Task
  7 endpoints rejecting it; empty, invalid UTF-8, 512/513-scalar boundaries,
  percent characters, non-ASCII, CR/LF, and exactly-once RFC 3986 encoding;
- relative raw Bot/None/Bearer behavior and absolute URLs receiving no audit
  header or credentials and rejecting explicit Discord auth/audit options;
- JSON and multipart branches for the exact three multipart endpoints,
  multiple forum files, optional/required CSV behavior, exact embedded-NUL
  bytes, metadata injection, exact preflight overflow, delayed ownership, and
  cancellation/handle destruction;
- transition App adapters and high-level thread helpers reaching the canonical
  request path once without waiting or bypassing error observation;
- progress audit count 224, Task 7 count 35, remaining count 148, stale
  progress rejection, and all new malformed-manifest self-tests.

Use the local HTTP server/interceptor only; make no live Discord calls. Compare
literal routes, queries, bodies, multipart fields, and headers. Hold the single
REST worker behind a blocker request before mutating/freeing every borrowed
input so a synchronous-copy bug cannot pass accidentally.

## RED to GREEN sequence, verification, and handoff

Start from a clean Task 6-reviewed base and record the current
`--progress-through 6` baseline. Then follow this order exactly:

1. Change only tests, manifest/audit contract fixtures, and build registration.
   Capture the expected compile/audit/runtime failures against old Task 7 code.
   Commit that buildable RED contract as
   `test: define channel REST migration contract`.
2. Add the call-options audit/auth tail and shared request preparation first,
   keeping the historical prefix and all Task 6 endpoint tests green. Then
   implement typed Task 7 records, serializers, endpoint submissions, manifest
   capabilities, and App/in-tree migration. Make the focused endpoint and audit
   targets green before broad verification.
3. Run the complete verification matrix below from the implementation tree.
   Review the RED-to-GREEN diff, run `git diff --check`, and commit production,
   headers, manifest, audit, migrations, and tests with the exact message
   `feat: migrate channel REST endpoints to requests`.
4. Write `task-7-implementer-report.md` beside this brief with the RED evidence,
   active/task/domain/capability counts, 35 canonical endpoints, 18 removals,
   field and ownership matrix, App migrations, exact commands/results,
   sanitizer/platform evidence, known baseline debt, and implementation hash.
   Commit the report separately as
   `docs: record channel REST endpoint migration`, then stop for review. Do not
   push.

Minimum GREEN verification:

- `python3 tools/audit_rest_v2_endpoints.py --self-test` and
  `python3 tools/audit_rest_v2_endpoints.py --progress-through 7`;
- `dcc_rest_v2_endpoint_smoke`, `dcc_rest_v2_request_smoke`, the complete
  `dcc_http_smoke`, `dcc_official_surface_smoke`, and official header smoke;
- App, sugar action, interaction flow/server, managed-message, command-registry,
  package-consumer, v2/transition surface, source-package, and release audits;
- standalone strict C11 and C++17 compilation of `dcc/rest/request.h`, every
  changed channel/thread/invite leaf and aggregate header,
  `dcc/rest/official_surface.h`, `dcc/rest.h`, and `dcc/app/legacy.h`;
- i686 and x86_64 MinGW header/include-order probes plus every changed
  production translation unit;
- focused ASan/UBSan and TSan runs for historical prefixes, delayed owned
  input, callback/handle coexistence, multipart cancellation, and auth-token
  lifetime, each repeated enough to expose races;
- the full enabled CTest suite, strict MkDocs build, install/package consumers,
  and final `git diff --check`.

Do not weaken a failing test, hide a later-task violation in generic-operation
allowlists, or combine the RED, GREEN, and report commits. Report pre-existing
platform/tool unavailability separately from regressions and include the exact
command and output evidence.
