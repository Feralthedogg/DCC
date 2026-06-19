#include "internal/rest/dcc_rest_builders_internal.h"

dcc_status_t dcc_rest_build_auto_moderation_actions(
    const dcc_auto_moderation_action_params_t *actions,
    size_t action_count,
    dcc_rest_buffer_t *body
) {
    if (body == NULL || (action_count != 0 && actions == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "[");
    for (size_t i = 0; i < action_count && status == DCC_OK; ++i) {
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_build_auto_moderation_action(&actions[i], body);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "]");
    }
    return status;
}
