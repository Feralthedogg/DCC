#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_sticker_list_id_field(
    dcc_json_parser_t *parser,
    dcc_sticker_t *sticker,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_snowflake_t *target = NULL;
    if (key_id == DCC_JSON_KEY_ID) {
        target = &sticker->id;
    } else if (key_id == DCC_JSON_KEY_PACK_ID) {
        target = &sticker->pack_id;
    } else if (key_id == DCC_JSON_KEY_GUILD_ID) {
        target = &sticker->guild_id;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    uint64_t value = 0;
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }
    *target = has_value ? value : 0U;
    *handled = 1;
    return DCC_OK;
}
