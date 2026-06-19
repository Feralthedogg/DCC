#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_identity_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    dcc_status_t status = DCC_OK;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    status = dcc_json_gateway_parse_d_core_session_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }
    status = dcc_json_gateway_parse_d_core_id_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }
    status = dcc_json_gateway_parse_d_core_channel_id_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }
    status = dcc_json_gateway_parse_d_core_guild_id_key(parser, out, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }
    return dcc_json_gateway_parse_d_core_webhook_id_key(parser, out, key_id, handled);
}
