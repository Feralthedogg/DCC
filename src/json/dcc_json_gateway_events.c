#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_apply_id_alias_from_event(dcc_json_gateway_payload_t *out) {
    if (!out->has_event_name || out->gateway.id == 0) {
        return;
    }

    const char *event_name = out->event_name;
    switch (event_name[0]) {
        case 'A':
            dcc_json_gateway_apply_auto_moderation_id_alias(out, event_name);
            return;
        case 'E':
            dcc_json_gateway_apply_entitlement_id_alias(out, event_name);
            return;
        case 'G':
            dcc_json_gateway_apply_guild_id_alias(out, event_name);
            return;
        default:
            return;
    }
}
