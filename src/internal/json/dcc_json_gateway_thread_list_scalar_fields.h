#ifndef DCC_JSON_GATEWAY_THREAD_LIST_SCALAR_FIELDS_H
#define DCC_JSON_GATEWAY_THREAD_LIST_SCALAR_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_thread_list_scalar_field(
    dcc_json_parser_t *parser,
    dcc_channel_t *thread,
    dcc_json_key_id_t key_id,
    int *handled
);

#endif
