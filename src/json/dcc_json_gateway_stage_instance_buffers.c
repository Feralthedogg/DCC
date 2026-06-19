#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_init_stage_instance_buffers(
    dcc_json_gateway_stage_instance_buffers_t *buffers,
    char *topic,
    size_t topic_cap
) {
    if (buffers == NULL || topic == NULL) {
        return DCC_ERR_JSON;
    }

    *buffers = (dcc_json_gateway_stage_instance_buffers_t){
        topic,
        topic_cap
    };
    return DCC_OK;
}
