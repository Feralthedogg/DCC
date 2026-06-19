#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_guild_user_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    dcc_status_t status = DCC_OK;
    int local_handled = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    status = dcc_json_gateway_parse_d_core_name_key(parser, out, key_id, &local_handled);
    if (status != DCC_OK || local_handled) {
        *handled = local_handled;
        return status;
    }

    status = dcc_json_gateway_parse_d_core_user_key(parser, out, key_id, &local_handled);
    if (status != DCC_OK || local_handled) {
        *handled = local_handled;
        return status;
    }

    status = dcc_json_gateway_parse_d_core_guild_bool_key(parser, out, key_id, &local_handled);
    if (status != DCC_OK || local_handled) {
        *handled = local_handled;
        return status;
    }

    status = dcc_json_gateway_parse_d_core_guild_limit_key(parser, out, key_id, &local_handled);
    if (status != DCC_OK || local_handled) {
        *handled = local_handled;
        return status;
    }

    status = dcc_json_gateway_parse_d_core_application_type_key(parser, out, key_id, &local_handled);
    if (status != DCC_OK || local_handled) {
        *handled = local_handled;
        return status;
    }

    *handled = 0;
    return DCC_OK;
}
