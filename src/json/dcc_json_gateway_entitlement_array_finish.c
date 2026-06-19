#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_interaction_entitlement_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->interaction.entitlements = stored > 0 ? out->interaction_entitlements : NULL;
    out->interaction.entitlements_count = stored;
    out->interaction.entitlements_truncated = truncated;
    out->has_interaction = 1;
}
