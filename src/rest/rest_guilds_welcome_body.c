#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_welcome_screen_internal.h"

dcc_status_t dcc_rest_build_welcome_screen_body(
    const dcc_welcome_screen_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        (params->welcome_channel_count != 0 && params->welcome_channels == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK && params->description != NULL && params->description[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "description", params->description);
    }
    if (status == DCC_OK && params->welcome_channel_count != 0) {
        status = dcc_rest_json_member_sep(&body, &first);
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\"welcome_channels\":[");
        }
        for (size_t i = 0; i < params->welcome_channel_count && status == DCC_OK; ++i) {
            const dcc_welcome_channel_params_t *channel = &params->welcome_channels[i];
            if (channel->channel_id == 0) {
                status = DCC_ERR_INVALID_ARG;
                break;
            }
            if (i != 0) {
                status = dcc_rest_buffer_append_cstr(&body, ",");
            }
            if (status == DCC_OK) {
                status = dcc_rest_buffer_append_cstr(&body, "{");
            }
            int channel_first = 1;
            if (status == DCC_OK) {
                status = dcc_rest_json_append_u64_string_member(&body, &channel_first, "channel_id", channel->channel_id);
            }
            if (status == DCC_OK) {
                status = dcc_rest_json_append_string_member(
                    &body,
                    &channel_first,
                    "description",
                    channel->description != NULL ? channel->description : ""
                );
            }
            if (status == DCC_OK && channel->emoji_id != 0) {
                status = dcc_rest_json_append_u64_string_member(&body, &channel_first, "emoji_id", channel->emoji_id);
            }
            if (status == DCC_OK && channel->emoji_name != NULL && channel->emoji_name[0] != '\0') {
                status = dcc_rest_json_append_string_member(&body, &channel_first, "emoji_name", channel->emoji_name);
            }
            if (status == DCC_OK) {
                status = dcc_rest_buffer_append_cstr(&body, "}");
            }
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "]");
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(&body, &first, "enabled", params->enabled);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out = body.data;
        return DCC_OK;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
