#ifndef DCC_JSON_GATEWAY_ENTITY_METADATA_FIELDS_H
#define DCC_JSON_GATEWAY_ENTITY_METADATA_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

#include <stddef.h>

dcc_status_t dcc_json_parse_entity_metadata_location(
    dcc_json_parser_t *parser,
    char *location,
    size_t location_cap,
    const char **out_location
);
dcc_status_t dcc_json_parse_entity_metadata_location_field(
    dcc_json_parser_t *parser,
    dcc_json_key_id_t key_id,
    char *location,
    size_t location_cap,
    const char **out_location
);

#endif
