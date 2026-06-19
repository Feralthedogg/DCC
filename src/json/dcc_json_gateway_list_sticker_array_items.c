#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_sticker_array_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated
) {
    if (parser == NULL || out == NULL || stored == NULL || truncated == NULL) {
        return DCC_ERR_JSON;
    }

    if (parser->cur < parser->end && *parser->cur == '{' && *stored < DCC_JSON_GATEWAY_OBJECT_LIST_CAP) {
        size_t index = *stored;
        dcc_status_t status = dcc_json_gateway_parse_sticker_list_object(
            parser,
            &out->gateway_stickers[index],
            out->gateway_sticker_names[index],
            sizeof(out->gateway_sticker_names[index]),
            out->gateway_sticker_descriptions[index],
            sizeof(out->gateway_sticker_descriptions[index]),
            out->gateway_sticker_tags[index],
            sizeof(out->gateway_sticker_tags[index]),
            out->gateway_sticker_assets[index],
            sizeof(out->gateway_sticker_assets[index]),
            out->gateway_sticker_usernames[index],
            sizeof(out->gateway_sticker_usernames[index]),
            out->gateway_sticker_user_global_names[index],
            sizeof(out->gateway_sticker_user_global_names[index]),
            out->gateway_sticker_user_avatars[index],
            sizeof(out->gateway_sticker_user_avatars[index])
        );
        if (status != DCC_OK) {
            return status;
        }
        if (out->gateway_stickers[index].id != 0 && index < DCC_JSON_GATEWAY_ID_LIST_CAP) {
            out->gateway_ids[index] = out->gateway_stickers[index].id;
        }
        *stored = index + 1U;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_skip_value(parser);
    if (status != DCC_OK) {
        return status;
    }
    *truncated = 1;
    return DCC_OK;
}
