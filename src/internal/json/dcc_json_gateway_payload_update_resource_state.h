#ifndef DCC_JSON_GATEWAY_PAYLOAD_UPDATE_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_UPDATE_RESOURCE_STATE_H

#include "internal/json/dcc_json_gateway_payload_thread_update_resource_state.h"
#include "internal/json/dcc_json_gateway_payload_channel_update_resource_state.h"
#include "internal/json/dcc_json_gateway_payload_identity_update_resource_state.h"
#include "internal/json/dcc_json_gateway_payload_guild_update_resource_state.h"

#define DCC_JSON_GATEWAY_PAYLOAD_UPDATE_RESOURCE_FIELDS \
    DCC_JSON_GATEWAY_PAYLOAD_THREAD_UPDATE_RESOURCE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_CHANNEL_UPDATE_RESOURCE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_IDENTITY_UPDATE_RESOURCE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_GUILD_UPDATE_RESOURCE_FIELDS

#endif
