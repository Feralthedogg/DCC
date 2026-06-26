# Official Discord API Surface

DCC keeps the low-level `dcc_rest_request()` escape hatch, but the public
surface now also includes typed wrappers for Discord endpoints that are outside
the older DPP parity layer.

For application code, include `<dcc/sugar.h>` first. Explicit API consumers can
include `<dcc/rest/official_surface.h>`, `<dcc/oauth2.h>`, and
`<dcc/webhook_events.h>` directly.

## Design Contract

The coverage model has four layers:

1. Route parity: every official Discord route that DCC does not already expose
   gets a named `dcc_rest_*` wrapper.
2. Authorization split: bot-token routes stay on `dcc_client_t`; OAuth2
   authorization-code, refresh, revoke, Bearer `GET /oauth2/@me`, and the
   OAuth2-documented Bot `GET /oauth2/applications/@me` use standalone
   `dcc_oauth2_*` helpers so bot REST clients do not accidentally mix
   authorization schemes.
3. Gateway send parity: public helpers wrap send opcodes that were previously
   only reachable through internal websocket code.
4. Sugar-first API: `<dcc/sugar.h>` re-exports concise aliases for application
   code, while the `dcc_*` functions remain the stable ABI surface.

Raw wrappers remain available for compatibility. For request bodies with a
stable official shape, DCC also provides typed params builders so application
code can stay sugar-first without hand-writing JSON.

## REST Surface

The official-surface wrappers follow the normal DCC REST callback model:

```c
dcc_rest_get_gateway(client, on_response, user_data);
dcc_rest_get_default_soundboard_sounds(client, on_response, user_data);
dcc_rest_get_lobby(client, lobby_id, on_response, user_data);
```

Added REST coverage:

| Area | Wrappers |
| --- | --- |
| Gateway/Application | `dcc_rest_get_gateway`, `dcc_rest_get_current_application`, `dcc_rest_modify_current_application`, `dcc_rest_modify_current_application_params`, `dcc_rest_get_activity_instance` |
| Soundboard | `dcc_rest_send_soundboard_sound`, `dcc_rest_get_default_soundboard_sounds`, guild soundboard CRUD |
| Subscriptions | `dcc_rest_get_sku_subscriptions`, `dcc_rest_get_sku_subscription` |
| Guild extras | `dcc_rest_bulk_ban_guild_members`, `dcc_rest_get_guild_role`, `dcc_rest_get_guild_role_member_counts`, widget JSON/PNG, incident actions |
| Invite target users | `dcc_rest_get_invite_target_users`, `dcc_rest_put_invite_target_users`, job status |
| Entitlements/Stickers/Users | single entitlement fetch, sticker pack fetch, current user's guild member fetch, role-connection delete |
| Legacy pins | `dcc_rest_legacy_pin_message`, `dcc_rest_legacy_unpin_message`, `dcc_rest_get_legacy_channel_pins` for `/channels/{channel.id}/pins...` |
| Webhook compatibility | Slack and GitHub compatibility execute endpoints |
| Lobby/Social SDK | lobby create/get/modify/delete, member updates, channel linking, messages, invites, moderation metadata |

Wrappers that accept `json_body` intentionally keep the same style as the rest
of DCC's REST API. They validate and format the route, then submit the JSON
payload unchanged.

Small, high-use request bodies also have typed helpers so application code does
not need to hand-write common JSON:

```c
const char *scopes[] = {"bot", "applications.commands"};
dcc_application_install_params_t install =
    DCC_APPLICATION_INSTALL_PARAMS(scopes, 2, "2048");
dcc_application_modify_params_t application = DCC_APPLICATION_MODIFY(
    .description = "DCC",
    .has_description = 1,
    .install_params = &install,
    .has_install_params = 1,
    .event_webhooks_status = DCC_APPLICATION_EVENT_WEBHOOK_ENABLED,
    .has_event_webhooks_status = 1
);
DCC_REST_MODIFY_CURRENT_APPLICATION_PARAMS(client, &application, on_response, NULL);

dcc_soundboard_send_params_t sound = DCC_SOUNDBOARD_SEND(
    .sound_id = sound_id,
    .source_guild_id = source_guild_id
);
DCC_REST_SEND_SOUNDBOARD_PARAMS(client, channel_id, &sound, on_response, NULL);

dcc_guild_soundboard_sound_params_t guild_sound = DCC_GUILD_SOUNDBOARD_SOUND(
    .name = "bell",
    .sound = "data:audio/ogg;base64,...",
    .volume = 0.5,
    .has_volume = 1,
    .emoji_name = "ding",
    .has_emoji_name = 1
);
DCC_REST_CREATE_GUILD_SOUNDBOARD_SOUND_PARAMS(
    client,
    guild_id,
    &guild_sound,
    on_response,
    NULL
);

dcc_bulk_ban_params_t ban = DCC_BULK_BAN_BODY(
    .user_ids = users,
    .user_id_count = user_count,
    .delete_message_seconds = 3600,
    .has_delete_message_seconds = 1
);
DCC_REST_BULK_BAN_PARAMS(client, guild_id, &ban, on_response, NULL);

dcc_lobby_member_spec_t initial_members[] = {
    DCC_LOBBY_MEMBER_SPEC(
        .user_id = user_id,
        .metadata_json = "{\"role\":\"tank\"}",
        .has_metadata = 1,
        .flags = 1,
        .has_flags = 1
    )
};
dcc_lobby_params_t create_lobby = DCC_LOBBY(
    .metadata_json = "{\"topic\":\"redstone\"}",
    .has_metadata = 1,
    .members = initial_members,
    .member_count = 1,
    .has_members = 1,
    .idle_timeout_seconds = 60,
    .has_idle_timeout_seconds = 1
);
DCC_REST_CREATE_LOBBY_PARAMS(client, &create_lobby, on_response, NULL);

dcc_lobby_create_or_join_params_t lobby = DCC_LOBBY_CREATE_OR_JOIN(
    .secret = "join-secret",
    .idle_timeout_seconds = 60,
    .has_idle_timeout_seconds = 1,
    .lobby_metadata_json = "{\"topic\":\"redstone\"}",
    .has_lobby_metadata = 1
);
DCC_REST_CREATE_OR_JOIN_LOBBY_PARAMS(client, &lobby, on_response, NULL);

dcc_lobby_channel_link_params_t link = DCC_LOBBY_LINK_CHANNEL(
    .channel_id = channel_id
);
DCC_REST_LINK_LOBBY_CHANNEL_PARAMS(client, lobby_id, &link, on_response, NULL);

dcc_lobby_member_params_t member = DCC_LOBBY_MEMBER(
    .metadata_json = "{\"role\":\"tank\"}",
    .has_metadata = 1,
    .flags = 1,
    .has_flags = 1
);
DCC_REST_ADD_LOBBY_MEMBER_PARAMS(client, lobby_id, user_id, &member, on_response, NULL);

dcc_lobby_member_bulk_item_t member_updates[] = {
    DCC_LOBBY_BULK_MEMBER(
        .user_id = user_id,
        .metadata_json = "{\"role\":\"tank\"}",
        .has_metadata = 1
    ),
    DCC_LOBBY_BULK_MEMBER(
        .user_id = old_user_id,
        .remove_member = 1,
        .has_remove_member = 1
    )
};
dcc_lobby_member_bulk_params_t bulk =
    DCC_LOBBY_BULK_MEMBERS(member_updates, 2);
DCC_REST_BULK_UPDATE_LOBBY_MEMBERS_PARAMS(client, lobby_id, &bulk, on_response, NULL);

dcc_lobby_message_params_t lobby_message = DCC_LOBBY_MESSAGE(
    .content = "hello",
    .metadata_json = "{\"kind\":\"chat\"}",
    .has_metadata = 1
);
DCC_REST_SEND_LOBBY_MESSAGE_PARAMS(client, lobby_id, &lobby_message, on_response, NULL);

dcc_lobby_message_moderation_metadata_item_t moderation_items[] = {
    DCC_LOBBY_MODERATION_METADATA_ITEM("decision", "allow"),
    DCC_LOBBY_MODERATION_METADATA_ITEM("source", "automod")
};
dcc_lobby_message_moderation_metadata_params_t moderation =
    DCC_LOBBY_MODERATION_METADATA(moderation_items, 2);
DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION_PARAMS(
    client,
    lobby_id,
    message_id,
    &moderation,
    on_response,
    NULL
);
```

The body builders return caller-owned JSON:

```c
char *json = NULL;
dcc_rest_build_guild_incident_actions_body(&actions, &json);
dcc_rest_official_body_json_free(json);
```

Guild soundboard sound builders follow Discord's field semantics. Create
requires `name` and `sound`; modify requires at least one explicitly supplied
field. Nullable fields such as `emoji_id` and `emoji_name` are emitted only
when their `has_*` flag is set:

```c
dcc_guild_soundboard_sound_params_t patch = DCC_GUILD_SOUNDBOARD_SOUND(
    .name = "bell2",
    .emoji_id = 0,
    .has_emoji_id = 1
);
dcc_rest_build_guild_soundboard_sound_modify_body(&patch, &json);
dcc_rest_official_body_json_free(json);
```

Lobby builders cover compact Social SDK request bodies while keeping metadata
open-ended. Metadata fields accept caller-owned JSON fragments, or `NULL` with
the corresponding `has_*` flag when Discord expects an explicit `null`:

```c
dcc_lobby_message_params_t message = DCC_LOBBY_MESSAGE(
    .content = "moderation pending",
    .metadata_json = NULL,
    .has_metadata = 1,
    .flags = 4,
    .has_flags = 1
);
dcc_rest_build_lobby_message_body(&message, &json);
dcc_rest_official_body_json_free(json);
```

`DCC_LOBBY` is shared by Create Lobby and Modify Lobby because both official
routes use the same fields. Set `has_members` with `member_count = 0` to emit an
explicit empty `members` array for modify operations that should clear lobby
membership:

```c
dcc_lobby_params_t patch_lobby = DCC_LOBBY(
    .metadata_json = NULL,
    .has_metadata = 1,
    .members = NULL,
    .member_count = 0,
    .has_members = 1
);
DCC_REST_MODIFY_LOBBY_PARAMS(client, lobby_id, &patch_lobby, on_response, NULL);
```

Bulk lobby member updates use Discord's array request body, not a wrapper
object:

```c
dcc_lobby_member_bulk_item_t updates[] = {
    DCC_LOBBY_BULK_MEMBER(.user_id = user_id, .flags = 1, .has_flags = 1)
};
dcc_lobby_member_bulk_params_t body = DCC_LOBBY_BULK_MEMBERS(updates, 1);
dcc_rest_build_lobby_member_bulk_body(&body, &json);
dcc_rest_official_body_json_free(json);
```

Lobby moderation metadata is a free-form string object in Discord's API. DCC's
typed helper builds that object from key/value pairs and enforces the documented
five-key limit:

```c
dcc_lobby_message_moderation_metadata_item_t moderation_items[] = {
    DCC_LOBBY_MODERATION_METADATA_ITEM("decision", "allow")
};
dcc_lobby_message_moderation_metadata_params_t moderation =
    DCC_LOBBY_MODERATION_METADATA(moderation_items, 1);
dcc_rest_build_lobby_message_moderation_metadata_body(&moderation, &json);
dcc_rest_official_body_json_free(json);
```

Every DCC-owned raw body wrapper in this official surface has a typed params
companion. The exceptions are `dcc_rest_execute_webhook_slack` and
`dcc_rest_execute_webhook_github`, because those compatibility endpoints accept
external Slack/GitHub webhook payload shapes rather than a Discord-owned request
object.

## Public Object Additions

The official surface also adds lightweight public object declarations for the
new response families that did not previously have a DCC object type. These are
borrowed-view structs, matching the existing `dcc_channel_t` and
`dcc_subscription_t` style: strings, arrays, and raw metadata JSON are owned by
the caller or parser layer that fills the object.

Activity instance responses from `dcc_rest_get_activity_instance` can be modeled
with `dcc_activity_instance_t`. Its location object is `dcc_activity_location_t`;
the location kind is represented by `dcc_activity_location_kind_t` with
`DCC_ACTIVITY_LOCATION_GUILD_CHANNEL` for Discord's `gc` kind and
`DCC_ACTIVITY_LOCATION_PRIVATE_CHANNEL` for Discord's `pc` kind.

Lobby responses from `dcc_rest_create_lobby`, `dcc_rest_get_lobby`, and related
Social SDK routes can be modeled with `dcc_lobby_t` and
`dcc_lobby_member_t`. Discord's `CanLinkLobby` member flag is exposed as
`DCC_LOBBY_MEMBER_CAN_LINK_LOBBY` through `dcc_lobby_member_flag_t`.

```c
dcc_lobby_member_t members[] = {
    {
        .id = user_id,
        .flags = DCC_LOBBY_MEMBER_CAN_LINK_LOBBY,
        .has_flags = 1,
        .metadata_json = NULL
    }
};
dcc_lobby_t lobby = {
    .id = lobby_id,
    .application_id = application_id,
    .members = members,
    .members_count = 1,
    .metadata_json = "{\"topic\":\"redstone\"}"
};
```

The full docs audit also verifies older DCC wrappers that already covered
official routes. For example, `/gateway/bot` remains available as
`dcc_rest_get_gateway_bot`, while the newly exposed `/gateway` route is
`dcc_rest_get_gateway`.

Official-surface addition wrapper list:

```text
dcc_rest_get_gateway
dcc_rest_get_current_application
dcc_rest_modify_current_application
dcc_rest_build_application_modify_body
dcc_rest_modify_current_application_params
dcc_rest_get_activity_instance
dcc_rest_build_soundboard_send_body
dcc_rest_send_soundboard_sound
dcc_rest_send_soundboard_sound_params
dcc_rest_get_default_soundboard_sounds
dcc_rest_get_guild_soundboard_sounds
dcc_rest_get_guild_soundboard_sound
dcc_rest_create_guild_soundboard_sound
dcc_rest_build_guild_soundboard_sound_create_body
dcc_rest_create_guild_soundboard_sound_params
dcc_rest_modify_guild_soundboard_sound
dcc_rest_build_guild_soundboard_sound_modify_body
dcc_rest_modify_guild_soundboard_sound_params
dcc_rest_delete_guild_soundboard_sound
dcc_rest_get_sku_subscriptions
dcc_rest_get_sku_subscription
dcc_rest_build_bulk_ban_body
dcc_rest_bulk_ban_guild_members
dcc_rest_bulk_ban_guild_members_params
dcc_rest_get_guild_role
dcc_rest_get_guild_role_member_counts
dcc_rest_get_guild_widget_json
dcc_rest_get_guild_widget_png
dcc_rest_build_guild_incident_actions_body
dcc_rest_modify_guild_incident_actions
dcc_rest_modify_guild_incident_actions_params
dcc_rest_get_invite_target_users
dcc_rest_put_invite_target_users
dcc_rest_get_invite_target_users_job_status
dcc_rest_get_entitlement
dcc_rest_get_sticker_pack
dcc_rest_get_current_user_guild_member
dcc_rest_delete_current_user_application_role_connection
dcc_rest_execute_webhook_slack
dcc_rest_execute_webhook_github
dcc_rest_legacy_pin_message
dcc_rest_legacy_unpin_message
dcc_rest_get_legacy_channel_pins
dcc_rest_create_lobby
dcc_rest_build_lobby_body
dcc_rest_create_lobby_params
dcc_rest_create_or_join_lobby
dcc_rest_build_lobby_create_or_join_body
dcc_rest_create_or_join_lobby_params
dcc_rest_get_lobby
dcc_rest_modify_lobby
dcc_rest_modify_lobby_params
dcc_rest_delete_lobby
dcc_rest_add_lobby_member
dcc_rest_build_lobby_member_body
dcc_rest_add_lobby_member_params
dcc_rest_bulk_update_lobby_members
dcc_rest_build_lobby_member_bulk_body
dcc_rest_bulk_update_lobby_members_params
dcc_rest_remove_lobby_member
dcc_rest_leave_lobby
dcc_rest_link_lobby_channel
dcc_rest_build_lobby_channel_link_body
dcc_rest_link_lobby_channel_params
dcc_rest_unlink_lobby_channel
dcc_rest_send_lobby_message
dcc_rest_build_lobby_message_body
dcc_rest_send_lobby_message_params
dcc_rest_get_lobby_messages
dcc_rest_update_lobby_message_moderation_metadata
dcc_rest_build_lobby_message_moderation_metadata_body
dcc_rest_update_lobby_message_moderation_metadata_params
dcc_rest_create_lobby_channel_invite_for_self
dcc_rest_create_lobby_channel_invite_for_user
dcc_rest_official_body_json_free
```

Sugar aliases are available for the same route families, including
`DCC_REST_GATEWAY`, `DCC_REST_GATEWAY_BOT`, `DCC_APPLICATION_INSTALL_PARAMS`,
`DCC_APPLICATION_INTEGRATION_CONFIG`, `DCC_APPLICATION_MODIFY`,
`DCC_REST_MODIFY_CURRENT_APPLICATION_PARAMS`, `DCC_SOUNDBOARD_SEND`,
`DCC_GUILD_SOUNDBOARD_SOUND`, `DCC_REST_SEND_SOUNDBOARD_PARAMS`,
`DCC_REST_CREATE_GUILD_SOUNDBOARD_SOUND_PARAMS`,
`DCC_REST_MODIFY_GUILD_SOUNDBOARD_SOUND_PARAMS`, `DCC_REST_BULK_BAN`,
`DCC_BULK_BAN_BODY`, `DCC_INCIDENT_ACTIONS_BODY`, `DCC_REST_GUILD_ROLE`,
`DCC_REST_CREATE_LOBBY`, `DCC_LOBBY_MEMBER_SPEC`, `DCC_LOBBY`,
`DCC_REST_CREATE_LOBBY_PARAMS`, `DCC_LOBBY_CREATE_OR_JOIN`,
`DCC_REST_CREATE_OR_JOIN_LOBBY`, `DCC_REST_CREATE_OR_JOIN_LOBBY_PARAMS`,
`DCC_REST_MODIFY_LOBBY_PARAMS`, `DCC_LOBBY_LINK_CHANNEL`,
`DCC_REST_LINK_LOBBY_CHANNEL_PARAMS`,
`DCC_LOBBY_MEMBER`, `DCC_REST_ADD_LOBBY_MEMBER_PARAMS`,
`DCC_LOBBY_BULK_MEMBER`, `DCC_LOBBY_BULK_MEMBERS`,
`DCC_REST_BULK_UPDATE_LOBBY_MEMBERS_PARAMS`, `DCC_LOBBY_MESSAGE`,
`DCC_REST_SEND_LOBBY_MESSAGE_PARAMS`,
`DCC_LOBBY_MODERATION_METADATA_ITEM`, `DCC_LOBBY_MODERATION_METADATA`,
`DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION_PARAMS`, `DCC_REST_LOBBY`, and
`DCC_REST_SEND_LOBBY_MESSAGE`.

The modern message pin wrappers use `/channels/{channel.id}/messages/pins`.
Discord still documents the legacy `/channels/{channel.id}/pins` routes, so DCC
also exposes `dcc_rest_legacy_pin_message`,
`dcc_rest_legacy_unpin_message`, and `dcc_rest_get_legacy_channel_pins`.
Sugar aliases for those legacy routes are `DCC_REST_LEGACY_PIN_MESSAGE`,
`DCC_REST_LEGACY_UNPIN_MESSAGE`, and `DCC_REST_LEGACY_CHANNEL_PINS`.

## OAuth2

OAuth2 is not routed through `dcc_client_t`, because bot REST clients always use
Bot authorization. Use the standalone helpers instead:

```c
dcc_oauth2_exchange_code_options_t opts = DCC_OAUTH2_EXCHANGE_CODE_OPTIONS(
    .client_id = client_id,
    .client_secret = client_secret,
    .code = code,
    .redirect_uri = redirect_uri
);

dcc_oauth2_exchange_code(&opts, on_oauth2_response, NULL);
```

Available helpers:

| Helper | Discord endpoint |
| --- | --- |
| `dcc_oauth2_get_current_authorization` | `GET /oauth2/@me` with Bearer authorization |
| `dcc_oauth2_get_current_bot_application` | `GET /oauth2/applications/@me` with Bot authorization |
| `dcc_oauth2_exchange_code` | `POST /oauth2/token` authorization-code flow |
| `dcc_oauth2_refresh_token` | `POST /oauth2/token` refresh-token flow |
| `dcc_oauth2_revoke_token` | `POST /oauth2/token/revoke` |

Sugar aliases are available as `DCC_OAUTH2_ME`,
`DCC_OAUTH2_CURRENT_BOT_APPLICATION`, `DCC_OAUTH2_EXCHANGE_CODE`,
`DCC_OAUTH2_REFRESH_TOKEN`, and `DCC_OAUTH2_REVOKE_TOKEN`.

## Gateway Send Events

DCC exposes public helpers for Gateway send events that were previously only
available through raw socket-level code:

```c
dcc_gateway_request_guild_members_options_t members = DCC_GATEWAY_MEMBERS(
    .guild_id = guild_id,
    .query = "",
    .limit = 0,
    .presences = 1
);
dcc_client_request_guild_members(client, &members);

const dcc_snowflake_t guilds[] = {guild_id};
dcc_client_request_soundboard_sounds(client, guilds, 1);

const char *fields[] = {"status", "voice_start_time"};
dcc_client_request_channel_info(client, guild_id, fields, 2);
```

Presence updates can be structured or raw:

```c
dcc_gateway_presence_update_t presence = DCC_GATEWAY_PRESENCE(
    .status = "online",
    .activities_json = "[]"
);
dcc_client_update_presence(client, &presence);
```

The public Gateway send helpers are:

```text
dcc_client_request_guild_members
dcc_client_request_soundboard_sounds
dcc_client_request_channel_info
dcc_client_update_voice_state
dcc_client_update_presence
dcc_client_update_presence_raw
```

Identify, Resume, and Heartbeat are runtime-managed internal sends. Application
code normally uses `dcc_client_start()`, `dcc_client_gateway_reconnect()`, and
the gateway runtime rather than sending those opcodes manually.

Official Gateway send event coverage:

```text
Identify
Resume
Heartbeat
Request Guild Members
Request Soundboard Sounds
Request Channel Info
Update Voice State
Update Presence
```

## Webhook Events

Discord outgoing Webhook Events can be attached to the existing interaction
server. Signature verification is handled by the interaction server route unless
the route is explicitly marked unsigned.

```c
static dcc_status_t on_event(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    const dcc_webhook_event_t *event,
    void *user_data
) {
    (void)server;
    (void)request;
    (void)user_data;
    if (event->type == DCC_WEBHOOK_EVENT_APPLICATION_AUTHORIZED) {
        /* event->data_json points at the inner event data object. */
    }
    return DCC_OK;
}

dcc_webhook_event_route_t route = DCC_WEBHOOK_EVENT_ROUTE(on_event, NULL);
dcc_webhook_events_add_route(server, "/discord/events", &route);
```

## Surface Audit

Run the DCC addition audit and the official Discord docs route audit before
cutting a release:

```bash
python3 tools/audit_official_surface.py
DCC_DISCORD_API_DOCS_ROOT=/path/to/discord-api-docs \
  python3 tools/audit_discord_api_docs_surface.py
DCC_DISCORD_API_DOCS_ROOT=/path/to/discord-api-docs \
  python3 tools/audit_official_events_surface.py
```

The docs route audit reads official `<Route>` tags from `discord-api-docs` and
checks that every documented route path has a DCC source path template. It also
tracks OAuth2 token and revoke flow endpoints, which the docs describe outside
`<Route>` blocks.

The audit reports both documented route entries and canonical route bindings.
Discord sometimes documents the same route shape under two semantic names, such
as webhook messages and interaction followup messages:

```text
/webhooks/{webhook.id}/{webhook.token}/messages/{message.id}
/webhooks/{application.id}/{interaction.token}/messages/{message.id}
```

DCC canonicalizes path parameter names to `{}` for coverage checks, so these are
one implementation binding with two official labels. A passing audit means every
official documented route entry is covered after this canonicalization. For the
current audited `discord-api-docs` main checkout this is:

```text
231 official route entries
231 documented method/path routes
227 canonical method/path routes
227 DCC public route bindings
```

The events audit reads the official Gateway Send Events and Webhook Events
tables and verifies that DCC has a public or runtime implementation for every
documented send event and a parsed enum/string-map entry for every outgoing
Webhook Event value.

PING payloads are acknowledged automatically with HTTP 204. Event payloads are
parsed into `dcc_webhook_event_t` and acknowledged with HTTP 204 after the
callback returns `DCC_OK`. The parser preserves the raw request body and fills
the event type, type name, numeric `application_id`, and borrowed
`timestamp`/`timestamp_len` span when present.

Webhook Events have an outer envelope and an inner `event` object. DCC exposes
both the full envelope and the useful inner spans without copying:
`raw_body`/`raw_body_len` point at the full request body,
`event_json`/`event_json_len` point at the inner `event` object, and
`data_json`/`data_json_len` point at the event-specific `data` object when
Discord includes one.

The parser treats the outer Webhook Event envelope as a JSON object with
top-level fields. Top-level `type`, `application_id`, and `event` are not
matched inside nested event data, so payloads that contain event-specific
objects with their own `type` or `application_id` fields cannot be mistaken for
a PING or routed as the wrong application.

Supported event names include application authorization/deauthorization,
entitlement create/update/delete, quest enrollment, lobby message events, and
game direct-message events.

Official outgoing Webhook Event values covered by `dcc_webhook_event_t`:

```text
APPLICATION_AUTHORIZED
APPLICATION_DEAUTHORIZED
ENTITLEMENT_CREATE
ENTITLEMENT_UPDATE
ENTITLEMENT_DELETE
QUEST_USER_ENROLLMENT
LOBBY_MESSAGE_CREATE
LOBBY_MESSAGE_UPDATE
LOBBY_MESSAGE_DELETE
GAME_DIRECT_MESSAGE_CREATE
GAME_DIRECT_MESSAGE_UPDATE
GAME_DIRECT_MESSAGE_DELETE
```

Webhook helper functions:

```text
dcc_webhook_event_type_name
dcc_webhook_event_type_from_string
dcc_webhook_event_parse
dcc_webhook_events_add_route
```

## Sugar Summary

`<dcc/sugar.h>` includes `dcc/sugar/official_surface.h`, which provides:

```text
DCC_GATEWAY_MEMBERS
DCC_GATEWAY_PRESENCE
DCC_OAUTH2_EXCHANGE_CODE_OPTIONS
DCC_OAUTH2_REFRESH_TOKEN_OPTIONS
DCC_OAUTH2_REVOKE_TOKEN_OPTIONS
DCC_APPLICATION_INSTALL_PARAMS
DCC_APPLICATION_INTEGRATION_CONFIG
DCC_APPLICATION_MODIFY
DCC_SOUNDBOARD_SEND
DCC_GUILD_SOUNDBOARD_SOUND
DCC_BULK_BAN_BODY
DCC_INCIDENT_ACTIONS_BODY
DCC_LOBBY_CREATE_OR_JOIN
DCC_LOBBY_MEMBER_SPEC
DCC_LOBBY
DCC_LOBBY_LINK_CHANNEL
DCC_LOBBY_MEMBER
DCC_LOBBY_BULK_MEMBER
DCC_LOBBY_BULK_MEMBERS
DCC_LOBBY_MESSAGE
DCC_LOBBY_MODERATION_METADATA_ITEM
DCC_LOBBY_MODERATION_METADATA
DCC_WEBHOOK_EVENT_ROUTE
DCC_WEBHOOK_EVENT_ROUTE_UNSIGNED
DCC_REST_GATEWAY
DCC_REST_GATEWAY_BOT
DCC_REST_CURRENT_APPLICATION
DCC_REST_MODIFY_CURRENT_APPLICATION
DCC_REST_MODIFY_CURRENT_APPLICATION_PARAMS
DCC_REST_ACTIVITY_INSTANCE
DCC_REST_SEND_SOUNDBOARD
DCC_REST_SEND_SOUNDBOARD_PARAMS
DCC_REST_DEFAULT_SOUNDBOARD_SOUNDS
DCC_REST_GUILD_SOUNDBOARD_SOUNDS
DCC_REST_GUILD_SOUNDBOARD_SOUND
DCC_REST_CREATE_GUILD_SOUNDBOARD_SOUND
DCC_REST_CREATE_GUILD_SOUNDBOARD_SOUND_PARAMS
DCC_REST_MODIFY_GUILD_SOUNDBOARD_SOUND
DCC_REST_MODIFY_GUILD_SOUNDBOARD_SOUND_PARAMS
DCC_REST_DELETE_GUILD_SOUNDBOARD_SOUND
DCC_REST_SKU_SUBSCRIPTIONS
DCC_REST_SKU_SUBSCRIPTION
DCC_REST_BULK_BAN
DCC_REST_BULK_BAN_PARAMS
DCC_REST_GUILD_ROLE
DCC_REST_ROLE_MEMBER_COUNTS
DCC_REST_WIDGET_JSON
DCC_REST_WIDGET_PNG
DCC_REST_INCIDENT_ACTIONS
DCC_REST_INCIDENT_ACTIONS_PARAMS
DCC_REST_INVITE_TARGET_USERS
DCC_REST_PUT_INVITE_TARGET_USERS
DCC_REST_INVITE_TARGET_USERS_JOB
DCC_REST_ENTITLEMENT
DCC_REST_STICKER_PACK
DCC_REST_CURRENT_USER_GUILD_MEMBER
DCC_REST_DELETE_USER_ROLE_CONNECTION
DCC_REST_WEBHOOK_SLACK
DCC_REST_WEBHOOK_GITHUB
DCC_REST_LEGACY_PIN_MESSAGE
DCC_REST_LEGACY_UNPIN_MESSAGE
DCC_REST_LEGACY_CHANNEL_PINS
DCC_REST_CREATE_LOBBY
DCC_REST_CREATE_LOBBY_PARAMS
DCC_REST_CREATE_OR_JOIN_LOBBY
DCC_REST_CREATE_OR_JOIN_LOBBY_PARAMS
DCC_REST_LOBBY
DCC_REST_MODIFY_LOBBY
DCC_REST_MODIFY_LOBBY_PARAMS
DCC_REST_DELETE_LOBBY
DCC_REST_ADD_LOBBY_MEMBER
DCC_REST_ADD_LOBBY_MEMBER_PARAMS
DCC_REST_BULK_UPDATE_LOBBY_MEMBERS
DCC_REST_BULK_UPDATE_LOBBY_MEMBERS_PARAMS
DCC_REST_REMOVE_LOBBY_MEMBER
DCC_REST_LEAVE_LOBBY
DCC_REST_LINK_LOBBY_CHANNEL
DCC_REST_LINK_LOBBY_CHANNEL_PARAMS
DCC_REST_UNLINK_LOBBY_CHANNEL
DCC_REST_SEND_LOBBY_MESSAGE
DCC_REST_SEND_LOBBY_MESSAGE_PARAMS
DCC_REST_LOBBY_MESSAGES
DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION
DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION_PARAMS
DCC_REST_CREATE_LOBBY_INVITE_SELF
DCC_REST_CREATE_LOBBY_INVITE_USER
DCC_GATEWAY_REQUEST_MEMBERS
DCC_GATEWAY_REQUEST_SOUNDBOARD
DCC_GATEWAY_REQUEST_CHANNEL_INFO
DCC_GATEWAY_UPDATE_PRESENCE
DCC_GATEWAY_UPDATE_PRESENCE_RAW
DCC_OAUTH2_ME
DCC_OAUTH2_CURRENT_BOT_APPLICATION
DCC_OAUTH2_EXCHANGE_CODE
DCC_OAUTH2_REFRESH_TOKEN
DCC_OAUTH2_REVOKE_TOKEN
DCC_WEBHOOK_EVENTS_ADD_ROUTE
```

Use the explicit `dcc_*` names in library code where ABI clarity matters, and
the `DCC_*` sugar macros in application code where concise declarations are
more readable.
