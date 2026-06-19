#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_presence_list_field(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_presence_list_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || presence == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_status_t status = dcc_json_gateway_parse_presence_list_identity_field(
        parser,
        presence,
        key_id,
        buffers,
        handled
    );
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_presence_list_status_field(
        parser,
        presence,
        key_id,
        buffers,
        handled
    );
    if (status != DCC_OK || *handled) {
        return status;
    }

    return dcc_json_gateway_parse_presence_list_activity_field(
        parser,
        presence,
        key_id,
        buffers,
        handled
    );
}
