#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_resolved_attachment_map(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->interaction.resolved.attachments = out->interaction_resolved_attachments;
    out->interaction.resolved.attachments_count = stored;
    out->interaction.resolved.attachments_truncated = truncated;
    out->has_interaction = 1;
}
