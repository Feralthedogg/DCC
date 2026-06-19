#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_activity_emoji_field(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activity,
    dcc_json_key_id_t key_id,
    char *emoji_name,
    size_t emoji_name_cap
) {
    if (key_id == DCC_JSON_KEY_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        activity->emoji_id = has_value ? value : 0;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_NAME) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            emoji_name,
            emoji_name_cap,
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        activity->emoji_name = has_value ? emoji_name : NULL;
        return DCC_OK;
    }

    return dcc_json_skip_value(parser);
}
