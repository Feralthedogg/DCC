#include "internal/rest/dcc_rest_onboarding_body_internal.h"
#include "internal/rest/dcc_rest_onboarding_prompt_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_build_onboarding_body(
    const dcc_onboarding_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->guild_id == 0 ||
        params->mode > DCC_ONBOARDING_MODE_ADVANCED ||
        (params->prompt_count != 0 && params->prompts == NULL) ||
        (params->default_channel_id_count != 0 && params->default_channel_ids == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;

    if (status == DCC_OK && params->prompt_count != 0) {
        status = dcc_rest_json_member_sep(&body, &first);
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\"prompts\":[");
        }
        for (size_t i = 0; i < params->prompt_count && status == DCC_OK; ++i) {
            if (i != 0) {
                status = dcc_rest_buffer_append_cstr(&body, ",");
            }
            if (status == DCC_OK) {
                status = dcc_rest_append_onboarding_prompt(&body, &params->prompts[i]);
            }
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "]");
        }
    }
    if (status == DCC_OK && params->default_channel_id_count != 0) {
        status = dcc_rest_json_append_snowflake_string_array_member(
            &body,
            &first,
            "default_channel_ids",
            params->default_channel_ids,
            params->default_channel_id_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(&body, &first, "enabled", params->enabled);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(&body, &first, "mode", (uint64_t)params->mode);
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
