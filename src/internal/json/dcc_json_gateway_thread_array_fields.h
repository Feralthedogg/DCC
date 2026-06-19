#ifndef DCC_JSON_GATEWAY_THREAD_ARRAY_FIELDS_H
#define DCC_JSON_GATEWAY_THREAD_ARRAY_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_thread_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
void dcc_json_gateway_finish_thread_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
);

#endif
