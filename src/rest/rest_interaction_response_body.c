#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_interaction_response_body_internal.h"

dcc_status_t dcc_rest_build_interaction_response_message_body(
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (type == 0 || message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *message_json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &message_json);
    if (status != DCC_OK) {
        return status;
    }

    dcc_rest_buffer_t body = {0};
    status = dcc_rest_buffer_append_cstr(&body, "{\"type\":");
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_u64_text(&body, (uint64_t)type);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, ",\"data\":");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, message_json);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    dcc_message_builder_json_free(message_json);
    return status;
}

dcc_status_t dcc_rest_build_interaction_response_type_body(
    dcc_interaction_response_type_t type,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (type == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"type\":");
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_u64_text(&body, (uint64_t)type);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
