#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_interaction_option_object(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_interaction_option_t *option,
    size_t list_index,
    size_t index,
    unsigned depth
) {
    if (parser == NULL || out == NULL || option == NULL) {
        return DCC_ERR_JSON;
    }
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    char *name = out->interaction_option_names[list_index][index];
    char *string_value = out->interaction_option_strings[list_index][index];
    dcc_json_gateway_interaction_option_state_t state = {
        name,
        string_value,
        0,
        0,
        0,
        0,
        0,
        {0}
    };

    memset(option, 0, sizeof(*option));
    name[0] = '\0';
    string_value[0] = '\0';

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return dcc_json_gateway_finalize_interaction_option_state(option, &state);
    }

    return dcc_json_gateway_parse_interaction_option_object_fields(
        parser,
        out,
        option,
        &state,
        depth
    );
}
