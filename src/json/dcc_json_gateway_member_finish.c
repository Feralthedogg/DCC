#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_member_object(dcc_json_gateway_payload_t *out) {
    if (out == NULL) {
        return;
    }
    out->member.guild_id = out->gateway.guild_id;
    out->has_member = 1;
}
