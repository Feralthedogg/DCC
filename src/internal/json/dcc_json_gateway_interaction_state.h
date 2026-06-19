#ifndef DCC_JSON_GATEWAY_INTERACTION_STATE_H
#define DCC_JSON_GATEWAY_INTERACTION_STATE_H

typedef struct dcc_json_gateway_interaction_option_state {
    char *name;
    char *string_value;
    int has_string;
    int has_bool;
    int bool_value;
    int has_number;
    int number_is_integer;
    char number_value[64];
} dcc_json_gateway_interaction_option_state_t;

#endif
