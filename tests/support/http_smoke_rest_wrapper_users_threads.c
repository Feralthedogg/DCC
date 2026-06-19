#include "http_smoke_support.h"

#if !defined(_WIN32)

int run_public_rest_wrapper_expect_users_threads_smoke(dcc_client_t *client) {
    if (
        run_public_rest_wrapper_expect(
                    client,
                    "leave_guild",
                    call_rest_leave_guild,
                    "DELETE",
                    "/users/@me/guilds/333",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "add_group_dm_recipient",
                    call_rest_add_group_dm_recipient,
                    "PUT",
                    "/channels/222/recipients/444",
                    "{\"access_token\":\"tok\",\"nick\":\"feral\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "add_group_dm_recipient_params",
                    call_rest_add_group_dm_recipient_params,
                    "PUT",
                    "/channels/222/recipients/444",
                    "{\"access_token\":\"tok2\",\"nick\":\"feral2\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "remove_group_dm_recipient",
                    call_rest_remove_group_dm_recipient,
                    "DELETE",
                    "/channels/222/recipients/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_current_user_voice_state",
                    call_rest_get_current_user_voice_state,
                    "GET",
                    "/guilds/333/voice-states/@me",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_current_user_voice_state",
                    call_rest_modify_current_user_voice_state,
                    "PATCH",
                    "/guilds/333/voice-states/@me",
                    "{\"channel_id\":\"222\",\"suppress\":false}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_current_user_voice_state_params",
                    call_rest_modify_current_user_voice_state_params,
                    "PATCH",
                    "/guilds/333/voice-states/@me",
                    "{\"channel_id\":\"222\",\"suppress\":false,\"request_to_speak_timestamp\":\"2026-06-16T00:00:00.000000+00:00\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_user_voice_state",
                    call_rest_get_user_voice_state,
                    "GET",
                    "/guilds/333/voice-states/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_user_voice_state",
                    call_rest_modify_user_voice_state,
                    "PATCH",
                    "/guilds/333/voice-states/444",
                    "{\"channel_id\":\"222\",\"suppress\":true}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_user_voice_state_params",
                    call_rest_modify_user_voice_state_params,
                    "PATCH",
                    "/guilds/333/voice-states/444",
                    "{\"channel_id\":\"222\",\"suppress\":true}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_voice_regions",
                    call_rest_get_voice_regions,
                    "GET",
                    "/voice/regions",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_voice_regions",
                    call_rest_get_guild_voice_regions,
                    "GET",
                    "/guilds/333/regions",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_skus",
                    call_rest_get_skus,
                    "GET",
                    "/applications/123/skus",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_entitlements",
                    call_rest_get_entitlements,
                    "GET",
                    "/applications/123/entitlements?limit=2&user_id=444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_entitlements_page",
                    call_rest_get_entitlements_page,
                    "GET",
                    "/applications/123/entitlements?user_id=444&sku_ids=555%2C556&before_id=111&after_id=222&limit=25&guild_id=333&exclude_ended=true",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_test_entitlement",
                    call_rest_create_test_entitlement,
                    "POST",
                    "/applications/123/entitlements",
                    "{\"sku_id\":\"555\",\"owner_id\":\"444\",\"owner_type\":2}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_test_entitlement_params",
                    call_rest_create_test_entitlement_params,
                    "POST",
                    "/applications/123/entitlements",
                    "{\"sku_id\":\"555\",\"owner_id\":\"444\",\"owner_type\":2}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_test_entitlement",
                    call_rest_delete_test_entitlement,
                    "DELETE",
                    "/applications/123/entitlements/666",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "consume_entitlement",
                    call_rest_consume_entitlement,
                    "POST",
                    "/applications/123/entitlements/666/consume",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_gateway_bot",
                    call_rest_get_gateway_bot,
                    "GET",
                    "/gateway/bot",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_member",
                    call_rest_get_guild_member,
                    "GET",
                    "/guilds/333/members/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "list_guild_members",
                    call_rest_list_guild_members,
                    "GET",
                    "/guilds/333/members?limit=2&after=444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "list_guild_members_page",
                    call_rest_list_guild_members_page,
                    "GET",
                    "/guilds/333/members?after=444&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "search_guild_members",
                    call_rest_search_guild_members,
                    "GET",
                    "/guilds/333/members/search?query=feral&limit=1",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "search_guild_members_page",
                    call_rest_search_guild_members_page,
                    "GET",
                    "/guilds/333/members/search?limit=1&query=feral%20ops",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "add_guild_member",
                    call_rest_add_guild_member,
                    "PUT",
                    "/guilds/333/members/444",
                    "{\"access_token\":\"tok\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "add_guild_member_params",
                    call_rest_add_guild_member_params,
                    "PUT",
                    "/guilds/333/members/444",
                    "{\"nick\":\"feral typed\",\"roles\":[\"555\",\"556\"],\"access_token\":\"tok2\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_member",
                    call_rest_modify_guild_member,
                    "PATCH",
                    "/guilds/333/members/444",
                    "{\"nick\":\"feral\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_member_params",
                    call_rest_modify_guild_member_params,
                    "PATCH",
                    "/guilds/333/members/444",
                    "{\"communication_disabled_until\":\"2026-06-15T03:00:00.000000+00:00\",\"nick\":null,\"roles\":[\"555\",\"556\"],\"mute\":true,\"deaf\":false}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "set_guild_member_timeout",
                    call_rest_set_guild_member_timeout,
                    "PATCH",
                    "/guilds/333/members/444",
                    "{\"communication_disabled_until\":\"2026-06-15T03:00:00.000000+00:00\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "clear_guild_member_timeout",
                    call_rest_clear_guild_member_timeout,
                    "PATCH",
                    "/guilds/333/members/444",
                    "{\"communication_disabled_until\":null}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "move_guild_member",
                    call_rest_move_guild_member,
                    "PATCH",
                    "/guilds/333/members/444",
                    "{\"channel_id\":\"222\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "disconnect_guild_member_voice",
                    call_rest_disconnect_guild_member_voice,
                    "PATCH",
                    "/guilds/333/members/444",
                    "{\"channel_id\":null}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "remove_guild_member",
                    call_rest_remove_guild_member,
                    "DELETE",
                    "/guilds/333/members/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "add_guild_member_role",
                    call_rest_add_guild_member_role,
                    "PUT",
                    "/guilds/333/members/444/roles/555",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "add_guild_member_role_params",
                    call_rest_add_guild_member_role_params,
                    "PUT",
                    "/guilds/333/members/444/roles/555",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "remove_guild_member_role",
                    call_rest_remove_guild_member_role,
                    "DELETE",
                    "/guilds/333/members/444/roles/555",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_roles",
                    call_rest_get_guild_roles,
                    "GET",
                    "/guilds/333/roles",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_role",
                    call_rest_create_guild_role,
                    "POST",
                    "/guilds/333/roles",
                    "{\"name\":\"ops\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_role_params",
                    call_rest_create_guild_role_params,
                    "POST",
                    "/guilds/333/roles",
                    "{\"name\":\"ops typed\",\"color\":1122867,\"position\":2,\"permissions\":\"2048\",\"hoist\":true,\"mentionable\":false,\"icon\":\"data:image/png;base64,AAAA\",\"unicode_emoji\":\"spark\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_role",
                    call_rest_modify_guild_role,
                    "PATCH",
                    "/guilds/333/roles/555",
                    "{\"name\":\"ops2\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_role_params",
                    call_rest_modify_guild_role_params,
                    "PATCH",
                    "/guilds/333/roles/555",
                    "{\"id\":\"555\",\"name\":\"ops edited\",\"color\":4478310,\"position\":3,\"permissions\":\"3072\",\"hoist\":false,\"mentionable\":true}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_role_positions",
                    call_rest_modify_guild_role_positions,
                    "PATCH",
                    "/guilds/333/roles",
                    "[{\"id\":\"555\",\"position\":1}]"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_role_positions_params",
                    call_rest_modify_guild_role_positions_params,
                    "PATCH",
                    "/guilds/333/roles",
                    "[{\"id\":\"555\",\"position\":1},{\"id\":\"556\",\"position\":2}]"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_guild_role",
                    call_rest_delete_guild_role,
                    "DELETE",
                    "/guilds/333/roles/555",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_application_role_connection_metadata",
                    call_rest_get_application_role_connection_metadata,
                    "GET",
                    "/applications/123/role-connections/metadata",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "update_application_role_connection_metadata",
                    call_rest_update_application_role_connection_metadata,
                    "PUT",
                    "/applications/123/role-connections/metadata",
                    "[{\"type\":1,\"key\":\"score\",\"name\":\"Score\",\"description\":\"Rank score\"}]"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "update_application_role_connection_metadata_params",
                    call_rest_update_application_role_connection_metadata_params,
                    "PUT",
                    "/applications/123/role-connections/metadata",
                    "[{\"type\":1,\"key\":\"score\",\"name\":\"Score\",\"name_localizations\":{\"ko\":\"Score KO\"},\"description\":\"Rank score\",\"description_localizations\":{\"ko\":\"Rank score KO\"}}]"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_current_user_application_role_connection",
                    call_rest_get_current_user_application_role_connection,
                    "GET",
                    "/users/@me/applications/123/role-connection",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "update_current_user_application_role_connection",
                    call_rest_update_current_user_application_role_connection,
                    "PUT",
                    "/users/@me/applications/123/role-connection",
                    "{\"platform_name\":\"dcc\",\"metadata\":{\"score\":\"42\"}}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "update_current_user_application_role_connection_params",
                    call_rest_update_current_user_application_role_connection_params,
                    "PUT",
                    "/users/@me/applications/123/role-connection",
                    "{\"platform_name\":\"dcc\",\"platform_username\":\"octo\",\"metadata\":{\"score\":\"42\",\"tier\":\"gold\"}}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_thread",
                    call_rest_get_thread,
                    "GET",
                    "/channels/999",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_thread",
                    call_rest_create_thread,
                    "POST",
                    "/channels/222/threads",
                    "{\"name\":\"ops\",\"auto_archive_duration\":60,\"type\":11}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_thread_params",
                    call_rest_create_thread_params,
                    "POST",
                    "/channels/222/threads",
                    "{\"name\":\"ops typed\",\"auto_archive_duration\":60,\"type\":12,\"invitable\":true,\"rate_limit_per_user\":5}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_forum_thread_params",
                    call_rest_create_forum_thread_params,
                    "POST",
                    "/channels/222/threads",
                    "{\"name\":\"forum typed\",\"rate_limit_per_user\":3,\"message\":{\"content\":\"forum starter\"},\"applied_tags\":[\"901\",\"902\"],\"auto_archive_duration\":1440}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_thread_from_message",
                    call_rest_create_thread_from_message,
                    "POST",
                    "/channels/222/messages/777/threads",
                    "{\"name\":\"ops-msg\",\"auto_archive_duration\":60}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_thread_from_message_params",
                    call_rest_create_thread_from_message_params,
                    "POST",
                    "/channels/222/messages/777/threads",
                    "{\"name\":\"ops-msg typed\",\"auto_archive_duration\":4320,\"rate_limit_per_user\":4}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_thread",
                    call_rest_modify_thread,
                    "PATCH",
                    "/channels/999",
                    "{\"archived\":true}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_thread_params",
                    call_rest_modify_thread_params,
                    "PATCH",
                    "/channels/999",
                    "{\"name\":\"renamed\",\"archived\":true,\"auto_archive_duration\":10080,\"locked\":true,\"invitable\":false,\"rate_limit_per_user\":6,\"applied_tags\":[\"901\"]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "join_thread",
                    call_rest_join_thread,
                    "PUT",
                    "/channels/999/thread-members/@me",
                    NULL
                ) != 0) {
        return 1;
    }
    return 0;
}
#endif
