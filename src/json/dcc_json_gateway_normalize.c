#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_payload(dcc_json_gateway_payload_t *out) {
    if (out == NULL) {
        return;
    }

    dcc_json_gateway_normalize_identity(out);
    dcc_json_gateway_normalize_resource_events(out);
    dcc_json_gateway_normalize_message_events(out);
    dcc_json_gateway_normalize_moderation_events(out);
    dcc_json_gateway_normalize_thread_events(out);
    dcc_json_gateway_normalize_update_events(out);
    dcc_json_gateway_normalize_guild_collections(out);
    dcc_json_gateway_apply_id_alias_from_event(out);
}
