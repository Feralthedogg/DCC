#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_user_object_value(
    dcc_json_parser_t *parser,
    dcc_user_t *user,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_user_buffers_t *buffers
) {
    if (parser == NULL || user == NULL || buffers == NULL) {
        return DCC_ERR_JSON;
    }

    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_user_id_field(
        parser,
        user,
        key_id,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        status = dcc_json_gateway_parse_user_text_field(
            parser,
            user,
            key_id,
            buffers,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_user_flag_field(parser, user, key_id, &handled);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_skip_value(parser);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
