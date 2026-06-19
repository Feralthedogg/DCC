#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_update_events(dcc_json_gateway_payload_t *out) {
    dcc_json_gateway_normalize_update_core_events(out);
    dcc_json_gateway_normalize_update_guild_events(out);
    dcc_json_gateway_normalize_update_channel_events(out);
}
