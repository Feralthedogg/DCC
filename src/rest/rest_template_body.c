#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_template_body_internal.h"

dcc_status_t dcc_rest_build_template_body(
    const dcc_template_params_t *params,
    dcc_rest_template_body_mode_t mode,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->name == NULL ||
        params->name[0] == '\0' ||
        (mode == DCC_REST_TEMPLATE_BODY_TEMPLATE &&
         (params->description == NULL || params->description[0] == '\0'))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
    }
    if (status == DCC_OK && mode == DCC_REST_TEMPLATE_BODY_TEMPLATE) {
        status = dcc_rest_json_append_string_member(&body, &first, "description", params->description);
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
