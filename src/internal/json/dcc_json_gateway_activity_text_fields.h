#ifndef DCC_JSON_GATEWAY_ACTIVITY_TEXT_FIELDS_H
#define DCC_JSON_GATEWAY_ACTIVITY_TEXT_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_activity_text_field(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activity,
    dcc_json_key_id_t key_id,
    char *name,
    size_t name_cap,
    char *state,
    size_t state_cap,
    char *details,
    size_t details_cap,
    char *url,
    size_t url_cap,
    int *handled
);

#endif
