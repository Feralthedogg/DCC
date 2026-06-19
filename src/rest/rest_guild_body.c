#include "internal/rest/dcc_rest_guild_body_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_build_guild_body(
    const dcc_guild_params_t *params,
    int with_id,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (!dcc_rest_guild_params_valid(params, with_id)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;

    if (status == DCC_OK && with_id) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "id", params->guild_id);
    }
    if (status == DCC_OK && params->name != NULL && params->name[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(&body, &first, "widget_enabled", params->widget_enabled);
    }
    if (status == DCC_OK && params->afk_channel_id != 0) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "afk_channel_id", params->afk_channel_id);
    }
    if (status == DCC_OK && params->afk_timeout != DCC_GUILD_AFK_OFF) {
        status = dcc_rest_json_append_u64_member(&body, &first, "afk_timeout", (uint64_t)params->afk_timeout);
    }
    if (status == DCC_OK && params->widget_enabled) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "widget_channel_id", params->widget_channel_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(
            &body,
            &first,
            "default_message_notifications",
            (uint64_t)params->default_message_notifications
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(
            &body,
            &first,
            "explicit_content_filter",
            (uint64_t)params->explicit_content_filter
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(&body, &first, "mfa_level", (uint64_t)params->mfa_level);
    }
    if (status == DCC_OK && params->system_channel_id != 0) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "system_channel_id", params->system_channel_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(
            &body,
            &first,
            "premium_progress_bar_enabled",
            params->premium_progress_bar_enabled
        );
    }
    if (status == DCC_OK && params->rules_channel_id != 0) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "rules_channel_id", params->rules_channel_id);
    }
    if (status == DCC_OK && params->vanity_url_code != NULL && params->vanity_url_code[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "vanity_url_code", params->vanity_url_code);
    }
    if (status == DCC_OK && params->description != NULL && params->description[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "description", params->description);
    }
    if (status == DCC_OK) {
        status = dcc_rest_guild_body_append_tail_members(&body, &first, params);
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
