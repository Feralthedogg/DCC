#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_member_list_time_field(
    dcc_json_parser_t *parser,
    dcc_member_t *member,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_member_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || member == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    char *target = NULL;
    size_t target_cap = 0;
    const char **field = NULL;

    if (key_id == DCC_JSON_KEY_JOINED_AT) {
        target = buffers->joined_at;
        target_cap = buffers->joined_at_cap;
        field = &member->joined_at;
    } else if (key_id == DCC_JSON_KEY_PREMIUM_SINCE) {
        target = buffers->premium_since;
        target_cap = buffers->premium_since_cap;
        field = &member->premium_since;
    } else if (key_id == DCC_JSON_KEY_COMMUNICATION_DISABLED_UNTIL) {
        target = buffers->communication_disabled_until;
        target_cap = buffers->communication_disabled_until_cap;
        field = &member->communication_disabled_until;
    } else {
        *handled = 0;
        return DCC_OK;
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
