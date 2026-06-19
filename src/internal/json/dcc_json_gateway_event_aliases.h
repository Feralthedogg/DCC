#ifndef DCC_JSON_GATEWAY_EVENT_ALIASES_H
#define DCC_JSON_GATEWAY_EVENT_ALIASES_H

#include "internal/json/dcc_json_gateway_payload_state.h"

void dcc_json_gateway_apply_id_alias_from_event(dcc_json_gateway_payload_t *out);
void dcc_json_gateway_apply_auto_moderation_id_alias(
    dcc_json_gateway_payload_t *out,
    const char *event_name
);
void dcc_json_gateway_apply_entitlement_id_alias(
    dcc_json_gateway_payload_t *out,
    const char *event_name
);
void dcc_json_gateway_apply_guild_id_alias(
    dcc_json_gateway_payload_t *out,
    const char *event_name
);

#endif
