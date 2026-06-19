#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_scheduled_event_resource(dcc_json_gateway_payload_t *out) {
    if (out->scheduled_event.guild_id == 0 && out->gateway.guild_id != 0) {
        out->scheduled_event.guild_id = out->gateway.guild_id;
    }
    if (out->scheduled_event.channel_id == 0 && out->gateway.channel_id != 0) {
        out->scheduled_event.channel_id = out->gateway.channel_id;
    }
    if (out->scheduled_event.id == 0 && out->gateway.scheduled_event_id != 0) {
        out->scheduled_event.id = out->gateway.scheduled_event_id;
    }
    if (out->scheduled_event.creator_id == 0 && out->scheduled_event.creator.id != 0) {
        out->scheduled_event.creator_id = out->scheduled_event.creator.id;
    }
    if (out->scheduled_event.creator.id == 0 && out->scheduled_event.creator_id != 0) {
        out->scheduled_event.creator.id = out->scheduled_event.creator_id;
    }
}
