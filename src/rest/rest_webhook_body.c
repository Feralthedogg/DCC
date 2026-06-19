#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_webhook_body_internal.h"

dcc_status_t dcc_rest_build_webhook_body(
    const dcc_webhook_params_t *params,
    int include_channel_id,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL || params->size < sizeof(*params)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(
            &body,
            &first,
            "name",
            params->name != NULL ? params->name : ""
        );
    }
    if (status == DCC_OK && include_channel_id && params->channel_id != 0) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "channel_id", params->channel_id);
    }
    if (status == DCC_OK && params->avatar != NULL && params->avatar[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "avatar", params->avatar);
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
