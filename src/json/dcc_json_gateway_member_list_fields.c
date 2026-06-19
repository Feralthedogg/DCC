#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_member_list_field(
    dcc_json_parser_t *parser,
    dcc_member_t *member,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_member_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || member == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_status_t status = dcc_json_gateway_parse_member_list_user_field(
        parser,
        member,
        key_id,
        buffers,
        handled
    );
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_member_list_text_field(parser, member, key_id, buffers, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_member_list_time_field(parser, member, key_id, buffers, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    return dcc_json_gateway_parse_member_list_scalar_field(parser, member, key_id, buffers, handled);
}
