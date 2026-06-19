#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_guild_presence_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->gateway.ids = out->gateway_ids;
    out->gateway.ids_count = stored;
    out->gateway.ids_truncated = truncated;
    out->guild.presences = out->gateway_guild_presences;
    out->guild.presences_count = stored;
    out->guild.presences_truncated = truncated;
    out->has_guild = 1;
}
