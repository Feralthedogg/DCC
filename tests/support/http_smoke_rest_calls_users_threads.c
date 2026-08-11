#include "http_smoke_support.h"

#if !defined(_WIN32)

dcc_status_t call_rest_leave_guild(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_leave_guild(client, 333, cb, user_data);
}
dcc_status_t call_rest_add_group_dm_recipient(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_add_group_dm_recipient(client, 222, 444, "{\"access_token\":\"tok\",\"nick\":\"feral\"}", cb, user_data);
}
dcc_status_t call_rest_add_group_dm_recipient_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_group_dm_recipient_params_t params = {
        .size = sizeof(params),
        .channel_id = 222,
        .user_id = 444,
        .access_token = "tok2",
        .nick = "feral2"
    };
    return dcc_rest_add_group_dm_recipient_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_remove_group_dm_recipient(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_remove_group_dm_recipient(client, 222, 444, cb, user_data);
}
dcc_status_t call_rest_get_current_user_voice_state(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_current_user_voice_state(client, 333, cb, user_data);
}
dcc_status_t call_rest_modify_current_user_voice_state(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_current_user_voice_state(client, 333, "{\"channel_id\":\"222\",\"suppress\":false}", cb, user_data);
}
dcc_status_t call_rest_modify_current_user_voice_state_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_voice_state_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .channel_id = 222,
        .suppress = 0,
        .request_to_speak_timestamp = "2026-06-16T00:00:00.000000+00:00"
    };
    return dcc_rest_modify_current_user_voice_state_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_get_user_voice_state(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_user_voice_state(client, 333, 444, cb, user_data);
}
dcc_status_t call_rest_modify_user_voice_state(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_user_voice_state(client, 333, 444, "{\"channel_id\":\"222\",\"suppress\":true}", cb, user_data);
}
dcc_status_t call_rest_modify_user_voice_state_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_voice_state_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .user_id = 444,
        .channel_id = 222,
        .suppress = 1
    };
    return dcc_rest_modify_user_voice_state_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_get_voice_regions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_voice_regions(client, cb, user_data);
}
dcc_status_t call_rest_get_guild_voice_regions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_voice_regions(client, 333, cb, user_data);
}
dcc_status_t call_rest_get_skus(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_skus(client, 123, cb, user_data);
}
dcc_status_t call_rest_get_entitlements(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_entitlements(client, 123, "limit=2&user_id=444", cb, user_data);
}
dcc_status_t call_rest_get_entitlements_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_snowflake_t sku_ids[] = {555, 556};
    return dcc_rest_get_entitlements_page(client, 123, 444, sku_ids, 2, 111, 222, 25, 333, 1, cb, user_data);
}
dcc_status_t call_rest_create_test_entitlement(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_test_entitlement(client, 123, "{\"sku_id\":\"555\",\"owner_id\":\"444\",\"owner_type\":2}", cb, user_data);
}
dcc_status_t call_rest_create_test_entitlement_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_test_entitlement_params_t params = {
        .size = sizeof(params),
        .application_id = 123,
        .sku_id = 555,
        .owner_id = 444,
        .owner_type = 2
    };
    return dcc_rest_create_test_entitlement_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_delete_test_entitlement(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_test_entitlement(client, 123, 666, cb, user_data);
}
dcc_status_t call_rest_consume_entitlement(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_consume_entitlement(client, 123, 666, cb, user_data);
}
dcc_status_t call_rest_get_gateway_bot(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_gateway_bot(client, cb, user_data);
}
dcc_status_t call_rest_get_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_member(client, 333, 444, cb, user_data);
}
dcc_status_t call_rest_list_guild_members(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_list_guild_members(client, 333, "limit=2&after=444", cb, user_data);
}
dcc_status_t call_rest_list_guild_members_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_list_guild_members_page(client, 333, 2, 444, cb, user_data);
}
dcc_status_t call_rest_search_guild_members(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_search_guild_members(client, 333, "query=feral&limit=1", cb, user_data);
}
dcc_status_t call_rest_search_guild_members_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_search_guild_members_page(client, 333, "feral ops", 1, cb, user_data);
}
dcc_status_t call_rest_add_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_add_guild_member(client, 333, 444, "{\"access_token\":\"tok\"}", cb, user_data);
}
dcc_status_t call_rest_add_guild_member_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_snowflake_t roles[] = {555, 556};
    const dcc_guild_member_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .user_id = 444,
        .flags = DCC_GUILD_MEMBER_UPDATE_NICK | DCC_GUILD_MEMBER_UPDATE_ROLES,
        .nick = "feral typed",
        .roles = roles,
        .role_count = sizeof(roles) / sizeof(roles[0]),
        .access_token = "tok2"
    };
    return dcc_rest_add_guild_member_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_modify_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_member(client, 333, 444, "{\"nick\":\"feral\"}", cb, user_data);
}
dcc_status_t call_rest_modify_guild_member_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_snowflake_t roles[] = {555, 556};
    const dcc_guild_member_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .user_id = 444,
        .flags = DCC_GUILD_MEMBER_UPDATE_TIMEOUT |
                 DCC_GUILD_MEMBER_UPDATE_NICK |
                 DCC_GUILD_MEMBER_UPDATE_ROLES |
                 DCC_GUILD_MEMBER_UPDATE_VOICE,
        .communication_disabled_until = "2026-06-15T03:00:00.000000+00:00",
        .nick = NULL,
        .roles = roles,
        .role_count = sizeof(roles) / sizeof(roles[0]),
        .mute = 1,
        .deaf = 0
    };
    return dcc_rest_modify_guild_member_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_set_guild_member_timeout(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_set_guild_member_timeout(
        client,
        333,
        444,
        "2026-06-15T03:00:00.000000+00:00",
        cb,
        user_data
    );
}
dcc_status_t call_rest_clear_guild_member_timeout(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_clear_guild_member_timeout(client, 333, 444, cb, user_data);
}
dcc_status_t call_rest_move_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_move_guild_member(client, 333, 444, 222, cb, user_data);
}
dcc_status_t call_rest_disconnect_guild_member_voice(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_move_guild_member(client, 333, 444, 0, cb, user_data);
}
dcc_status_t call_rest_remove_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_remove_guild_member(client, 333, 444, cb, user_data);
}
dcc_status_t call_rest_add_guild_member_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_add_guild_member_role(client, 333, 444, 555, cb, user_data);
}
dcc_status_t call_rest_add_guild_member_role_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_guild_member_role_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .user_id = 444,
        .role_id = 555
    };
    return dcc_rest_add_guild_member_role_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_remove_guild_member_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_remove_guild_member_role(client, 333, 444, 555, cb, user_data);
}
dcc_status_t call_rest_get_guild_roles(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_roles(client, 333, cb, user_data);
}
dcc_status_t call_rest_create_guild_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_guild_role(client, 333, "{\"name\":\"ops\"}", cb, user_data);
}
dcc_status_t call_rest_create_guild_role_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_role_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .name = "ops typed",
        .color = 1122867,
        .position = 2,
        .permissions = 2048,
        .hoist = 1,
        .mentionable = 0,
        .icon = "data:image/png;base64,AAAA",
        .unicode_emoji = "spark"
    };
    return dcc_rest_create_guild_role_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_modify_guild_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_role(client, 333, 555, "{\"name\":\"ops2\"}", cb, user_data);
}
dcc_status_t call_rest_modify_guild_role_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_role_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .role_id = 555,
        .name = "ops edited",
        .color = 4478310,
        .position = 3,
        .permissions = 3072,
        .hoist = 0,
        .mentionable = 1
    };
    return dcc_rest_modify_guild_role_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_modify_guild_role_positions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_role_positions(client, 333, "[{\"id\":\"555\",\"position\":1}]", cb, user_data);
}
dcc_status_t call_rest_modify_guild_role_positions_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_role_position_t positions[] = {
        { .role_id = 555, .position = 1 },
        { .role_id = 556, .position = 2 }
    };
    return dcc_rest_modify_guild_role_positions_params(
        client,
        333,
        positions,
        sizeof(positions) / sizeof(positions[0]),
        cb,
        user_data
    );
}
dcc_status_t call_rest_delete_guild_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_guild_role(client, 333, 555, cb, user_data);
}
dcc_status_t call_rest_get_application_role_connection_metadata(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_get_application_role_connection_metadata(client, 123, cb, user_data);
}
dcc_status_t call_rest_update_application_role_connection_metadata(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_update_application_role_connection_metadata(
        client,
        123,
        "[{\"type\":1,\"key\":\"score\",\"name\":\"Score\",\"description\":\"Rank score\"}]",
        cb,
        user_data
    );
}
dcc_status_t call_rest_update_application_role_connection_metadata_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_application_role_connection_metadata_params_t metadata[] = {
        {
            .size = sizeof(dcc_application_role_connection_metadata_params_t),
            .type = DCC_APPLICATION_ROLE_CONNECTION_METADATA_INTEGER_LESS_THAN_OR_EQUAL,
            .key = "score",
            .name = "Score",
            .name_localizations_json = "{\"ko\":\"Score KO\"}",
            .description = "Rank score",
            .description_localizations_json = "{\"ko\":\"Rank score KO\"}"
        }
    };
    return dcc_rest_update_application_role_connection_metadata_params(
        client,
        123,
        metadata,
        sizeof(metadata) / sizeof(metadata[0]),
        cb,
        user_data
    );
}
dcc_status_t call_rest_get_current_user_application_role_connection(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_get_current_user_application_role_connection(client, 123, cb, user_data);
}
dcc_status_t call_rest_update_current_user_application_role_connection(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_update_current_user_application_role_connection(
        client,
        123,
        "{\"platform_name\":\"dcc\",\"metadata\":{\"score\":\"42\"}}",
        cb,
        user_data
    );
}
dcc_status_t call_rest_update_current_user_application_role_connection_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_rest_multipart_field_t metadata[] = {
        { .name = "score", .value = "42" },
        { .name = "tier", .value = "gold" }
    };
    const dcc_application_role_connection_params_t params = {
        .size = sizeof(dcc_application_role_connection_params_t),
        .platform_name = "dcc",
        .platform_username = "octo",
        .metadata_fields = metadata,
        .metadata_field_count = sizeof(metadata) / sizeof(metadata[0])
    };
    return dcc_rest_update_current_user_application_role_connection_params(
        client,
        123,
        &params,
        cb,
        user_data
    );
}
dcc_status_t call_rest_get_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_get_channel(client, 999, &options, NULL);
}
dcc_status_t call_rest_create_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
        DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION |
        DCC_THREAD_PARAMS_PRESENT_TYPE;
    params.name = "ops";
    params.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR;
    params.type = DCC_CHANNEL_PUBLIC_THREAD;
    return dcc_rest_create_thread(client, 222, &params, &options, NULL);
}
dcc_status_t call_rest_create_thread_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
        DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION |
        DCC_THREAD_PARAMS_PRESENT_TYPE | DCC_THREAD_PARAMS_PRESENT_INVITABLE |
        DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER;
    params.name = "ops typed";
    params.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR;
    params.type = DCC_CHANNEL_PRIVATE_THREAD;
    params.invitable = 1;
    params.rate_limit_per_user = 5;
    return dcc_rest_create_thread(client, 222, &params, &options, NULL);
}
dcc_status_t call_rest_create_forum_thread_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_snowflake_t tags[] = {901, 902};
    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    dcc_status_t status = dcc_message_builder_set_content(&message, "forum starter");
    payload.message = &message;
    params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
        DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION |
        DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER |
        DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS |
        DCC_THREAD_PARAMS_PRESENT_MESSAGE;
    params.name = "forum typed";
    params.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY;
    params.rate_limit_per_user = 3;
    params.applied_tags = tags;
    params.applied_tag_count = sizeof(tags) / sizeof(tags[0]);
    params.message = &payload;
    return status == DCC_OK
        ? dcc_rest_create_forum_thread(client, 222, &params, &options, NULL)
        : status;
}
dcc_status_t call_rest_create_thread_from_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
        DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION;
    params.name = "ops-msg";
    params.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR;
    return dcc_rest_create_thread_from_message(client, 222, 777, &params, &options, NULL);
}
dcc_status_t call_rest_create_thread_from_message_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
        DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION |
        DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER;
    params.name = "ops-msg typed";
    params.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_3_DAYS;
    params.rate_limit_per_user = 4;
    return dcc_rest_create_thread_from_message(client, 222, 777, &params, &options, NULL);
}
dcc_status_t call_rest_modify_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.kind = DCC_CHANNEL_PARAMS_THREAD;
    params.payload.thread.present = DCC_CHANNEL_THREAD_PRESENT_ARCHIVED;
    params.payload.thread.archived = 1;
    return dcc_rest_modify_channel(client, 999, &params, &options, NULL);
}
dcc_status_t call_rest_modify_thread_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_snowflake_t tags[] = {901};
    dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.kind = DCC_CHANNEL_PARAMS_THREAD;
    params.payload.thread.present = DCC_CHANNEL_THREAD_PRESENT_NAME |
        DCC_CHANNEL_THREAD_PRESENT_ARCHIVED |
        DCC_CHANNEL_THREAD_PRESENT_AUTO_ARCHIVE_DURATION |
        DCC_CHANNEL_THREAD_PRESENT_LOCKED |
        DCC_CHANNEL_THREAD_PRESENT_INVITABLE |
        DCC_CHANNEL_THREAD_PRESENT_RATE_LIMIT_PER_USER |
        DCC_CHANNEL_THREAD_PRESENT_APPLIED_TAGS;
    params.payload.thread.name = "renamed";
    params.payload.thread.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_WEEK;
    params.payload.thread.invitable = 0;
    params.payload.thread.rate_limit_per_user = 6;
    params.payload.thread.archived = 1;
    params.payload.thread.locked = 1;
    params.payload.thread.applied_tags = tags;
    params.payload.thread.applied_tag_count = sizeof(tags) / sizeof(tags[0]);
    return dcc_rest_modify_channel(client, 999, &params, &options, NULL);
}
dcc_status_t call_rest_join_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_join_thread(client, 999, &options, NULL);
}

#endif
