#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_options(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    out->interaction.focused_option = NULL;
    dcc_status_t status = dcc_json_gateway_parse_interaction_option_array(
        parser,
        out,
        &out->interaction.options,
        &out->interaction.options_count,
        &out->interaction.options_truncated,
        0U
    );
    if (status == DCC_OK) {
        out->has_interaction = 1;
    }
    return status;
}
