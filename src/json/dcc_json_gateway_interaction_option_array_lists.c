#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_begin_interaction_option_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    const dcc_interaction_option_t **items_out,
    uint8_t *truncated_out,
    unsigned depth,
    size_t *list_index_out,
    dcc_interaction_option_t **items_mut_out
) {
    if (parser == NULL || out == NULL || items_out == NULL ||
        truncated_out == NULL || list_index_out == NULL || items_mut_out == NULL) {
        return DCC_ERR_JSON;
    }

    if (depth >= 4U || out->interaction_option_list_count >= DCC_JSON_GATEWAY_INTERACTION_OPTION_LIST_CAP) {
        dcc_status_t status = dcc_json_skip_value(parser);
        if (status == DCC_OK) {
            *truncated_out = 1;
        }
        return status;
    }

    size_t list_index = out->interaction_option_list_count++;
    dcc_interaction_option_t *items = out->interaction_option_lists[list_index];
    memset(items, 0, sizeof(out->interaction_option_lists[list_index]));

    *items_out = items;
    *list_index_out = list_index;
    *items_mut_out = items;
    return DCC_OK;
}
