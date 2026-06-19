#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_emoji_array_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated
) {
    if (parser->cur < parser->end && *parser->cur == '{' &&
        *stored < DCC_JSON_GATEWAY_OBJECT_LIST_CAP) {
        dcc_status_t status = dcc_json_gateway_parse_emoji_list_object(
            parser,
            &out->gateway_emojis[*stored],
            out->gateway_emoji_names[*stored],
            sizeof(out->gateway_emoji_names[*stored]),
            out->gateway_emoji_usernames[*stored],
            sizeof(out->gateway_emoji_usernames[*stored]),
            out->gateway_emoji_user_global_names[*stored],
            sizeof(out->gateway_emoji_user_global_names[*stored]),
            out->gateway_emoji_user_avatars[*stored],
            sizeof(out->gateway_emoji_user_avatars[*stored])
        );
        if (status != DCC_OK) {
            return status;
        }
        if (out->gateway_emojis[*stored].id != 0 &&
            *stored < DCC_JSON_GATEWAY_ID_LIST_CAP) {
            out->gateway_ids[*stored] = out->gateway_emojis[*stored].id;
        }
        (*stored)++;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_skip_value(parser);
    if (status == DCC_OK) {
        *truncated = 1;
    }
    return status;
}
