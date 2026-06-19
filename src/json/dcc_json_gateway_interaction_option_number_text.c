#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_number_text(
    dcc_json_parser_t *parser,
    char *out,
    size_t out_cap,
    int *integer_out
) {
    const char *start = NULL;
    size_t len = 0;
    bool integer = true;
    dcc_status_t status = dcc_json_parse_number_span(parser, &start, &len, &integer);
    if (status != DCC_OK) {
        return status;
    }
    if (out == NULL || out_cap == 0 || len == 0 || len >= out_cap) {
        return DCC_ERR_JSON;
    }

    memcpy(out, start, len);
    out[len] = '\0';
    if (integer_out != NULL) {
        *integer_out = integer ? 1 : 0;
    }
    return DCC_OK;
}
