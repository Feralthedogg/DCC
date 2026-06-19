#ifndef DCC_JSON_GATEWAY_THREAD_MEMBER_TIME_FIELDS_H
#define DCC_JSON_GATEWAY_THREAD_MEMBER_TIME_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

#include <dcc/objects.h>

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_thread_member_time_field(
    dcc_json_parser_t *parser,
    dcc_thread_member_t *member,
    dcc_json_key_id_t key_id,
    char *join_timestamp,
    size_t join_timestamp_cap,
    int *handled
);

#endif
