#ifndef DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_STATE_H

#include "internal/json/dcc_json_gateway_payload_gateway_core_state.h"
#include "internal/json/dcc_json_gateway_payload_gateway_text_state.h"
#include "internal/json/dcc_json_gateway_payload_gateway_time_state.h"
#include "internal/json/dcc_json_gateway_payload_gateway_media_state.h"
#include "internal/json/dcc_json_gateway_payload_gateway_id_collection_state.h"

#define DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_FIELDS \
    DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_CORE_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_TEXT_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_TIME_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_MEDIA_FIELDS; \
    DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_ID_COLLECTION_FIELDS

#endif
