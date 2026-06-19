#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_object_collection_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_json_gateway_parse_d_media_collection_key(
        parser,
        out,
        key_id,
        handled
    );
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_d_guild_collection_key(
        parser,
        out,
        key_id,
        handled
    );
    if (status != DCC_OK) {
        return status;
    }

    return DCC_OK;
}
