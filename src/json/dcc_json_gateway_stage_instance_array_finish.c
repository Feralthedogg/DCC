#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_stage_instance_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->gateway.ids = out->gateway_ids;
    out->gateway.ids_count = stored;
    out->gateway.ids_truncated = truncated;
    out->guild.stage_instances = out->gateway_guild_stage_instances;
    out->guild.stage_instances_count = stored;
    out->guild.stage_instances_truncated = truncated;
    out->has_guild = 1;
}
