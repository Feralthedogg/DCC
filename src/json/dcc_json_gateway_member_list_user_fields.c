#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_member_list_user_field(
    dcc_json_parser_t *parser,
    dcc_member_t *member,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_member_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || member == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_USER) {
        *handled = 1;
        return dcc_json_gateway_parse_user_object(
            parser,
            &member->user,
            buffers->username,
            buffers->username_cap,
            buffers->global_name,
            buffers->global_name_cap,
            buffers->user_avatar,
            buffers->user_avatar_cap
        );
    }

    if (key_id == DCC_JSON_KEY_ROLES) {
        *handled = 1;
        return dcc_json_parse_snowflake_array(
            parser,
            buffers->role_ids,
            DCC_JSON_GATEWAY_ID_LIST_CAP,
            &member->role_ids,
            &member->role_ids_count,
            &member->role_ids_truncated
        );
    }

    *handled = 0;
    return DCC_OK;
}
