#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_guild_limit_value_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_json_gateway_parse_d_core_guild_capacity_limit_key(
        parser,
        out,
        key_id,
        handled
    );
    if (status != DCC_OK || *handled) {
        return status;
    }

    return dcc_json_gateway_parse_d_core_guild_policy_limit_key(
        parser,
        out,
        key_id,
        handled
    );
}
