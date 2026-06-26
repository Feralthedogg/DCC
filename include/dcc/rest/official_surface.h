/**
 * @file include/dcc/rest/official_surface.h
 * @brief Public wrappers for Discord API endpoints not covered by the legacy DPP parity layer.
 */

#ifndef DCC_REST_OFFICIAL_SURFACE_H
#define DCC_REST_OFFICIAL_SURFACE_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `GET /gateway`.
 *
 * This endpoint returns the base Gateway URL without the bot session-start
 * limit metadata returned by dcc_rest_get_gateway_bot().
 */
DCC_API dcc_status_t dcc_rest_get_gateway(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /applications/@me`.
 */
DCC_API dcc_status_t dcc_rest_get_current_application(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
);

typedef enum dcc_application_integration_type {
    DCC_APPLICATION_INTEGRATION_TYPE_GUILD_INSTALL = 0,
    DCC_APPLICATION_INTEGRATION_TYPE_USER_INSTALL = 1
} dcc_application_integration_type_t;

typedef enum dcc_application_event_webhook_status {
    DCC_APPLICATION_EVENT_WEBHOOK_DISABLED = 1,
    DCC_APPLICATION_EVENT_WEBHOOK_ENABLED = 2
} dcc_application_event_webhook_status_t;

typedef struct dcc_application_install_params {
    size_t size;
    const char *const *scopes;
    size_t scope_count;
    const char *permissions;
    uint8_t has_permissions;
} dcc_application_install_params_t;

typedef struct dcc_application_integration_type_config {
    size_t size;
    uint32_t integration_type;
    const dcc_application_install_params_t *oauth2_install_params;
    uint8_t has_oauth2_install_params;
} dcc_application_integration_type_config_t;

typedef struct dcc_application_modify_params {
    size_t size;
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
    uint32_t event_webhooks_status;
    const char *const *event_webhooks_types;
    size_t event_webhooks_type_count;
    uint8_t has_custom_install_url;
    uint8_t has_description;
    uint8_t has_role_connections_verification_url;
    uint8_t has_install_params;
    uint8_t has_integration_types_config;
    uint8_t has_flags;
    uint8_t has_icon;
    uint8_t has_cover_image;
    uint8_t has_interactions_endpoint_url;
    uint8_t has_tags;
    uint8_t has_event_webhooks_url;
    uint8_t has_event_webhooks_status;
    uint8_t has_event_webhooks_types;
} dcc_application_modify_params_t;

/**
 * @brief Submits `PATCH /applications/@me`.
 *
 * @param json_body JSON application edit payload.
 */
DCC_API dcc_status_t dcc_rest_modify_current_application(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_application_modify_body(
    const dcc_application_modify_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_modify_current_application_params(
    dcc_client_t *client,
    const dcc_application_modify_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /applications/{application.id}/activity-instances/{instance_id}`.
 */
DCC_API dcc_status_t dcc_rest_get_activity_instance(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *instance_id,
    dcc_rest_cb cb,
    void *user_data
);

typedef struct dcc_soundboard_send_params {
    size_t size;
    dcc_snowflake_t sound_id;
    dcc_snowflake_t source_guild_id;
} dcc_soundboard_send_params_t;

typedef struct dcc_guild_soundboard_sound_params {
    size_t size;
    const char *name;
    const char *sound;
    double volume;
    dcc_snowflake_t emoji_id;
    const char *emoji_name;
    uint8_t has_volume;
    uint8_t has_emoji_id;
    uint8_t has_emoji_name;
} dcc_guild_soundboard_sound_params_t;

/**
 * @brief Submits `POST /channels/{channel.id}/send-soundboard-sound`.
 *
 * @param json_body JSON body containing `sound_id` and optional
 * `source_guild_id`.
 */
DCC_API dcc_status_t dcc_rest_send_soundboard_sound(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_soundboard_send_body(
    const dcc_soundboard_send_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_send_soundboard_sound_params(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_soundboard_send_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /soundboard-default-sounds`.
 */
DCC_API dcc_status_t dcc_rest_get_default_soundboard_sounds(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /guilds/{guild.id}/soundboard-sounds`.
 */
DCC_API dcc_status_t dcc_rest_get_guild_soundboard_sounds(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /guilds/{guild.id}/soundboard-sounds/{sound.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_guild_soundboard_sound(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sound_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `POST /guilds/{guild.id}/soundboard-sounds`.
 *
 * @param json_body JSON body containing soundboard sound creation fields.
 */
DCC_API dcc_status_t dcc_rest_create_guild_soundboard_sound(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_guild_soundboard_sound_create_body(
    const dcc_guild_soundboard_sound_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_create_guild_soundboard_sound_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_guild_soundboard_sound_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `PATCH /guilds/{guild.id}/soundboard-sounds/{sound.id}`.
 *
 * @param json_body JSON body containing soundboard sound edit fields.
 */
DCC_API dcc_status_t dcc_rest_modify_guild_soundboard_sound(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sound_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_guild_soundboard_sound_modify_body(
    const dcc_guild_soundboard_sound_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_modify_guild_soundboard_sound_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sound_id,
    const dcc_guild_soundboard_sound_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `DELETE /guilds/{guild.id}/soundboard-sounds/{sound.id}`.
 */
DCC_API dcc_status_t dcc_rest_delete_guild_soundboard_sound(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sound_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /skus/{sku.id}/subscriptions`.
 */
DCC_API dcc_status_t dcc_rest_get_sku_subscriptions(
    dcc_client_t *client,
    dcc_snowflake_t sku_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /skus/{sku.id}/subscriptions/{subscription.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_sku_subscription(
    dcc_client_t *client,
    dcc_snowflake_t sku_id,
    dcc_snowflake_t subscription_id,
    dcc_rest_cb cb,
    void *user_data
);

typedef struct dcc_bulk_ban_params {
    size_t size;
    const dcc_snowflake_t *user_ids;
    size_t user_id_count;
    uint32_t delete_message_seconds;
    uint8_t has_delete_message_seconds;
} dcc_bulk_ban_params_t;

/**
 * @brief Submits `POST /guilds/{guild.id}/bulk-ban`.
 *
 * @param json_body JSON body containing `user_ids` and optional
 * `delete_message_seconds`.
 */
DCC_API dcc_status_t dcc_rest_bulk_ban_guild_members(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_bulk_ban_body(
    const dcc_bulk_ban_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_bulk_ban_guild_members_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_bulk_ban_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /guilds/{guild.id}/roles/{role.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_guild_role(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /guilds/{guild.id}/roles/member-counts`.
 */
DCC_API dcc_status_t dcc_rest_get_guild_role_member_counts(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /guilds/{guild.id}/widget.json`.
 */
DCC_API dcc_status_t dcc_rest_get_guild_widget_json(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /guilds/{guild.id}/widget.png`.
 *
 * @param style Optional widget image style query value, such as `shield` or
 * `banner1`. Pass NULL for Discord's default style.
 */
DCC_API dcc_status_t dcc_rest_get_guild_widget_png(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *style,
    dcc_rest_cb cb,
    void *user_data
);

typedef struct dcc_guild_incident_actions_params {
    size_t size;
    const char *invites_disabled_until;
    const char *dms_disabled_until;
    uint8_t has_invites_disabled_until;
    uint8_t has_dms_disabled_until;
} dcc_guild_incident_actions_params_t;

/**
 * @brief Submits `PUT /guilds/{guild.id}/incident-actions`.
 */
DCC_API dcc_status_t dcc_rest_modify_guild_incident_actions(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_guild_incident_actions_body(
    const dcc_guild_incident_actions_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_modify_guild_incident_actions_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_guild_incident_actions_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API void dcc_rest_official_body_json_free(char *json);

/**
 * @brief Submits `GET /invites/{invite.code}/target-users`.
 */
DCC_API dcc_status_t dcc_rest_get_invite_target_users(
    dcc_client_t *client,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `PUT /invites/{invite.code}/target-users`.
 *
 * Uploads the CSV file as a multipart `target_users_file` form field.
 */
DCC_API dcc_status_t dcc_rest_put_invite_target_users(
    dcc_client_t *client,
    const char *invite_code,
    const char *filename,
    const void *csv_data,
    size_t csv_len,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /invites/{invite.code}/target-users/job-status`.
 */
DCC_API dcc_status_t dcc_rest_get_invite_target_users_job_status(
    dcc_client_t *client,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /applications/{application.id}/entitlements/{entitlement.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /sticker-packs/{pack.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_sticker_pack(
    dcc_client_t *client,
    dcc_snowflake_t pack_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /users/@me/guilds/{guild.id}/member`.
 */
DCC_API dcc_status_t dcc_rest_get_current_user_guild_member(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `DELETE /users/@me/applications/{application.id}/role-connection`.
 */
DCC_API dcc_status_t dcc_rest_delete_current_user_application_role_connection(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `POST /webhooks/{webhook.id}/{webhook.token}/slack`.
 */
DCC_API dcc_status_t dcc_rest_execute_webhook_slack(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `POST /webhooks/{webhook.id}/{webhook.token}/github`.
 */
DCC_API dcc_status_t dcc_rest_execute_webhook_github(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

typedef struct dcc_lobby_create_or_join_params {
    size_t size;
    const char *secret;
    uint32_t idle_timeout_seconds;
    const char *lobby_metadata_json;
    const char *member_metadata_json;
    uint8_t has_idle_timeout_seconds;
    uint8_t has_lobby_metadata;
    uint8_t has_member_metadata;
} dcc_lobby_create_or_join_params_t;

typedef struct dcc_lobby_member_spec {
    size_t size;
    dcc_snowflake_t user_id;
    const char *metadata_json;
    uint64_t flags;
    uint8_t has_metadata;
    uint8_t has_flags;
} dcc_lobby_member_spec_t;

typedef struct dcc_lobby_params {
    size_t size;
    const char *metadata_json;
    const dcc_lobby_member_spec_t *members;
    size_t member_count;
    uint32_t idle_timeout_seconds;
    uint8_t has_metadata;
    uint8_t has_members;
    uint8_t has_idle_timeout_seconds;
} dcc_lobby_params_t;

typedef struct dcc_lobby_channel_link_params {
    size_t size;
    dcc_snowflake_t channel_id;
} dcc_lobby_channel_link_params_t;

typedef struct dcc_lobby_member_params {
    size_t size;
    const char *metadata_json;
    uint64_t flags;
    uint8_t has_metadata;
    uint8_t has_flags;
} dcc_lobby_member_params_t;

typedef struct dcc_lobby_member_bulk_item {
    size_t size;
    dcc_snowflake_t user_id;
    const char *metadata_json;
    uint64_t flags;
    uint8_t has_metadata;
    uint8_t has_flags;
    uint8_t remove_member;
    uint8_t has_remove_member;
} dcc_lobby_member_bulk_item_t;

typedef struct dcc_lobby_member_bulk_params {
    size_t size;
    const dcc_lobby_member_bulk_item_t *items;
    size_t item_count;
} dcc_lobby_member_bulk_params_t;

typedef struct dcc_lobby_message_params {
    size_t size;
    const char *content;
    const char *metadata_json;
    uint64_t flags;
    uint8_t has_metadata;
    uint8_t has_flags;
} dcc_lobby_message_params_t;

typedef struct dcc_lobby_message_moderation_metadata_item {
    const char *key;
    const char *value;
} dcc_lobby_message_moderation_metadata_item_t;

typedef struct dcc_lobby_message_moderation_metadata_params {
    size_t size;
    const dcc_lobby_message_moderation_metadata_item_t *items;
    size_t item_count;
} dcc_lobby_message_moderation_metadata_params_t;

/**
 * @brief Submits `POST /lobbies`.
 */
DCC_API dcc_status_t dcc_rest_create_lobby(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_lobby_body(
    const dcc_lobby_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_create_lobby_params(
    dcc_client_t *client,
    const dcc_lobby_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `PUT /lobbies`.
 *
 * This Social SDK endpoint requires Bearer authorization; bot clients should
 * use it only with an OAuth-capable REST transport.
 */
DCC_API dcc_status_t dcc_rest_create_or_join_lobby(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_lobby_create_or_join_body(
    const dcc_lobby_create_or_join_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_create_or_join_lobby_params(
    dcc_client_t *client,
    const dcc_lobby_create_or_join_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /lobbies/{lobby.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_lobby(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `PATCH /lobbies/{lobby.id}`.
 */
DCC_API dcc_status_t dcc_rest_modify_lobby(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_lobby_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const dcc_lobby_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `DELETE /lobbies/{lobby.id}`.
 */
DCC_API dcc_status_t dcc_rest_delete_lobby(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `PUT /lobbies/{lobby.id}/members/{user.id}`.
 */
DCC_API dcc_status_t dcc_rest_add_lobby_member(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_lobby_member_body(
    const dcc_lobby_member_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_add_lobby_member_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t user_id,
    const dcc_lobby_member_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `POST /lobbies/{lobby.id}/members/bulk`.
 */
DCC_API dcc_status_t dcc_rest_bulk_update_lobby_members(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_lobby_member_bulk_body(
    const dcc_lobby_member_bulk_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_bulk_update_lobby_members_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const dcc_lobby_member_bulk_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `DELETE /lobbies/{lobby.id}/members/{user.id}`.
 */
DCC_API dcc_status_t dcc_rest_remove_lobby_member(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `DELETE /lobbies/{lobby.id}/members/@me`.
 */
DCC_API dcc_status_t dcc_rest_leave_lobby(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `PATCH /lobbies/{lobby.id}/channel-linking`.
 */
DCC_API dcc_status_t dcc_rest_link_lobby_channel(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_lobby_channel_link_body(
    const dcc_lobby_channel_link_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_link_lobby_channel_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const dcc_lobby_channel_link_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `PATCH /lobbies/{lobby.id}/channel-linking` with an empty body.
 */
DCC_API dcc_status_t dcc_rest_unlink_lobby_channel(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `POST /lobbies/{lobby.id}/messages`.
 */
DCC_API dcc_status_t dcc_rest_send_lobby_message(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_lobby_message_body(
    const dcc_lobby_message_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_send_lobby_message_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const dcc_lobby_message_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /lobbies/{lobby.id}/messages`.
 */
DCC_API dcc_status_t dcc_rest_get_lobby_messages(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `PUT /lobbies/{lobby.id}/messages/{message.id}/moderation-metadata`.
 */
DCC_API dcc_status_t dcc_rest_update_lobby_message_moderation_metadata(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t message_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_build_lobby_message_moderation_metadata_body(
    const dcc_lobby_message_moderation_metadata_params_t *params,
    char **out_json
);

DCC_API dcc_status_t dcc_rest_update_lobby_message_moderation_metadata_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t message_id,
    const dcc_lobby_message_moderation_metadata_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `POST /lobbies/{lobby.id}/members/@me/invites`.
 */
DCC_API dcc_status_t dcc_rest_create_lobby_channel_invite_for_self(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits `POST /lobbies/{lobby.id}/members/{user.id}/invites`.
 */
DCC_API dcc_status_t dcc_rest_create_lobby_channel_invite_for_user(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
