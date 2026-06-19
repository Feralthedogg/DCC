#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_entitlement_body_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_build_test_entitlement_body(
    const dcc_test_entitlement_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->application_id == 0 ||
        params->sku_id == 0 ||
        params->owner_id == 0 ||
        params->owner_type == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "sku_id", params->sku_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "owner_id", params->owner_id);
    }
    if (status == DCC_OK) {
        if (!first) {
            status = dcc_rest_buffer_append_cstr(&body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\"owner_type\":");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_u64_text(&body, params->owner_type);
        }
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
