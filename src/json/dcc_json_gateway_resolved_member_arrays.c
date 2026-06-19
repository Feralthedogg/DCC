#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_resolved_member_map(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated,
    uint8_t permissions_truncated
) {
    out->interaction.resolved.members = out->interaction_resolved_members;
    out->interaction.resolved.members_count = stored;
    out->interaction.resolved.members_truncated = truncated;
    if (permissions_truncated) {
        out->interaction.resolved.member_permissions = out->interaction_resolved_member_permissions;
        out->interaction.resolved.member_permissions_truncated = 1;
    }
    out->has_interaction = 1;
}
