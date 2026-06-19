#ifndef DCC_JSON_GATEWAY_MEMBER_LIST_OBJECT_PREPARE_H
#define DCC_JSON_GATEWAY_MEMBER_LIST_OBJECT_PREPARE_H

#include "internal/json/dcc_json_core.h"

void dcc_json_gateway_prepare_member_list_object(
    dcc_member_t *member,
    dcc_snowflake_t guild_id,
    dcc_permission_t *permissions,
    int *has_permissions
);

#endif
