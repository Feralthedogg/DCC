#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_init_member_list_buffers(
    dcc_json_gateway_member_buffers_t *buffers,
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
    if (buffers == NULL || role_ids == NULL || username == NULL ||
        global_name == NULL || user_avatar == NULL || nick == NULL ||
        avatar == NULL || joined_at == NULL || premium_since == NULL ||
        communication_disabled_until == NULL) {
        return DCC_ERR_JSON;
    }

    *buffers = (dcc_json_gateway_member_buffers_t){
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
    };
    return DCC_OK;
}
