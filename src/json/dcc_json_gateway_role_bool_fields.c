#include "internal/json/dcc_json_internal.h"

static void dcc_json_gateway_role_bool_cache_or(uint64_t *cache_fields, uint64_t field) {
    if (cache_fields != NULL) {
        *cache_fields |= field;
    }
}

dcc_status_t dcc_json_gateway_parse_role_bool_field(
    dcc_json_parser_t *parser,
    dcc_role_t *role,
    dcc_json_key_id_t key_id,
    uint64_t *cache_fields,
    int *handled
) {
    if (parser == NULL || role == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    uint64_t cache_field = 0;
    uint8_t *slot = NULL;
    if (key_id == DCC_JSON_KEY_HOIST) {
        slot = &role->hoist;
        cache_field = DCC_CACHE_FIELD_ROLE_HOIST;
    } else if (key_id == DCC_JSON_KEY_MANAGED) {
        slot = &role->managed;
        cache_field = DCC_CACHE_FIELD_ROLE_MANAGED;
    } else if (key_id == DCC_JSON_KEY_MENTIONABLE) {
        slot = &role->mentionable;
        cache_field = DCC_CACHE_FIELD_ROLE_MENTIONABLE;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    int value = 0;
    dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    *slot = value ? 1U : 0U;
    dcc_json_gateway_role_bool_cache_or(cache_fields, cache_field);
    *handled = 1;
    return DCC_OK;
}
