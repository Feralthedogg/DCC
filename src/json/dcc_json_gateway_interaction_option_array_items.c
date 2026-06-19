#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_array_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_interaction_option_t *items,
    size_t list_index,
    size_t index,
    unsigned depth,
    size_t *count_out,
    uint8_t *truncated_out
) {
    if (parser == NULL || out == NULL || items == NULL ||
        count_out == NULL || truncated_out == NULL) {
        return DCC_ERR_JSON;
    }

    if (index >= DCC_JSON_GATEWAY_INTERACTION_OPTION_CAP) {
        dcc_status_t status = dcc_json_skip_value(parser);
        if (status != DCC_OK) {
            return status;
        }
        *truncated_out = 1;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_gateway_parse_interaction_option_object(
        parser,
        out,
        &items[index],
        list_index,
        index,
        depth
    );
    if (status != DCC_OK) {
        return status;
    }

    *count_out = index + 1U;
    if (items[index].focused && out->interaction.focused_option == NULL) {
        out->interaction.focused_option = &items[index];
    }
    return DCC_OK;
}
