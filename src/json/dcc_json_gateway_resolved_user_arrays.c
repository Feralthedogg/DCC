#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_resolved_user_map(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->interaction.resolved.users = out->interaction_resolved_users;
    out->interaction.resolved.users_count = stored;
    out->interaction.resolved.users_truncated = truncated;
    out->has_interaction = 1;
}
