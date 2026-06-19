#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_nullable_string_target(
    dcc_json_parser_t *parser,
    const char **target,
    char *buffer,
    size_t buffer_cap
) {
    if (target == NULL || buffer == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        buffer,
        buffer_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }

    *target = has_value ? buffer : NULL;
    return DCC_OK;
}
