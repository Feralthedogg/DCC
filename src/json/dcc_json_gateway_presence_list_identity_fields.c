#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_presence_list_identity_field(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_presence_list_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || presence == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_USER) {
        *handled = 1;
        return dcc_json_gateway_parse_user_object(
            parser,
            &presence->user,
            buffers->username,
            buffers->username_cap,
            buffers->global_name,
            buffers->global_name_cap,
            buffers->avatar,
            buffers->avatar_cap
        );
    }

    if (key_id == DCC_JSON_KEY_USER_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        presence->user.id = has_value ? value : 0;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_GUILD_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        presence->guild_id = has_value ? value : 0;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
