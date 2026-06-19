#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_emoji_list_bool_field(
    dcc_json_parser_t *parser,
    dcc_emoji_t *emoji,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_REQUIRE_COLONS &&
        key_id != DCC_JSON_KEY_MANAGED &&
        key_id != DCC_JSON_KEY_ANIMATED &&
        key_id != DCC_JSON_KEY_AVAILABLE) {
        *handled = 0;
        return DCC_OK;
    }

    int value = 0;
    dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    if (key_id == DCC_JSON_KEY_REQUIRE_COLONS) {
        emoji->require_colons = value ? 1U : 0U;
    } else if (key_id == DCC_JSON_KEY_MANAGED) {
        emoji->managed = value ? 1U : 0U;
    } else if (key_id == DCC_JSON_KEY_ANIMATED) {
        emoji->animated = value ? 1U : 0U;
    } else {
        emoji->available = value ? 1U : 0U;
    }

    *handled = 1;
    return DCC_OK;
}
