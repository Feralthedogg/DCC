#include <dcc/rest.h>

#define DCC_EXPECT_TASK8_SIGNATURE(name_, ...) \
    typedef dcc_status_t (*name_##_task8_signature_t)(__VA_ARGS__); \
    _Static_assert( \
        _Generic(&(name_), name_##_task8_signature_t: 1, default: 0), \
        #name_ " does not match the exact Task 8 request signature" \
    )

DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_begin_guild_prune, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_prune_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_create_guild_ban, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_guild_ban_create_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_create_guild_soundboard_sound, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_soundboard_sound_create_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_delete_guild_ban, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_delete_guild_integration, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_delete_guild_soundboard_sound, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_current_user_guilds, dcc_client_t *, const dcc_rest_current_user_guilds_query_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_query_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_audit_log, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_audit_log_query_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_ban, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_bans, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_bans_query_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_integrations, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_preview, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_prune_count, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_prune_query_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_soundboard_sound, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_soundboard_sounds, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_vanity_url, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_voice_regions, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_welcome_screen, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_widget, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_widget_json, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_widget_png, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_widget_image_query_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_leave_guild, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_update_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_incident_actions, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_incident_actions_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_soundboard_sound, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_guild_soundboard_sound_update_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_welcome_screen, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_welcome_screen_update_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_widget, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_widget_update_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);

DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_add_guild_member, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_guild_member_add_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_add_guild_member_role, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_bulk_ban_guild_members, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_bulk_ban_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_current_user_guild_member, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_member, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_list_guild_members, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_members_query_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_current_guild_member, dcc_client_t *, dcc_snowflake_t, const dcc_rest_current_guild_member_update_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_member, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_guild_member_update_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_remove_guild_member, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_remove_guild_member_role, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_search_guild_members, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_member_search_query_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_set_current_guild_member_nickname, dcc_client_t *, dcc_snowflake_t, const dcc_rest_current_guild_member_nickname_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);

DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_create_guild_role, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_role_create_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_delete_guild_role, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_role, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_role_member_counts, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_roles, dcc_client_t *, dcc_snowflake_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_role, dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t, const dcc_rest_guild_role_update_t *, const dcc_rest_call_options_t *, dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_role_positions, dcc_client_t *, dcc_snowflake_t, const dcc_rest_guild_role_position_t *, size_t, const dcc_rest_call_options_t *, dcc_rest_request_t **);
