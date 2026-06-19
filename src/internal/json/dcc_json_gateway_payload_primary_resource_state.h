#ifndef DCC_JSON_GATEWAY_PAYLOAD_PRIMARY_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_PRIMARY_RESOURCE_STATE_H

#include "internal/json/dcc_json_gateway_payload_guild_resource_state.h"
#include "internal/json/dcc_json_gateway_payload_base_resource_state.h"
#include "internal/json/dcc_json_gateway_payload_scheduled_resource_state.h"
#include "internal/json/dcc_json_gateway_payload_commerce_resource_state.h"
#include "internal/json/dcc_json_gateway_payload_event_resource_state.h"

#define DCC_JSON_GATEWAY_PAYLOAD_PRIMARY_RESOURCE_FIELDS \
    DCC_JSON_GATEWAY_PAYLOAD_GUILD_RESOURCE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_BASE_RESOURCE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_SCHEDULED_RESOURCE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_COMMERCE_RESOURCE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_EVENT_RESOURCE_FIELDS

#endif
