#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_VALUE_FINALIZE_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_VALUE_FINALIZE_H

#include "internal/json/dcc_json_core.h"

dcc_status_t dcc_json_gateway_finalize_interaction_option_value(
    dcc_interaction_option_t *option,
    int has_string,
    const char *string_value,
    int has_bool,
    int bool_value,
    int has_number,
    const char *number_value,
    int number_is_integer
);

#endif
