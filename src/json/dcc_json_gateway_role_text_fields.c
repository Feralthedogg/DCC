#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_role_text_field(
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
    if (key_id != DCC_JSON_KEY_NAME) {
        *handled = 0;
        return DCC_OK;
    }
    if (name == NULL) {
        return DCC_ERR_JSON;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        name,
        name_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    role->name = has_value ? name : NULL;
    if (cache_fields != NULL) {
        *cache_fields |= DCC_CACHE_FIELD_ROLE_NAME;
    }
    *handled = 1;
    return DCC_OK;
}
