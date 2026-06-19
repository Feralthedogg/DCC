#ifndef DCC_JSON_GATEWAY_STAGE_INSTANCE_BUFFERS_H
#define DCC_JSON_GATEWAY_STAGE_INSTANCE_BUFFERS_H

#include <stddef.h>

typedef struct dcc_json_gateway_stage_instance_buffers {
    char *topic;
    size_t topic_cap;
} dcc_json_gateway_stage_instance_buffers_t;

#endif
