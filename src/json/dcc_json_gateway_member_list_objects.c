#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_member_list_object(
    dcc_json_parser_t *parser,
    dcc_member_t *member,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t *role_ids,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *user_avatar,
    size_t user_avatar_cap,
    char *nick,
    size_t nick_cap,
    char *avatar,
    size_t avatar_cap,
    char *joined_at,
    size_t joined_at_cap,
    char *premium_since,
    size_t premium_since_cap,
    char *communication_disabled_until,
    size_t communication_disabled_until_cap,
    dcc_permission_t *permissions,
    int *has_permissions
) {
    if (parser == NULL || member == NULL) {
        return DCC_ERR_JSON;
    }
    dcc_json_gateway_member_buffers_t buffers;
    dcc_status_t status = dcc_json_gateway_init_member_list_buffers(
        &buffers,
        role_ids,
        username,
        username_cap,
        global_name,
        global_name_cap,
        user_avatar,
        user_avatar_cap,
        nick,
        nick_cap,
        avatar,
        avatar_cap,
        joined_at,
        joined_at_cap,
        premium_since,
        premium_since_cap,
        communication_disabled_until,
        communication_disabled_until_cap,
        permissions,
        has_permissions
    );
    if (status != DCC_OK) {
        return status;
    }

    return dcc_json_gateway_parse_member_list_object_with_buffers(
        parser,
        member,
        guild_id,
        &buffers
    );
}
