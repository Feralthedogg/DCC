#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_guild_channel_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->gateway.channel_ids = out->gateway_channel_ids;
    out->gateway.channel_ids_count = stored;
    out->gateway.channel_ids_truncated = truncated;
    out->guild.channels = out->gateway_guild_channels;
    out->guild.channels_count = stored;
    out->guild.channels_truncated = truncated;
    out->has_guild = 1;
}
