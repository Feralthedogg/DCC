#ifndef DCC_JSON_GATEWAY_MEMBER_LIST_BUFFER_INIT_H
#define DCC_JSON_GATEWAY_MEMBER_LIST_BUFFER_INIT_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_member_buffers.h"

#include <stddef.h>

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
);

#endif
