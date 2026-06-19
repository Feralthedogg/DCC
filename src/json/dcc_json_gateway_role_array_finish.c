#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_guild_role_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->gateway.role_ids = out->gateway_role_ids;
    out->gateway.role_ids_count = stored;
    out->gateway.role_ids_truncated = truncated;
    out->guild.roles = out->gateway_guild_roles;
    out->guild.roles_count = stored;
    out->guild.roles_truncated = truncated;
    out->has_guild = 1;
}
