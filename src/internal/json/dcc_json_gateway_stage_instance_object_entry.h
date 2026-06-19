#ifndef DCC_JSON_GATEWAY_STAGE_INSTANCE_OBJECT_ENTRY_H
#define DCC_JSON_GATEWAY_STAGE_INSTANCE_OBJECT_ENTRY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_stage_instance_list_object(
    dcc_json_parser_t *parser,
    dcc_stage_instance_t *stage_instance,
    char *topic,
    size_t topic_cap
);

#endif
