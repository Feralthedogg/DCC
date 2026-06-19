#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_thread_member_scalar_field(
    dcc_json_parser_t *parser,
    dcc_thread_member_t *member,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_FLAGS) {
        *handled = 0;
        return DCC_OK;
    }

    uint64_t value = 0;
    dcc_status_t status = dcc_json_parse_u64_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    member->flags = value;
    *handled = 1;
    return DCC_OK;
}
