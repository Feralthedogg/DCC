#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_resolved_message_map(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->interaction.resolved.messages = out->interaction_resolved_messages;
    out->interaction.resolved.messages_count = stored;
    out->interaction.resolved.messages_truncated = truncated;
    out->has_interaction = 1;
}
