#include "internal/rest/dcc_rest_builders_internal.h"

dcc_status_t dcc_rest_build_scheduled_event_body(
    const dcc_scheduled_event_params_t *params,
    uint8_t include_id,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_rest_scheduled_event_body_context_t context;
    dcc_status_t status = dcc_rest_scheduled_event_prepare_body_context(params, include_id, &context);
    if (status != DCC_OK) {
        return status;
    }

    dcc_rest_buffer_t body = {0};
    status = dcc_rest_scheduled_event_append_body(&body, &context);
    if (status == DCC_OK) {
        *out = body.data;
        return DCC_OK;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
