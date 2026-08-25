/**
 * @file include/dcc/rest/official_surface.h
 * @brief Public wrappers for Discord API endpoints not covered by the legacy
 * DPP parity layer.
 */

#ifndef DCC_REST_OFFICIAL_SURFACE_H
#define DCC_REST_OFFICIAL_SURFACE_H

#include <dcc/rest/base.h>
#include <dcc/application_types.h>
#include <dcc/rest/request.h>
#include <dcc/rest/types/guild_members.h>
#include <dcc/rest/types/guilds.h>
#include <dcc/rest/types/invites.h>
#include <dcc/rest/types/official_surface.h>
#include <dcc/rest/types/webhooks.h>

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
    dcc_client_t *client, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /applications/@me`.
 */

/**
 * @brief Submits `PATCH /applications/@me`.
 *
 * @param json_body JSON application edit payload.
 */
DCC_API dcc_status_t dcc_rest_modify_current_application(
    dcc_client_t *client, const dcc_application_modify_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET
 * /applications/{application.id}/activity-instances/{instance_id}`.
 */
DCC_API dcc_status_t dcc_rest_get_activity_instance(
    dcc_client_t *client, dcc_snowflake_t application_id,
    const char *instance_id, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);


/**
 * @brief Submits `POST /channels/{channel.id}/send-soundboard-sound`.
 *
 * @param json_body JSON body containing `sound_id` and optional
 * `source_guild_id`.
 */
DCC_API dcc_status_t dcc_rest_send_soundboard_sound(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_soundboard_send_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /soundboard-default-sounds`.
 */
DCC_API dcc_status_t dcc_rest_get_default_soundboard_sounds(
    dcc_client_t *client, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /guilds/{guild.id}/soundboard-sounds`.
 */
DCC_API dcc_status_t dcc_rest_get_guild_soundboard_sounds(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /guilds/{guild.id}/soundboard-sounds/{sound.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_guild_soundboard_sound(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t sound_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `POST /guilds/{guild.id}/soundboard-sounds`.
 *
 */
DCC_API dcc_status_t dcc_rest_create_guild_soundboard_sound(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_soundboard_sound_create_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `PATCH /guilds/{guild.id}/soundboard-sounds/{sound.id}`.
 *
 */
DCC_API dcc_status_t dcc_rest_modify_guild_soundboard_sound(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t sound_id,
    const dcc_rest_guild_soundboard_sound_update_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `DELETE /guilds/{guild.id}/soundboard-sounds/{sound.id}`.
 */
DCC_API dcc_status_t dcc_rest_delete_guild_soundboard_sound(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t sound_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /skus/{sku.id}/subscriptions`.
 */
DCC_API dcc_status_t dcc_rest_get_sku_subscriptions(
    dcc_client_t *client, dcc_snowflake_t sku_id,
    const dcc_rest_sku_subscription_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /skus/{sku.id}/subscriptions/{subscription.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_sku_subscription(
    dcc_client_t *client, dcc_snowflake_t sku_id,
    dcc_snowflake_t subscription_id, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

/**
 * @brief Submits `POST /guilds/{guild.id}/bulk-ban`.
 *
 */
DCC_API dcc_status_t dcc_rest_bulk_ban_guild_members(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_bulk_ban_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /guilds/{guild.id}/roles/{role.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_guild_role(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t role_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /guilds/{guild.id}/roles/member-counts`.
 */
DCC_API dcc_status_t dcc_rest_get_guild_role_member_counts(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /guilds/{guild.id}/widget.json`.
 */
DCC_API dcc_status_t dcc_rest_get_guild_widget_json(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /guilds/{guild.id}/widget.png`.
 *
 */
DCC_API dcc_status_t dcc_rest_get_guild_widget_png(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_widget_image_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `PUT /guilds/{guild.id}/incident-actions`.
 */
DCC_API dcc_status_t dcc_rest_modify_guild_incident_actions(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_incident_actions_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /invites/{invite.code}/target-users`.
 */
DCC_API dcc_status_t dcc_rest_get_invite_target_users(
    dcc_client_t *client, const char *invite_code,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `PUT /invites/{invite.code}/target-users`.
 *
 * Uploads the CSV file as a multipart `target_users_file` form field.
 */
DCC_API dcc_status_t dcc_rest_put_invite_target_users(
    dcc_client_t *client, const char *invite_code,
    const dcc_rest_invite_target_users_upload_t *upload,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /invites/{invite.code}/target-users/job-status`.
 */
DCC_API dcc_status_t dcc_rest_get_invite_target_users_job_status(
    dcc_client_t *client, const char *invite_code,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET
 * /applications/{application.id}/entitlements/{entitlement.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_entitlement(
    dcc_client_t *client, dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /sticker-packs/{pack.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_sticker_pack(
    dcc_client_t *client,
    dcc_snowflake_t pack_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `GET /users/@me/guilds/{guild.id}/member`.
 */
DCC_API dcc_status_t dcc_rest_get_current_user_guild_member(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `DELETE
 * /users/@me/applications/{application.id}/role-connection`.
 */
DCC_API dcc_status_t dcc_rest_delete_current_user_application_role_connection(
    dcc_client_t *client, dcc_snowflake_t application_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `POST /webhooks/{webhook.id}/{webhook.token}/slack`.
 * @details `payload` is borrowed for the call. Uses the
 * @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_execute_webhook_slack(
    dcc_client_t *client, dcc_snowflake_t webhook_id, const char *webhook_token,
    const dcc_rest_webhook_compat_payload_t *payload,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `POST /webhooks/{webhook.id}/{webhook.token}/github`.
 * @details `payload` is borrowed for the call. Uses the
 * @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_execute_webhook_github(
    dcc_client_t *client, dcc_snowflake_t webhook_id, const char *webhook_token,
    const dcc_rest_webhook_compat_payload_t *payload,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `POST /lobbies`.
 */
DCC_API dcc_status_t dcc_rest_create_lobby(
    dcc_client_t *client, const dcc_lobby_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `PUT /lobbies`.
 *
 * This Social SDK endpoint requires Bearer authorization; bot clients should
 * use it only with an OAuth-capable REST transport.
 */
DCC_API dcc_status_t dcc_rest_create_or_join_lobby(
    dcc_client_t *client, const dcc_lobby_create_or_join_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /lobbies/{lobby.id}`.
 */
DCC_API dcc_status_t dcc_rest_get_lobby(
    dcc_client_t *client, dcc_snowflake_t lobby_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `PATCH /lobbies/{lobby.id}`.
 */
DCC_API dcc_status_t dcc_rest_modify_lobby(
    dcc_client_t *client, dcc_snowflake_t lobby_id,
    const dcc_lobby_params_t *params, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

/**
 * @brief Submits `DELETE /lobbies/{lobby.id}`.
 */
DCC_API dcc_status_t dcc_rest_delete_lobby(
    dcc_client_t *client, dcc_snowflake_t lobby_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `PUT /lobbies/{lobby.id}/members/{user.id}`.
 */
DCC_API dcc_status_t dcc_rest_add_lobby_member(
    dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_snowflake_t user_id,
    const dcc_lobby_member_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `POST /lobbies/{lobby.id}/members/bulk`.
 */
DCC_API dcc_status_t dcc_rest_bulk_update_lobby_members(
    dcc_client_t *client, dcc_snowflake_t lobby_id,
    const dcc_lobby_member_bulk_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `DELETE /lobbies/{lobby.id}/members/{user.id}`.
 */
DCC_API dcc_status_t dcc_rest_remove_lobby_member(
    dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `DELETE /lobbies/{lobby.id}/members/@me`.
 */
DCC_API dcc_status_t dcc_rest_leave_lobby(
    dcc_client_t *client, dcc_snowflake_t lobby_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `PATCH /lobbies/{lobby.id}/channel-linking`.
 */
DCC_API dcc_status_t dcc_rest_link_lobby_channel(
    dcc_client_t *client, dcc_snowflake_t lobby_id,
    const dcc_lobby_channel_link_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `POST /lobbies/{lobby.id}/messages`.
 */
DCC_API dcc_status_t dcc_rest_send_lobby_message(
    dcc_client_t *client, dcc_snowflake_t lobby_id,
    const dcc_lobby_message_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `GET /lobbies/{lobby.id}/messages`.
 */
DCC_API dcc_status_t dcc_rest_get_lobby_messages(
    dcc_client_t *client, dcc_snowflake_t lobby_id,
    const dcc_rest_lobby_message_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `PUT
 * /lobbies/{lobby.id}/messages/{message.id}/moderation-metadata`.
 */
DCC_API dcc_status_t dcc_rest_update_lobby_message_moderation_metadata(
    dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_snowflake_t message_id,
    const dcc_lobby_message_moderation_metadata_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `POST /lobbies/{lobby.id}/members/@me/invites`.
 */
DCC_API dcc_status_t dcc_rest_create_lobby_channel_invite_for_self(
    dcc_client_t *client, dcc_snowflake_t lobby_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

/**
 * @brief Submits `POST /lobbies/{lobby.id}/members/{user.id}/invites`.
 */
DCC_API dcc_status_t dcc_rest_create_lobby_channel_invite_for_user(
    dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

#ifdef __cplusplus
}
#endif

#endif
