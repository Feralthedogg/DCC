#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_thread_list_scalar_field(
    dcc_json_parser_t *parser,
    dcc_channel_t *thread,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_NSFW) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        thread->nsfw = value ? 1U : 0U;
    } else if (key_id == DCC_JSON_KEY_TYPE ||
               key_id == DCC_JSON_KEY_POSITION ||
               key_id == DCC_JSON_KEY_RATE_LIMIT_PER_USER ||
               key_id == DCC_JSON_KEY_BITRATE ||
               key_id == DCC_JSON_KEY_USER_LIMIT) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        if (key_id == DCC_JSON_KEY_TYPE) {
            thread->type = value;
        } else if (key_id == DCC_JSON_KEY_POSITION) {
            thread->position = value;
        } else if (key_id == DCC_JSON_KEY_RATE_LIMIT_PER_USER) {
            thread->rate_limit_per_user = value;
        } else if (key_id == DCC_JSON_KEY_BITRATE) {
            thread->bitrate = value;
        } else {
            thread->user_limit = value;
        }
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
