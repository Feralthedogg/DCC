#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_add_interaction_resolved_permission(
    dcc_json_gateway_payload_t *out,
    dcc_snowflake_t user_id,
    dcc_permission_t permissions
) {
    if (out == NULL || user_id == 0) {
        return;
    }

    for (size_t i = 0; i < out->interaction.resolved.member_permissions_count; i++) {
        if (out->interaction_resolved_member_permissions[i].user_id == user_id) {
            out->interaction_resolved_member_permissions[i].permissions = permissions;
            out->interaction.resolved.member_permissions = out->interaction_resolved_member_permissions;
            out->has_interaction = 1;
            return;
        }
    }

    size_t count = out->interaction.resolved.member_permissions_count;
    if (count >= DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP) {
        out->interaction.resolved.member_permissions = out->interaction_resolved_member_permissions;
        out->interaction.resolved.member_permissions_truncated = 1;
        out->has_interaction = 1;
        return;
    }

    out->interaction_resolved_member_permissions[count].user_id = user_id;
    out->interaction_resolved_member_permissions[count].permissions = permissions;
    out->interaction.resolved.member_permissions = out->interaction_resolved_member_permissions;
    out->interaction.resolved.member_permissions_count = count + 1U;
    out->has_interaction = 1;
}

