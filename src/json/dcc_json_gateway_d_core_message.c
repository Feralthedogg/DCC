#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_message_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_json_gateway_parse_d_core_message_text_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_core_message_components_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_core_message_bool_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_core_message_scheduled_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_core_message_flags_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    return dcc_json_gateway_parse_d_core_message_author_key(parser, out, key_id, handled);
}
