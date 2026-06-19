#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_resource_events(dcc_json_gateway_payload_t *out) {
    dcc_json_gateway_normalize_scheduled_event_resource(out);
    dcc_json_gateway_normalize_commerce_resources(out);
    dcc_json_gateway_normalize_invite_stage_resources(out);
    dcc_json_gateway_normalize_soundboard_resource(out);
}
