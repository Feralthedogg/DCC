#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_message_events(dcc_json_gateway_payload_t *out) {
    dcc_json_gateway_normalize_message_reaction(out);
    dcc_json_gateway_normalize_poll_vote(out);
}
