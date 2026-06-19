#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_role_field(
    dcc_json_parser_t *parser,
    dcc_role_t *role,
    dcc_json_key_id_t key_id,
    char *name,
    size_t name_cap,
    uint64_t *cache_fields,
    int *handled
) {
    if (parser == NULL || role == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_status_t status = dcc_json_gateway_parse_role_id_field(
        parser,
        role,
        key_id,
        handled
    );
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_role_text_field(
        parser,
        role,
        key_id,
        name,
        name_cap,
        cache_fields,
        handled
    );
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_role_scalar_field(
        parser,
        role,
        key_id,
        cache_fields,
        handled
    );
    if (status != DCC_OK || *handled) {
        return status;
    }

    return dcc_json_gateway_parse_role_bool_field(
        parser,
        role,
        key_id,
        cache_fields,
        handled
    );
}
