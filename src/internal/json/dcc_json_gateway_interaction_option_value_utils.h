#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_VALUE_UTILS_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_VALUE_UTILS_H

#include <stddef.h>
#include <stdint.h>

int dcc_json_gateway_interaction_option_type_is_snowflake(uint32_t type);
int dcc_json_gateway_parse_double_ascii(
    const char *data,
    size_t len,
    double *out
);

#endif
