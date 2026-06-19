#include "internal/rest/dcc_rest_guild_prune_body_internal.h"

#include "internal/rest/dcc_rest_buffer_internal.h"

dcc_status_t dcc_rest_build_guild_prune_body(
    uint32_t days,
    const dcc_snowflake_t *include_roles,
    size_t include_role_count,
    uint8_t compute_prune_count,
    char **out
) {
    if (out == NULL || (include_role_count != 0 && include_roles == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int needs_comma = 0;
    if (include_role_count != 0 && status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "\"include_roles\":");
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_snowflake_json_string_array(&body, include_roles, include_role_count);
        }
        needs_comma = 1;
    }
    if (status == DCC_OK) {
        if (needs_comma) {
            status = dcc_rest_buffer_append_cstr(&body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\"compute_prune_count\":");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, compute_prune_count != 0 ? "true" : "false");
        }
        needs_comma = 1;
    }
    if (status == DCC_OK) {
        if (needs_comma) {
            status = dcc_rest_buffer_append_cstr(&body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\"days\":");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_u64_text(&body, days);
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
