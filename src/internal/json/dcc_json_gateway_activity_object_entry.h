#ifndef DCC_JSON_GATEWAY_ACTIVITY_OBJECT_ENTRY_H
#define DCC_JSON_GATEWAY_ACTIVITY_OBJECT_ENTRY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_activity_object(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activity,
    char *name,
    size_t name_cap,
    char *state,
    size_t state_cap,
    char *details,
    size_t details_cap,
    char *url,
    size_t url_cap,
    char *emoji_name,
    size_t emoji_name_cap
);

#endif
