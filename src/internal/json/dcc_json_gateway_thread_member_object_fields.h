#ifndef DCC_JSON_GATEWAY_THREAD_MEMBER_OBJECT_FIELDS_H
#define DCC_JSON_GATEWAY_THREAD_MEMBER_OBJECT_FIELDS_H

#include "internal/json/dcc_json_core.h"

#include <dcc/objects.h>

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_thread_member_list_object(
    dcc_json_parser_t *parser,
    dcc_thread_member_t *member,
    char *join_timestamp,
    size_t join_timestamp_cap
);

#endif
