#ifndef DCC_JSON_GATEWAY_D_CHANNEL_NUMERIC_SCALAR_APPLY_H
#define DCC_JSON_GATEWAY_D_CHANNEL_NUMERIC_SCALAR_APPLY_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"
#include "internal/json/dcc_json_keys.h"

#include <stdint.h>

dcc_status_t dcc_json_gateway_apply_d_channel_numeric_scalar_key(
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    uint32_t value,
    int *handled
);

#endif
