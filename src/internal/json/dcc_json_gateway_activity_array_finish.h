#ifndef DCC_JSON_GATEWAY_ACTIVITY_ARRAY_FINISH_H
#define DCC_JSON_GATEWAY_ACTIVITY_ARRAY_FINISH_H

#include "internal/json/dcc_json_gateway_activity_buffers.h"

#include <stddef.h>
#include <stdint.h>

void dcc_json_gateway_finish_activity_array(
    const dcc_json_gateway_activity_array_t *array,
    size_t stored,
    uint8_t truncated
);

#endif
