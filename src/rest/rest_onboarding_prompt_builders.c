#include "internal/rest/dcc_rest_onboarding_option_builders_internal.h"
#include "internal/rest/dcc_rest_onboarding_prompt_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_append_onboarding_prompt(
    dcc_rest_buffer_t *body,
    const dcc_onboarding_prompt_params_t *prompt
) {
    if (body == NULL ||
        prompt == NULL ||
        prompt->size < sizeof(*prompt) ||
        prompt->type > DCC_ONBOARDING_PROMPT_DROPDOWN ||
        (prompt->option_count != 0 && prompt->options == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(body, &first, "type", (uint64_t)prompt->type);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(
            body,
            &first,
            "title",
            prompt->title != NULL ? prompt->title : ""
        );
    }
    if (status == DCC_OK && prompt->option_count != 0) {
        status = dcc_rest_json_member_sep(body, &first);
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(body, "\"options\":[");
        }
        for (size_t i = 0; i < prompt->option_count && status == DCC_OK; ++i) {
            if (i != 0) {
                status = dcc_rest_buffer_append_cstr(body, ",");
            }
            if (status == DCC_OK) {
                status = dcc_rest_append_onboarding_option(body, &prompt->options[i]);
            }
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(body, "]");
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(
            body,
            &first,
            "single_select",
            (prompt->flags & DCC_ONBOARDING_PROMPT_SINGLE_SELECT) != 0
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(
            body,
            &first,
            "required",
            (prompt->flags & DCC_ONBOARDING_PROMPT_REQUIRED) != 0
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(
            body,
            &first,
            "in_onboarding",
            (prompt->flags & DCC_ONBOARDING_PROMPT_IN_ONBOARDING) != 0
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}
