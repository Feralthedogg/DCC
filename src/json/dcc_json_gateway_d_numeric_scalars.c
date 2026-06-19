#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_numeric_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_json_gateway_parse_d_member_numeric_scalar_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_scheduled_event_numeric_scalar_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_moderation_numeric_scalar_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_invite_numeric_scalar_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_channel_numeric_scalar_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_voice_presence_numeric_scalar_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    *handled = 0;
    return DCC_OK;
}
