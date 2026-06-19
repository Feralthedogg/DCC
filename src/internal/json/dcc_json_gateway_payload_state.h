#ifndef DCC_JSON_GATEWAY_PAYLOAD_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_STATE_H

#include "internal/dcc_cache_fields.h"
#include "internal/json/dcc_json_gateway_payload_collection_state.h"
#include "internal/json/dcc_json_gateway_payload_core_state.h"
#include "internal/json/dcc_json_gateway_payload_gateway_state.h"
#include "internal/json/dcc_json_gateway_payload_interaction_state.h"
#include "internal/json/dcc_json_gateway_payload_limits.h"
#include "internal/json/dcc_json_gateway_payload_resource_state.h"

#include <dcc/objects.h>

#include <stddef.h>
#include <stdint.h>

typedef struct dcc_json_gateway_payload {
    DCC_JSON_GATEWAY_PAYLOAD_CORE_FIELDS;
    DCC_JSON_GATEWAY_PAYLOAD_RESOURCE_FIELDS;
    DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_FIELDS;
    DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_FIELDS;
    DCC_JSON_GATEWAY_PAYLOAD_COLLECTION_FIELDS;
} dcc_json_gateway_payload_t;

#endif
