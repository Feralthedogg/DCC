#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_activity_object_field(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activity,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_activity_buffers_t *buffers
) {
    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_activity_text_field(
        parser,
        activity,
        key_id,
        buffers->name,
        buffers->name_cap,
        buffers->state,
        buffers->state_cap,
        buffers->details,
        buffers->details_cap,
        buffers->url,
        buffers->url_cap,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        status = dcc_json_gateway_parse_activity_scalar_field(parser, activity, key_id, &handled);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled && key_id == DCC_JSON_KEY_EMOJI) {
        status = dcc_json_gateway_parse_activity_emoji(
            parser,
            activity,
            buffers->emoji_name,
            buffers->emoji_name_cap
        );
        if (status != DCC_OK) {
            return status;
        }
        handled = 1;
    }
    if (!handled) {
        status = dcc_json_skip_value(parser);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
