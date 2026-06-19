#include "internal/rest/dcc_rest_onboarding_option_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

static dcc_status_t dcc_rest_json_append_onboarding_emoji_member(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_onboarding_emoji_params_t *emoji
) {
    if (emoji != NULL &&
        (emoji->size < sizeof(*emoji) || (emoji->role_count != 0 && emoji->roles == NULL))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_json_member_sep(body, first);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "\"emoji\":{");
    }

    int emoji_first = 1;
    const char *name = emoji != NULL && emoji->name != NULL ? emoji->name : "";
    const char *image = emoji != NULL ? emoji->image : NULL;
    const dcc_snowflake_t *roles = emoji != NULL ? emoji->roles : NULL;
    size_t role_count = emoji != NULL ? emoji->role_count : 0U;

    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(body, &emoji_first, "name", name);
    }
    if (status == DCC_OK && image != NULL && image[0] != '\0') {
        status = dcc_rest_json_append_string_member(body, &emoji_first, "image", image);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_snowflake_string_array_member(body, &emoji_first, "roles", roles, role_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

dcc_status_t dcc_rest_append_onboarding_option(
    dcc_rest_buffer_t *body,
    const dcc_onboarding_prompt_option_params_t *option
) {
    if (body == NULL ||
        option == NULL ||
        option->size < sizeof(*option) ||
        (option->channel_id_count != 0 && option->channel_ids == NULL) ||
        (option->role_id_count != 0 && option->role_ids == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_onboarding_emoji_member(body, &first, option->emoji);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(
            body,
            &first,
            "title",
            option->title != NULL ? option->title : ""
        );
    }
    if (status == DCC_OK && option->description != NULL && option->description[0] != '\0') {
        status = dcc_rest_json_append_string_member(body, &first, "description", option->description);
    }
    if (status == DCC_OK && option->channel_id_count != 0) {
        status = dcc_rest_json_append_snowflake_string_array_member(
            body,
            &first,
            "channel_ids",
            option->channel_ids,
            option->channel_id_count
        );
    }
    if (status == DCC_OK && option->role_id_count != 0) {
        status = dcc_rest_json_append_snowflake_string_array_member(
            body,
            &first,
            "role_ids",
            option->role_ids,
            option->role_id_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}
