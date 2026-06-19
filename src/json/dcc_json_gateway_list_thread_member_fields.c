#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_thread_member_field(
    dcc_json_parser_t *parser,
    dcc_thread_member_t *member,
    dcc_json_key_id_t key_id,
    char *join_timestamp,
    size_t join_timestamp_cap
) {
    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_thread_member_id_field(
        parser,
        member,
        key_id,
        &handled
    );
    if (status != DCC_OK || handled) {
        return status;
    }

    status = dcc_json_gateway_parse_thread_member_scalar_field(
        parser,
        member,
        key_id,
        &handled
    );
    if (status != DCC_OK || handled) {
        return status;
    }

    status = dcc_json_gateway_parse_thread_member_time_field(
        parser,
        member,
        key_id,
        join_timestamp,
        join_timestamp_cap,
        &handled
    );
    if (status != DCC_OK || handled) {
        return status;
    }

    return dcc_json_skip_value(parser);
}
