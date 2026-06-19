#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_resolved_channel_map(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->interaction.resolved.channels = out->interaction_resolved_channels;
    out->interaction.resolved.channels_count = stored;
    out->interaction.resolved.channels_truncated = truncated;
    out->has_interaction = 1;
}
