#include "internal/json/dcc_json_internal.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

int dcc_json_gateway_interaction_option_type_is_snowflake(uint32_t type) {
    return type == 6U || type == 7U || type == 8U || type == 9U || type == 11U;
}

int dcc_json_gateway_parse_double_ascii(const char *data, size_t len, double *out) {
    if (data == NULL || out == NULL || len == 0 || len >= 64U) {
        return -1;
    }

    char buf[64];
    memcpy(buf, data, len);
    buf[len] = '\0';
    errno = 0;
    char *end = NULL;
    double value = strtod(buf, &end);
    if (errno != 0 || end == NULL || *end != '\0') {
        return -1;
    }

    *out = value;
    return 0;
}
