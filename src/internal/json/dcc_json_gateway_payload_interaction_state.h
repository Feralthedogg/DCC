#ifndef DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_STATE_H

#include "internal/json/dcc_json_gateway_payload_interaction_core_state.h"
#include "internal/json/dcc_json_gateway_payload_interaction_option_state.h"
#include "internal/json/dcc_json_gateway_payload_interaction_resolved_member_state.h"
#include "internal/json/dcc_json_gateway_payload_interaction_resolved_resource_state.h"
#include "internal/json/dcc_json_gateway_payload_interaction_resolved_user_state.h"

#define DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_FIELDS \
    DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_CORE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_OPTION_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_RESOLVED_USER_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_RESOLVED_MEMBER_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_RESOLVED_RESOURCE_FIELDS

#endif
