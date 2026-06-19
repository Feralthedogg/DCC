#ifndef DCC_JSON_GATEWAY_THREAD_LIST_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_THREAD_LIST_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resource_buffers.h"

dcc_status_t dcc_json_gateway_parse_thread_list_object_fields(
    dcc_json_parser_t *parser,
    dcc_channel_t *thread,
    const dcc_json_gateway_thread_buffers_t *buffers
);

#endif
