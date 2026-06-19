#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_scheduled_event_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->gateway.ids = out->gateway_ids;
    out->gateway.ids_count = stored;
    out->gateway.ids_truncated = truncated;
    out->guild.scheduled_events = out->gateway_guild_scheduled_events;
    out->guild.scheduled_events_count = stored;
    out->guild.scheduled_events_truncated = truncated;
    out->has_guild = 1;
}
