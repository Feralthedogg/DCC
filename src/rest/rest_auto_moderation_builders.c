#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_build_auto_moderation_rule_body(
    const dcc_auto_moderation_rule_params_t *params,
    uint8_t include_id,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        (include_id != 0 && params->rule_id == 0) ||
        params->name == NULL ||
        params->name[0] == '\0' ||
        params->event_type != DCC_AUTO_MODERATION_EVENT_MESSAGE_SEND ||
        params->trigger_type < DCC_AUTO_MODERATION_TRIGGER_KEYWORD ||
        params->trigger_type > DCC_AUTO_MODERATION_TRIGGER_MENTION_SPAM ||
        params->trigger_metadata.size < sizeof(params->trigger_metadata) ||
        (params->action_count != 0 && params->actions == NULL) ||
        (params->exempt_role_count != 0 && params->exempt_roles == NULL) ||
        (params->exempt_channel_count != 0 && params->exempt_channels == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK && include_id != 0) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "id", params->rule_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(&body, &first, "enabled", params->enabled);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(&body, &first, "event_type", (uint64_t)params->event_type);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(&body, &first, "trigger_type", (uint64_t)params->trigger_type);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_member_sep(&body, &first);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "\"trigger_metadata\":");
    }
    if (status == DCC_OK) {
        status = dcc_rest_build_auto_moderation_trigger_metadata(&params->trigger_metadata, &body);
    }
    if (status == DCC_OK && params->action_count != 0) {
        status = dcc_rest_json_member_sep(&body, &first);
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\"actions\":");
        }
        if (status == DCC_OK) {
            status = dcc_rest_build_auto_moderation_actions(params->actions, params->action_count, &body);
        }
    }
    if (status == DCC_OK && params->exempt_role_count != 0) {
        status = dcc_rest_json_append_snowflake_string_array_member(
            &body,
            &first,
            "exempt_roles",
            params->exempt_roles,
            params->exempt_role_count
        );
    }
    if (status == DCC_OK && params->exempt_channel_count != 0) {
        status = dcc_rest_json_append_snowflake_string_array_member(
            &body,
            &first,
            "exempt_channels",
            params->exempt_channels,
            params->exempt_channel_count
        );
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
