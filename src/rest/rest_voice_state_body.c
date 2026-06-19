#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_voice_state_body_internal.h"

dcc_status_t dcc_rest_build_voice_state_body(
    const dcc_voice_state_params_t *params,
    int include_request_to_speak_timestamp,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL || params->size < sizeof(*params) || params->guild_id == 0 || params->channel_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "channel_id", params->channel_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(&body, &first, "suppress", params->suppress);
    }
    if (status == DCC_OK && include_request_to_speak_timestamp) {
        status = dcc_rest_json_append_nullable_string_member(
            &body,
            &first,
            "request_to_speak_timestamp",
            params->request_to_speak_timestamp
        );
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
