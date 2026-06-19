#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_resolved_role_map(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->interaction.resolved.roles = out->interaction_resolved_roles;
    out->interaction.resolved.roles_count = stored;
    out->interaction.resolved.roles_truncated = truncated;
    out->has_interaction = 1;
}
