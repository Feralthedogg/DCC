#ifndef DCC_JSON_GATEWAY_THREAD_LIST_OBJECT_ENTRY_H
#define DCC_JSON_GATEWAY_THREAD_LIST_OBJECT_ENTRY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_thread_list_object(
    dcc_json_parser_t *parser,
    dcc_channel_t *thread,
    char *name,
    size_t name_cap,
    char *topic,
    size_t topic_cap
);

#endif
