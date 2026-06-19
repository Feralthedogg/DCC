#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_parse_entity_metadata_location_field(
    dcc_json_parser_t *parser,
    dcc_json_key_id_t key_id,
    char *location,
    size_t location_cap,
    const char **out_location
) {
    if (key_id != DCC_JSON_KEY_LOCATION) {
        return dcc_json_skip_value(parser);
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        location,
        location_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    *out_location = has_value ? location : NULL;
    return DCC_OK;
}
