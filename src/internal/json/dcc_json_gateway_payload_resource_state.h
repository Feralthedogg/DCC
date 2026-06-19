#ifndef DCC_JSON_GATEWAY_PAYLOAD_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_RESOURCE_STATE_H

#include "internal/json/dcc_json_gateway_payload_message_resource_state.h"
#include "internal/json/dcc_json_gateway_payload_primary_resource_state.h"
#include "internal/json/dcc_json_gateway_payload_update_resource_state.h"

#define DCC_JSON_GATEWAY_PAYLOAD_RESOURCE_FIELDS \
    DCC_JSON_GATEWAY_PAYLOAD_MESSAGE_RESOURCE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_PRIMARY_RESOURCE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_UPDATE_RESOURCE_FIELDS

#endif
