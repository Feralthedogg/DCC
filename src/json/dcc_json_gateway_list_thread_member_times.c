#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_thread_member_time_field(
    dcc_json_parser_t *parser,
    dcc_thread_member_t *member,
    dcc_json_key_id_t key_id,
    char *join_timestamp,
    size_t join_timestamp_cap,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_JOIN_TIMESTAMP) {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        join_timestamp,
        join_timestamp_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    member->join_timestamp = has_value ? join_timestamp : NULL;
    *handled = 1;
    return DCC_OK;
}
