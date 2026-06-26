/**
 * @file include/dcc/sugar/official_surface.h
 * @brief Sugar macros for the official Discord API surface wrappers.
 */

#ifndef DCC_SUGAR_OFFICIAL_SURFACE_H
#define DCC_SUGAR_OFFICIAL_SURFACE_H

#include <dcc/client.h>
#include <dcc/oauth2.h>
#include <dcc/rest/messages/pins.h>
#include <dcc/rest/official_surface.h>
#include <dcc/rest/resources/gateway.h>
#include <dcc/webhook_events.h>

/** @brief Initializes Gateway Request Guild Members options. */
#define DCC_GATEWAY_MEMBERS(...) \
    ((dcc_gateway_request_guild_members_options_t){.size = sizeof(dcc_gateway_request_guild_members_options_t), __VA_ARGS__})

/** @brief Initializes Gateway Update Presence options. */
#define DCC_GATEWAY_PRESENCE(...) \
    ((dcc_gateway_presence_update_t){.size = sizeof(dcc_gateway_presence_update_t), __VA_ARGS__})

/** @brief Initializes OAuth2 authorization-code exchange options. */
#define DCC_OAUTH2_EXCHANGE_CODE_OPTIONS(...) \
    ((dcc_oauth2_exchange_code_options_t){.size = sizeof(dcc_oauth2_exchange_code_options_t), __VA_ARGS__})

/** @brief Initializes OAuth2 refresh-token options. */
#define DCC_OAUTH2_REFRESH_TOKEN_OPTIONS(...) \
    ((dcc_oauth2_refresh_token_options_t){.size = sizeof(dcc_oauth2_refresh_token_options_t), __VA_ARGS__})

/** @brief Initializes OAuth2 revoke-token options. */
#define DCC_OAUTH2_REVOKE_TOKEN_OPTIONS(...) \
    ((dcc_oauth2_revoke_token_options_t){.size = sizeof(dcc_oauth2_revoke_token_options_t), __VA_ARGS__})

/** @brief Initializes application default install params. */
#define DCC_APPLICATION_INSTALL_PARAMS(scopes_, scope_count_, permissions_) \
    ((dcc_application_install_params_t){ \
        .size = sizeof(dcc_application_install_params_t), \
        .scopes = (scopes_), \
        .scope_count = (scope_count_), \
        .permissions = (permissions_), \
        .has_permissions = 1 \
    })

/** @brief Initializes one application integration type configuration. */
#define DCC_APPLICATION_INTEGRATION_CONFIG(type_, install_params_) \
    ((dcc_application_integration_type_config_t){ \
        .size = sizeof(dcc_application_integration_type_config_t), \
        .integration_type = (type_), \
        .oauth2_install_params = (install_params_), \
        .has_oauth2_install_params = 1 \
    })

/** @brief Initializes an application edit body. */
#define DCC_APPLICATION_MODIFY(...) \
    ((dcc_application_modify_params_t){.size = sizeof(dcc_application_modify_params_t), __VA_ARGS__})

/** @brief Initializes a send-soundboard-sound body. */
#define DCC_SOUNDBOARD_SEND(...) \
    ((dcc_soundboard_send_params_t){.size = sizeof(dcc_soundboard_send_params_t), __VA_ARGS__})

/** @brief Initializes a guild soundboard sound create/modify body. */
#define DCC_GUILD_SOUNDBOARD_SOUND(...) \
    ((dcc_guild_soundboard_sound_params_t){.size = sizeof(dcc_guild_soundboard_sound_params_t), __VA_ARGS__})

/** @brief Initializes a bulk guild ban body. */
#define DCC_BULK_BAN_BODY(...) \
    ((dcc_bulk_ban_params_t){.size = sizeof(dcc_bulk_ban_params_t), __VA_ARGS__})

/** @brief Initializes a guild incident-actions body. */
#define DCC_INCIDENT_ACTIONS_BODY(...) \
    ((dcc_guild_incident_actions_params_t){.size = sizeof(dcc_guild_incident_actions_params_t), __VA_ARGS__})

/** @brief Initializes a create-or-join lobby body. */
#define DCC_LOBBY_CREATE_OR_JOIN(...) \
    ((dcc_lobby_create_or_join_params_t){.size = sizeof(dcc_lobby_create_or_join_params_t), __VA_ARGS__})

/** @brief Initializes one create/modify lobby member spec. */
#define DCC_LOBBY_MEMBER_SPEC(...) \
    ((dcc_lobby_member_spec_t){.size = sizeof(dcc_lobby_member_spec_t), __VA_ARGS__})

/** @brief Initializes a create/modify lobby body. */
#define DCC_LOBBY(...) \
    ((dcc_lobby_params_t){.size = sizeof(dcc_lobby_params_t), __VA_ARGS__})

/** @brief Initializes a link-lobby-channel body. */
#define DCC_LOBBY_LINK_CHANNEL(...) \
    ((dcc_lobby_channel_link_params_t){.size = sizeof(dcc_lobby_channel_link_params_t), __VA_ARGS__})

/** @brief Initializes an add-lobby-member body. */
#define DCC_LOBBY_MEMBER(...) \
    ((dcc_lobby_member_params_t){.size = sizeof(dcc_lobby_member_params_t), __VA_ARGS__})

/** @brief Initializes one bulk lobby member update item. */
#define DCC_LOBBY_BULK_MEMBER(...) \
    ((dcc_lobby_member_bulk_item_t){.size = sizeof(dcc_lobby_member_bulk_item_t), __VA_ARGS__})

/** @brief Initializes a bulk lobby member update body. */
#define DCC_LOBBY_BULK_MEMBERS(items_, count_) \
    ((dcc_lobby_member_bulk_params_t){ \
        .size = sizeof(dcc_lobby_member_bulk_params_t), \
        .items = (items_), \
        .item_count = (count_) \
    })

/** @brief Initializes a send-lobby-message body. */
#define DCC_LOBBY_MESSAGE(...) \
    ((dcc_lobby_message_params_t){.size = sizeof(dcc_lobby_message_params_t), __VA_ARGS__})

/** @brief Initializes one lobby message moderation metadata item. */
#define DCC_LOBBY_MODERATION_METADATA_ITEM(key_, value_) \
    ((dcc_lobby_message_moderation_metadata_item_t){.key = (key_), .value = (value_)})

/** @brief Initializes a lobby message moderation metadata body. */
#define DCC_LOBBY_MODERATION_METADATA(items_, count_) \
    ((dcc_lobby_message_moderation_metadata_params_t){ \
        .size = sizeof(dcc_lobby_message_moderation_metadata_params_t), \
        .items = (items_), \
        .item_count = (count_) \
    })

/** @brief Initializes a caller-owned Webhook Events route. */
#define DCC_WEBHOOK_EVENT_ROUTE(callback_, user_data_) \
    ((dcc_webhook_event_route_t){ \
        .size = sizeof(dcc_webhook_event_route_t), \
        .callback = (callback_), \
        .user_data = (user_data_) \
    })

/** @brief Initializes an unsigned caller-owned Webhook Events route. */
#define DCC_WEBHOOK_EVENT_ROUTE_UNSIGNED(callback_, user_data_) \
    ((dcc_webhook_event_route_t){ \
        .size = sizeof(dcc_webhook_event_route_t), \
        .flags = DCC_INTERACTION_ROUTE_F_UNSIGNED, \
        .callback = (callback_), \
        .user_data = (user_data_) \
    })

#define DCC_REST_GATEWAY(client_, cb_, user_data_) \
    dcc_rest_get_gateway((client_), (cb_), (user_data_))
#define DCC_REST_GATEWAY_BOT(client_, cb_, user_data_) \
    dcc_rest_get_gateway_bot((client_), (cb_), (user_data_))
#define DCC_REST_CURRENT_APPLICATION(client_, cb_, user_data_) \
    dcc_rest_get_current_application((client_), (cb_), (user_data_))
#define DCC_REST_MODIFY_CURRENT_APPLICATION(client_, json_, cb_, user_data_) \
    dcc_rest_modify_current_application((client_), (json_), (cb_), (user_data_))
#define DCC_REST_MODIFY_CURRENT_APPLICATION_PARAMS(client_, params_, cb_, user_data_) \
    dcc_rest_modify_current_application_params((client_), (params_), (cb_), (user_data_))
#define DCC_REST_ACTIVITY_INSTANCE(client_, application_id_, instance_id_, cb_, user_data_) \
    dcc_rest_get_activity_instance((client_), (application_id_), (instance_id_), (cb_), (user_data_))
#define DCC_REST_SEND_SOUNDBOARD(client_, channel_id_, json_, cb_, user_data_) \
    dcc_rest_send_soundboard_sound((client_), (channel_id_), (json_), (cb_), (user_data_))
#define DCC_REST_SEND_SOUNDBOARD_PARAMS(client_, channel_id_, params_, cb_, user_data_) \
    dcc_rest_send_soundboard_sound_params((client_), (channel_id_), (params_), (cb_), (user_data_))
#define DCC_REST_DEFAULT_SOUNDBOARD_SOUNDS(client_, cb_, user_data_) \
    dcc_rest_get_default_soundboard_sounds((client_), (cb_), (user_data_))
#define DCC_REST_GUILD_SOUNDBOARD_SOUNDS(client_, guild_id_, cb_, user_data_) \
    dcc_rest_get_guild_soundboard_sounds((client_), (guild_id_), (cb_), (user_data_))
#define DCC_REST_GUILD_SOUNDBOARD_SOUND(client_, guild_id_, sound_id_, cb_, user_data_) \
    dcc_rest_get_guild_soundboard_sound((client_), (guild_id_), (sound_id_), (cb_), (user_data_))
#define DCC_REST_CREATE_GUILD_SOUNDBOARD_SOUND(client_, guild_id_, json_, cb_, user_data_) \
    dcc_rest_create_guild_soundboard_sound((client_), (guild_id_), (json_), (cb_), (user_data_))
#define DCC_REST_CREATE_GUILD_SOUNDBOARD_SOUND_PARAMS(client_, guild_id_, params_, cb_, user_data_) \
    dcc_rest_create_guild_soundboard_sound_params((client_), (guild_id_), (params_), (cb_), (user_data_))
#define DCC_REST_MODIFY_GUILD_SOUNDBOARD_SOUND(client_, guild_id_, sound_id_, json_, cb_, user_data_) \
    dcc_rest_modify_guild_soundboard_sound((client_), (guild_id_), (sound_id_), (json_), (cb_), (user_data_))
#define DCC_REST_MODIFY_GUILD_SOUNDBOARD_SOUND_PARAMS(client_, guild_id_, sound_id_, params_, cb_, user_data_) \
    dcc_rest_modify_guild_soundboard_sound_params((client_), (guild_id_), (sound_id_), (params_), (cb_), (user_data_))
#define DCC_REST_DELETE_GUILD_SOUNDBOARD_SOUND(client_, guild_id_, sound_id_, cb_, user_data_) \
    dcc_rest_delete_guild_soundboard_sound((client_), (guild_id_), (sound_id_), (cb_), (user_data_))
#define DCC_REST_SKU_SUBSCRIPTIONS(client_, sku_id_, query_, cb_, user_data_) \
    dcc_rest_get_sku_subscriptions((client_), (sku_id_), (query_), (cb_), (user_data_))
#define DCC_REST_SKU_SUBSCRIPTION(client_, sku_id_, subscription_id_, cb_, user_data_) \
    dcc_rest_get_sku_subscription((client_), (sku_id_), (subscription_id_), (cb_), (user_data_))
#define DCC_REST_BULK_BAN(client_, guild_id_, json_, cb_, user_data_) \
    dcc_rest_bulk_ban_guild_members((client_), (guild_id_), (json_), (cb_), (user_data_))
#define DCC_REST_BULK_BAN_PARAMS(client_, guild_id_, params_, cb_, user_data_) \
    dcc_rest_bulk_ban_guild_members_params((client_), (guild_id_), (params_), (cb_), (user_data_))
#define DCC_REST_GUILD_ROLE(client_, guild_id_, role_id_, cb_, user_data_) \
    dcc_rest_get_guild_role((client_), (guild_id_), (role_id_), (cb_), (user_data_))
#define DCC_REST_ROLE_MEMBER_COUNTS(client_, guild_id_, cb_, user_data_) \
    dcc_rest_get_guild_role_member_counts((client_), (guild_id_), (cb_), (user_data_))
#define DCC_REST_WIDGET_JSON(client_, guild_id_, cb_, user_data_) \
    dcc_rest_get_guild_widget_json((client_), (guild_id_), (cb_), (user_data_))
#define DCC_REST_WIDGET_PNG(client_, guild_id_, style_, cb_, user_data_) \
    dcc_rest_get_guild_widget_png((client_), (guild_id_), (style_), (cb_), (user_data_))
#define DCC_REST_INCIDENT_ACTIONS(client_, guild_id_, json_, cb_, user_data_) \
    dcc_rest_modify_guild_incident_actions((client_), (guild_id_), (json_), (cb_), (user_data_))
#define DCC_REST_INCIDENT_ACTIONS_PARAMS(client_, guild_id_, params_, cb_, user_data_) \
    dcc_rest_modify_guild_incident_actions_params((client_), (guild_id_), (params_), (cb_), (user_data_))
#define DCC_REST_INVITE_TARGET_USERS(client_, invite_code_, cb_, user_data_) \
    dcc_rest_get_invite_target_users((client_), (invite_code_), (cb_), (user_data_))
#define DCC_REST_PUT_INVITE_TARGET_USERS(client_, invite_code_, filename_, csv_, csv_len_, cb_, user_data_) \
    dcc_rest_put_invite_target_users((client_), (invite_code_), (filename_), (csv_), (csv_len_), (cb_), (user_data_))
#define DCC_REST_INVITE_TARGET_USERS_JOB(client_, invite_code_, cb_, user_data_) \
    dcc_rest_get_invite_target_users_job_status((client_), (invite_code_), (cb_), (user_data_))
#define DCC_REST_ENTITLEMENT(client_, application_id_, entitlement_id_, cb_, user_data_) \
    dcc_rest_get_entitlement((client_), (application_id_), (entitlement_id_), (cb_), (user_data_))
#define DCC_REST_STICKER_PACK(client_, pack_id_, cb_, user_data_) \
    dcc_rest_get_sticker_pack((client_), (pack_id_), (cb_), (user_data_))
#define DCC_REST_CURRENT_USER_GUILD_MEMBER(client_, guild_id_, cb_, user_data_) \
    dcc_rest_get_current_user_guild_member((client_), (guild_id_), (cb_), (user_data_))
#define DCC_REST_DELETE_USER_ROLE_CONNECTION(client_, application_id_, cb_, user_data_) \
    dcc_rest_delete_current_user_application_role_connection((client_), (application_id_), (cb_), (user_data_))
#define DCC_REST_WEBHOOK_SLACK(client_, webhook_id_, token_, json_, cb_, user_data_) \
    dcc_rest_execute_webhook_slack((client_), (webhook_id_), (token_), (json_), (cb_), (user_data_))
#define DCC_REST_WEBHOOK_GITHUB(client_, webhook_id_, token_, json_, cb_, user_data_) \
    dcc_rest_execute_webhook_github((client_), (webhook_id_), (token_), (json_), (cb_), (user_data_))

#define DCC_REST_LEGACY_PIN_MESSAGE(client_, channel_id_, message_id_, cb_, user_data_) \
    dcc_rest_legacy_pin_message((client_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_REST_LEGACY_UNPIN_MESSAGE(client_, channel_id_, message_id_, cb_, user_data_) \
    dcc_rest_legacy_unpin_message((client_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_REST_LEGACY_CHANNEL_PINS(client_, channel_id_, query_, cb_, user_data_) \
    dcc_rest_get_legacy_channel_pins((client_), (channel_id_), (query_), (cb_), (user_data_))

#define DCC_REST_CREATE_LOBBY(client_, json_, cb_, user_data_) \
    dcc_rest_create_lobby((client_), (json_), (cb_), (user_data_))
#define DCC_REST_CREATE_LOBBY_PARAMS(client_, params_, cb_, user_data_) \
    dcc_rest_create_lobby_params((client_), (params_), (cb_), (user_data_))
#define DCC_REST_CREATE_OR_JOIN_LOBBY(client_, json_, cb_, user_data_) \
    dcc_rest_create_or_join_lobby((client_), (json_), (cb_), (user_data_))
#define DCC_REST_CREATE_OR_JOIN_LOBBY_PARAMS(client_, params_, cb_, user_data_) \
    dcc_rest_create_or_join_lobby_params((client_), (params_), (cb_), (user_data_))
#define DCC_REST_LOBBY(client_, lobby_id_, cb_, user_data_) \
    dcc_rest_get_lobby((client_), (lobby_id_), (cb_), (user_data_))
#define DCC_REST_MODIFY_LOBBY(client_, lobby_id_, json_, cb_, user_data_) \
    dcc_rest_modify_lobby((client_), (lobby_id_), (json_), (cb_), (user_data_))
#define DCC_REST_MODIFY_LOBBY_PARAMS(client_, lobby_id_, params_, cb_, user_data_) \
    dcc_rest_modify_lobby_params((client_), (lobby_id_), (params_), (cb_), (user_data_))
#define DCC_REST_DELETE_LOBBY(client_, lobby_id_, cb_, user_data_) \
    dcc_rest_delete_lobby((client_), (lobby_id_), (cb_), (user_data_))
#define DCC_REST_ADD_LOBBY_MEMBER(client_, lobby_id_, user_id_, json_, cb_, user_data_) \
    dcc_rest_add_lobby_member((client_), (lobby_id_), (user_id_), (json_), (cb_), (user_data_))
#define DCC_REST_ADD_LOBBY_MEMBER_PARAMS(client_, lobby_id_, user_id_, params_, cb_, user_data_) \
    dcc_rest_add_lobby_member_params((client_), (lobby_id_), (user_id_), (params_), (cb_), (user_data_))
#define DCC_REST_BULK_UPDATE_LOBBY_MEMBERS(client_, lobby_id_, json_, cb_, user_data_) \
    dcc_rest_bulk_update_lobby_members((client_), (lobby_id_), (json_), (cb_), (user_data_))
#define DCC_REST_BULK_UPDATE_LOBBY_MEMBERS_PARAMS(client_, lobby_id_, params_, cb_, user_data_) \
    dcc_rest_bulk_update_lobby_members_params((client_), (lobby_id_), (params_), (cb_), (user_data_))
#define DCC_REST_REMOVE_LOBBY_MEMBER(client_, lobby_id_, user_id_, cb_, user_data_) \
    dcc_rest_remove_lobby_member((client_), (lobby_id_), (user_id_), (cb_), (user_data_))
#define DCC_REST_LEAVE_LOBBY(client_, lobby_id_, cb_, user_data_) \
    dcc_rest_leave_lobby((client_), (lobby_id_), (cb_), (user_data_))
#define DCC_REST_LINK_LOBBY_CHANNEL(client_, lobby_id_, json_, cb_, user_data_) \
    dcc_rest_link_lobby_channel((client_), (lobby_id_), (json_), (cb_), (user_data_))
#define DCC_REST_LINK_LOBBY_CHANNEL_PARAMS(client_, lobby_id_, params_, cb_, user_data_) \
    dcc_rest_link_lobby_channel_params((client_), (lobby_id_), (params_), (cb_), (user_data_))
#define DCC_REST_UNLINK_LOBBY_CHANNEL(client_, lobby_id_, cb_, user_data_) \
    dcc_rest_unlink_lobby_channel((client_), (lobby_id_), (cb_), (user_data_))
#define DCC_REST_SEND_LOBBY_MESSAGE(client_, lobby_id_, json_, cb_, user_data_) \
    dcc_rest_send_lobby_message((client_), (lobby_id_), (json_), (cb_), (user_data_))
#define DCC_REST_SEND_LOBBY_MESSAGE_PARAMS(client_, lobby_id_, params_, cb_, user_data_) \
    dcc_rest_send_lobby_message_params((client_), (lobby_id_), (params_), (cb_), (user_data_))
#define DCC_REST_LOBBY_MESSAGES(client_, lobby_id_, query_, cb_, user_data_) \
    dcc_rest_get_lobby_messages((client_), (lobby_id_), (query_), (cb_), (user_data_))
#define DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION(client_, lobby_id_, message_id_, json_, cb_, user_data_) \
    dcc_rest_update_lobby_message_moderation_metadata((client_), (lobby_id_), (message_id_), (json_), (cb_), (user_data_))
#define DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION_PARAMS(client_, lobby_id_, message_id_, params_, cb_, user_data_) \
    dcc_rest_update_lobby_message_moderation_metadata_params( \
        (client_), \
        (lobby_id_), \
        (message_id_), \
        (params_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_REST_CREATE_LOBBY_INVITE_SELF(client_, lobby_id_, cb_, user_data_) \
    dcc_rest_create_lobby_channel_invite_for_self((client_), (lobby_id_), (cb_), (user_data_))
#define DCC_REST_CREATE_LOBBY_INVITE_USER(client_, lobby_id_, user_id_, cb_, user_data_) \
    dcc_rest_create_lobby_channel_invite_for_user((client_), (lobby_id_), (user_id_), (cb_), (user_data_))

#define DCC_GATEWAY_REQUEST_MEMBERS(client_, options_) \
    dcc_client_request_guild_members((client_), (options_))
#define DCC_GATEWAY_REQUEST_SOUNDBOARD(client_, guild_ids_, guild_id_count_) \
    dcc_client_request_soundboard_sounds((client_), (guild_ids_), (guild_id_count_))
#define DCC_GATEWAY_REQUEST_CHANNEL_INFO(client_, guild_id_, fields_, field_count_) \
    dcc_client_request_channel_info((client_), (guild_id_), (fields_), (field_count_))
#define DCC_GATEWAY_UPDATE_PRESENCE(client_, presence_) \
    dcc_client_update_presence((client_), (presence_))
#define DCC_GATEWAY_UPDATE_PRESENCE_RAW(client_, json_) \
    dcc_client_update_presence_raw((client_), (json_))

#define DCC_OAUTH2_ME(access_token_, cb_, user_data_) \
    dcc_oauth2_get_current_authorization((access_token_), (cb_), (user_data_))
#define DCC_OAUTH2_CURRENT_BOT_APPLICATION(bot_token_, cb_, user_data_) \
    dcc_oauth2_get_current_bot_application((bot_token_), (cb_), (user_data_))
#define DCC_OAUTH2_EXCHANGE_CODE(options_, cb_, user_data_) \
    dcc_oauth2_exchange_code((options_), (cb_), (user_data_))
#define DCC_OAUTH2_REFRESH_TOKEN(options_, cb_, user_data_) \
    dcc_oauth2_refresh_token((options_), (cb_), (user_data_))
#define DCC_OAUTH2_REVOKE_TOKEN(options_, cb_, user_data_) \
    dcc_oauth2_revoke_token((options_), (cb_), (user_data_))

#define DCC_WEBHOOK_EVENTS_ADD_ROUTE(server_, path_, route_) \
    dcc_webhook_events_add_route((server_), (path_), (route_))

#endif
