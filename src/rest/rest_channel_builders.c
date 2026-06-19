#include "internal/rest/dcc_rest_channel_body_internal.h"
#include "internal/rest/dcc_rest_channel_builder_context_internal.h"
#include "internal/rest/dcc_rest_channel_builder_json_internal.h"

dcc_status_t dcc_rest_build_channel_body(
    const dcc_channel_params_t *params,
    uint8_t include_id,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    dcc_rest_channel_build_context_t ctx;
    dcc_status_t status = dcc_rest_channel_build_context_init(&ctx, params, include_id);
    if (status != DCC_OK) {
        return status;
    }

    dcc_rest_buffer_t body = {0};
    status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_channel_append_body_members(&body, &first, &ctx);
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
