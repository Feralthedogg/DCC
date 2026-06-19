#include "internal/json/dcc_json_internal.h"

#include <string.h>

void dcc_json_gateway_prepare_member_list_object(
    dcc_member_t *member,
    dcc_snowflake_t guild_id,
    dcc_permission_t *permissions,
    int *has_permissions
) {
    memset(member, 0, sizeof(*member));
    if (permissions != NULL) {
        *permissions = 0;
    }
    if (has_permissions != NULL) {
        *has_permissions = 0;
    }
    member->guild_id = guild_id;
}
