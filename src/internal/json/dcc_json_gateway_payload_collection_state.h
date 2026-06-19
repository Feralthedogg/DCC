#ifndef DCC_JSON_GATEWAY_PAYLOAD_COLLECTION_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_COLLECTION_STATE_H

#include "internal/json/dcc_json_gateway_payload_activity_collection_state.h"
#include "internal/json/dcc_json_gateway_payload_guild_collection_state.h"
#include "internal/json/dcc_json_gateway_payload_list_collection_state.h"
#include "internal/json/dcc_json_gateway_payload_member_collection_state.h"
#include "internal/json/dcc_json_gateway_payload_presence_collection_state.h"

#define DCC_JSON_GATEWAY_PAYLOAD_COLLECTION_FIELDS \
    DCC_JSON_GATEWAY_PAYLOAD_ACTIVITY_COLLECTION_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_GUILD_COLLECTION_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_PRESENCE_COLLECTION_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_LIST_COLLECTION_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_MEMBER_COLLECTION_FIELDS

#endif
