#ifndef DCC_JSON_GATEWAY_STAGE_INSTANCE_BUFFER_INIT_H
#define DCC_JSON_GATEWAY_STAGE_INSTANCE_BUFFER_INIT_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resource_buffers.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_init_stage_instance_buffers(
    dcc_json_gateway_stage_instance_buffers_t *buffers,
    char *topic,
    size_t topic_cap
);

#endif
