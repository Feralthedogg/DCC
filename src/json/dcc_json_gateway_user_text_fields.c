#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_user_text_field(
    dcc_json_parser_t *parser,
    dcc_user_t *user,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_user_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || user == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    char *target = NULL;
    size_t target_cap = 0;
    const char **field = NULL;

    if (key_id == DCC_JSON_KEY_USERNAME) {
        target = buffers->username;
        target_cap = buffers->username_cap;
        field = &user->username;
    } else if (key_id == DCC_JSON_KEY_GLOBAL_NAME) {
        target = buffers->global_name;
        target_cap = buffers->global_name_cap;
        field = &user->global_name;
    } else if (key_id == DCC_JSON_KEY_AVATAR) {
        target = buffers->avatar;
        target_cap = buffers->avatar_cap;
        field = &user->avatar;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    if (target == NULL || target_cap == 0U) {
        return dcc_json_skip_value(parser);
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        target,
        target_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }

    *field = has_value ? target : NULL;
    return DCC_OK;
}
