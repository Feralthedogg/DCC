#include "internal/json/dcc_json_internal.h"

static void dcc_json_gateway_role_scalar_cache_or(uint64_t *cache_fields, uint64_t field) {
    if (cache_fields != NULL) {
        *cache_fields |= field;
    }
}

dcc_status_t dcc_json_gateway_parse_role_scalar_field(
    dcc_json_parser_t *parser,
    dcc_role_t *role,
    dcc_json_key_id_t key_id,
    uint64_t *cache_fields,
    int *handled
) {
    if (parser == NULL || role == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_PERMISSIONS) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        role->permissions = has_value ? value : 0;
        dcc_json_gateway_role_scalar_cache_or(cache_fields, DCC_CACHE_FIELD_ROLE_PERMISSIONS);
        *handled = 1;
        return DCC_OK;
    }

    uint64_t cache_field = 0;
    uint32_t *slot = NULL;
    if (key_id == DCC_JSON_KEY_COLOR) {
        slot = &role->color;
        cache_field = DCC_CACHE_FIELD_ROLE_COLOR;
    } else if (key_id == DCC_JSON_KEY_POSITION) {
        slot = &role->position;
        cache_field = DCC_CACHE_FIELD_ROLE_POSITION;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    uint32_t value = 0;
    dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    *slot = value;
    dcc_json_gateway_role_scalar_cache_or(cache_fields, cache_field);
    *handled = 1;
    return DCC_OK;
}
