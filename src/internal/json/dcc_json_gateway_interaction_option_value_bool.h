#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_VALUE_BOOL_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_VALUE_BOOL_H

#include "internal/json/dcc_json_core.h"

dcc_status_t dcc_json_gateway_finalize_interaction_option_bool_value(
    dcc_interaction_option_t *option,
    int bool_value
);

#endif
